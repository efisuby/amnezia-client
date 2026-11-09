#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcessEnvironment>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QUuid>
#include <QTest>

#include "utils/testCoreController.h"
#include "core/models/serverDescription.h"
#include "secureQSettings.h"
#include "vpnConnection.h"

using namespace amnezia;

class TestUiIpModelAndController : public QObject
{
    Q_OBJECT

private:
    TestCoreController *m_coreController;
    SecureQSettings *m_settings;

    QString normalizeHostname(const QString &hostname) const
    {
        QString normalized = hostname;
        normalized.replace("https://", "");
        normalized.replace("http://", "");
        normalized.replace("ftp://", "");
        normalized = normalized.split("/", Qt::SkipEmptyParts).first();
        return normalized;
    }

private slots:
    void initTestCase()
    {
        QString testOrg = "AmneziaVPN-Test-" + QUuid::createUuid().toString();
        m_settings = new SecureQSettings(testOrg, "amnezia-client", nullptr, false);

        auto vpnConnection = QSharedPointer<VpnConnection>::create(nullptr, nullptr);

        m_coreController = new TestCoreController(vpnConnection, m_settings, nullptr, this);
    }

    void cleanupTestCase()
    {
        m_settings->clearSettings();
        delete m_coreController;
        delete m_settings;
    }

    void init()
    {
        m_settings->clearSettings();
        if (m_coreController->m_serversModel) {
            m_coreController->m_serversModel->updateModel(QVector<ServerDescription>(), QString{});
        }
    }

    void testRolesAndSignals()
    {
        QSignalSpy finishedSpy(m_coreController->m_ipSplitTunnelingUiController, &IpSplitTunnelingUiController::finished);
        QSignalSpy errorOccurredSpy(m_coreController->m_ipSplitTunnelingUiController, &IpSplitTunnelingUiController::errorOccurred);
        QSignalSpy isSplitTunnelingChangedSpy(m_coreController->m_ipSplitTunnelingUiController, &IpSplitTunnelingUiController::isSplitTunnelingEnabledChanged);

        m_coreController->m_ipSplitTunnelingUiController->toggleSplitTunneling(true);
        QVERIFY2(isSplitTunnelingChangedSpy.count() == 1, "isSplitTunnelingChangedSpy signal should be emitted");
        QVERIFY2(m_coreController->m_ipSplitTunnelingUiController->isSplitTunnelingEnabled() == true, "ipSplitTunneling should be enabled");

        m_coreController->m_ipSplitTunnelingUiController->toggleSplitTunneling(false);
        QVERIFY2(isSplitTunnelingChangedSpy.count() == 2, "isSplitTunnelingChangedSpy signal should be emitted 2nd time");
        QVERIFY2(m_coreController->m_ipSplitTunnelingUiController->isSplitTunnelingEnabled() == false, "ipSplitTunneling should be disabled");

        QString site = "2ip.io";

        m_coreController->m_ipSplitTunnelingUiController->addSite(site);
        m_coreController->m_ipSplitTunnelingUiController->addSite("whatismyipaddress.com");
        m_coreController->m_ipSplitTunnelingUiController->updateModel();
        QVERIFY2(finishedSpy.count() == 2, "finished signal should be emitted 2 times");
        QVERIFY2(m_coreController->m_ipSplitTunnelingModel->rowCount() == 2, "IpSplitTunnelingModel should have 2 rows");

        QModelIndex siteModelIndex = m_coreController->m_ipSplitTunnelingModel->index(0, 0);
        QVERIFY2(siteModelIndex.isValid(), "Ip model index should be valid");

        auto siteUrl = m_coreController->m_ipSplitTunnelingModel->data(siteModelIndex, IpSplitTunnelingModel::UrlRole);
        QCOMPARE(siteUrl, normalizeHostname(site));

        auto siteIp = m_coreController->m_ipSplitTunnelingModel->data(siteModelIndex, IpSplitTunnelingModel::IpRole);
        QVERIFY2(siteIp.isNull() == false, "Ip should not be empty");

        m_coreController->m_ipSplitTunnelingUiController->removeSite(0);
        m_coreController->m_ipSplitTunnelingUiController->updateModel();
        QVERIFY2(finishedSpy.count() == 3, "finished signal should be emitted");
        QVERIFY2(m_coreController->m_ipSplitTunnelingModel->rowCount() == 1, "IpSplitTunnelingModel should have 1 row");

        QTemporaryDir tempDir;
        QVERIFY2(tempDir.isValid(), "Temporary directory should be created");

        const QString sitesFile = tempDir.filePath(QStringLiteral("sites.json"));
        {
            QFile file(sitesFile);
            QVERIFY2(file.open(QIODevice::WriteOnly), "Sites import file should be writable");
            file.write(R"([
                {"hostname": "example.com", "ip": "93.184.216.34"},
                {"hostname": "test.org", "ip": "1.2.3.4"}
            ])");
        }

        m_coreController->m_ipSplitTunnelingUiController->importSites(sitesFile, true);
        m_coreController->m_ipSplitTunnelingUiController->updateModel();
        QVERIFY2(errorOccurredSpy.count() == 0, "errorOccurred signal should not be emitted");
        QVERIFY2(finishedSpy.count() == 4, "finished signal should be emitted");
        QVERIFY2(m_coreController->m_ipSplitTunnelingModel->rowCount() > 1, "IpSplitTunnelingModel should have more than 1 row");

        const QString exportFile = tempDir.filePath(QStringLiteral("test_ips_export.json"));
        m_coreController->m_ipSplitTunnelingUiController->exportSites(exportFile);
        QVERIFY2(finishedSpy.count() == 5, "finished signal should be emitted");

        m_coreController->m_ipSplitTunnelingUiController->removeSites();
        m_coreController->m_ipSplitTunnelingUiController->updateModel();
        QVERIFY2(finishedSpy.count() == 6, "finished signal should be emitted");
        QVERIFY2(m_coreController->m_ipSplitTunnelingModel->rowCount() == 0, "IpSplitTunnelingModel should have 0 rows");
    }

    void testReplaceSitesFromDownloadedList()
    {
        auto *controller = m_coreController->m_ipSplitTunnelingController;
        auto *repository = m_coreController->m_appSettingsRepository;

        controller->setRouteMode(RouteMode::VpnOnlyForwardSites);
        repository->addVpnSite(RouteMode::VpnOnlyForwardSites, "manual.example.com", { "10.0.0.1" });
        repository->addVpnSite(RouteMode::VpnAllExceptSites, "direct.example.com", { "10.0.0.2" });

        const QByteArray downloadedList = R"([
            {"hostname": "1.1.1.1\/32", "ip": ""},
            {"hostname": "1.9.0.0\/16", "ip": ""},
            {"hostname": "2.16.0.0\/14", "ip": ""}
        ])";

        QString errorMessage;
        QVERIFY(controller->replaceSitesFromJson(RouteMode::VpnOnlyForwardSites, downloadedList, errorMessage));

        const QVariantMap forwardSites = repository->vpnSites(RouteMode::VpnOnlyForwardSites);
        QCOMPARE(forwardSites.size(), 3);
        QVERIFY(forwardSites.contains("1.1.1.1/32"));
        QVERIFY(forwardSites.contains("1.9.0.0/16"));
        QVERIFY(forwardSites.contains("2.16.0.0/14"));
        QVERIFY(!forwardSites.contains("manual.example.com"));
        QCOMPARE(controller->getCurrentSites().size(), 3);

        QVERIFY(repository->vpnSites(RouteMode::VpnAllExceptSites).contains("direct.example.com"));

        QVERIFY(!controller->replaceSitesFromJson(RouteMode::VpnOnlyForwardSites, "not json", errorMessage));
        QVERIFY(!controller->replaceSitesFromJson(RouteMode::VpnOnlyForwardSites, "[]", errorMessage));
        QCOMPARE(repository->vpnSites(RouteMode::VpnOnlyForwardSites).size(), 3);
    }

    void testSitesListAutoUpdateConditions()
    {
        auto *controller = m_coreController->m_ipSplitTunnelingController;
        auto *repository = m_coreController->m_appSettingsRepository;

        QVERIFY(!controller->isSitesListAutoUpdateEnabled());
        QCOMPARE(controller->sitesListUrl(), QStringLiteral("https://iplist.opencck.org/?format=amnezia&data=cidr4"));

        controller->setRouteMode(RouteMode::VpnOnlyForwardSites);
        controller->toggleSplitTunneling(true);
        QVERIFY(!controller->isSitesListAutoUpdateNeeded());

        controller->setSitesListAutoUpdateEnabled(true);
        QVERIFY(controller->isSitesListAutoUpdateNeeded());

        controller->setRouteMode(RouteMode::VpnAllExceptSites);
        QVERIFY(!controller->isSitesListAutoUpdateNeeded());
        controller->setRouteMode(RouteMode::VpnOnlyForwardSites);

        controller->toggleSplitTunneling(false);
        QVERIFY(!controller->isSitesListAutoUpdateNeeded());
        controller->toggleSplitTunneling(true);

        controller->setSitesListUrl("  ");
        QVERIFY(repository->sitesListUrl().isEmpty());
        QVERIFY(!controller->isSitesListAutoUpdateNeeded());
    }
};

QTEST_MAIN(TestUiIpModelAndController)
#include "testUiIpModelAndController.moc"
