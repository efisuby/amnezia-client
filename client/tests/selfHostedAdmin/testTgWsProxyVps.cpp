#include <QTest>
#include <QUuid>

#include <libssh/libssh.h>

#include "core/installers/tgWsProxyInstaller.h"
#include "core/models/selfhosted/selfHostedAdminServerConfig.h"
#include "core/utils/selfhosted/sshSession.h"
#include "secureQSettings.h"
#include "utils/testCoreController.h"
#include "utils/testUtils.h"
#include "vpnConnection.h"

using namespace amnezia;
using namespace amnezia::test;

class TestTgWsProxyVps : public QObject
{
    Q_OBJECT

private:
    TestCoreController *m_coreController = nullptr;
    SecureQSettings *m_settings = nullptr;

    ServerCredentials credentialsFromEnv() const
    {
        ServerCredentials credentials;
        credentials.hostName = getEnvValue("SELF_HOSTED_SERVER_HOST_NAME");
        credentials.userName = getEnvValue("SELF_HOSTED_SERVER_USER_NAME");
        credentials.secretData = getEnvValue("SELF_HOSTED_SERVER_PASSWORD");
        credentials.port = getEnvValue("SELF_HOSTED_SERVER_SSH_PORT").toInt();
        return credentials;
    }

    QString runOnHost(const ServerCredentials &credentials, const QString &command)
    {
        QString output;
        auto collect = [&output](const QString &data, libssh::Client &) {
            output += data;
            return ErrorCode::NoError;
        };
        SshSession sshSession;
        const ErrorCode errorCode = sshSession.runScript(credentials, command, collect, collect);
        return errorCode == ErrorCode::NoError ? output.trimmed() : QString();
    }

    QString runInContainer(const ServerCredentials &credentials, const QString &command)
    {
        QString output;
        auto collect = [&output](const QString &data, libssh::Client &) {
            output += data;
            return ErrorCode::NoError;
        };
        SshSession sshSession;
        const ErrorCode errorCode = sshSession.runScript(
                credentials, QStringLiteral("sudo docker exec -i amnezia-tgwsproxy sh -c '%1'").arg(command), collect, collect);
        return errorCode == ErrorCode::NoError ? output.trimmed() : QString();
    }

    TgWsProxyProtocolConfig installedConfig(const QString &serverId) const
    {
        const auto adminConfig = m_coreController->m_serversRepository->selfHostedAdminConfig(serverId);
        const auto *config = adminConfig->containerConfig(DockerContainer::TgWsProxy).getTgWsProxyProtocolConfig();
        return config ? *config : TgWsProxyProtocolConfig {};
    }

private slots:
    void initTestCase()
    {
        m_settings = new SecureQSettings("AmneziaVPN-Test-" + QUuid::createUuid().toString(), "amnezia-client", nullptr, false);
        m_coreController = new TestCoreController(QSharedPointer<VpnConnection>::create(nullptr, nullptr), m_settings, nullptr, this);
        ssh_init();
    }

    void cleanupTestCase()
    {
        ssh_finalize();
        m_settings->clearSettings();
        delete m_coreController;
        delete m_settings;
    }

    void testInstallUpdateAndScan()
    {
        const ServerCredentials credentials = credentialsFromEnv();
        if (credentials.hostName.isEmpty() || credentials.userName.isEmpty() || credentials.secretData.isEmpty()) {
            QSKIP("Set SELF_HOSTED_SERVER_HOST_NAME, SELF_HOSTED_SERVER_USER_NAME, SELF_HOSTED_SERVER_PASSWORD");
        }

        bool wasInstalled = false;
        QCOMPARE(m_coreController->m_installController->installServer(credentials, DockerContainer::TgWsProxy, 18443,
                                                                      TransportProto::Tcp, wasInstalled),
                 ErrorCode::NoError);
        QVERIFY(wasInstalled);

        const QString serverId = m_coreController->m_serversRepository->serverIdAt(0);
        const TgWsProxyProtocolConfig installed = installedConfig(serverId);
        QCOMPARE(installed.port, QStringLiteral("18443"));
        QCOMPARE(runInContainer(credentials, "cat /data/secret"), installed.secret);
        QVERIFY(!runInContainer(credentials, "pidof tg-ws-proxy").isEmpty());

        ContainerConfig oldConfig = m_coreController->m_serversRepository->selfHostedAdminConfig(serverId)->containerConfig(
                DockerContainer::TgWsProxy);
        ContainerConfig newConfig = oldConfig;
        newConfig.getTgWsProxyProtocolConfig()->port = QStringLiteral("18444");
        newConfig.getTgWsProxyProtocolConfig()->fakeTlsDomain = QStringLiteral("dzen.ru");
        QCOMPARE(m_coreController->m_installController->updateServerConfig(serverId, DockerContainer::TgWsProxy, oldConfig, newConfig),
                 ErrorCode::NoError);

        QCOMPARE(runInContainer(credentials, "cat /data/secret"), installed.secret);
        QCOMPARE(runInContainer(credentials, "cat /data/port"), QStringLiteral("18444"));
        QCOMPARE(runInContainer(credentials, "cat /data/fake_tls_domain"), QStringLiteral("dzen.ru"));
        QVERIFY(!runInContainer(credentials, "pidof tg-ws-proxy").isEmpty());
        QVERIFY(runOnHost(credentials, "timeout 5 bash -c '</dev/tcp/127.0.0.1/18444' && echo port_open").contains("port_open"));

        SshSession sshSession;
        TgWsProxyInstaller installer;
        ContainerConfig scanned = installer.createBaseConfig(DockerContainer::TgWsProxy, 18444, TransportProto::Tcp);
        QCOMPARE(installer.extractConfigFromContainer(DockerContainer::TgWsProxy, credentials, &sshSession, scanned), ErrorCode::NoError);
        QCOMPARE(scanned.getTgWsProxyProtocolConfig()->secret, installed.secret);
        QCOMPARE(scanned.getTgWsProxyProtocolConfig()->fakeTlsDomain, QStringLiteral("dzen.ru"));

        QCOMPARE(m_coreController->m_installController->removeAllContainers(serverId), ErrorCode::NoError);
    }
};

QTEST_MAIN(TestTgWsProxyVps)
#include "testTgWsProxyVps.moc"
