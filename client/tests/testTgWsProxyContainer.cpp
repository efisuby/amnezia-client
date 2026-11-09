#include <QTest>
#include <QUrlQuery>

#include "core/installers/tgWsProxyInstaller.h"
#include "core/models/containerConfig.h"
#include "core/protocols/protocolUtils.h"
#include "core/utils/containers/containerUtils.h"
#include "core/utils/selfhosted/scriptsRegistry.h"

using namespace amnezia;

class TestTgWsProxyContainer : public QObject
{
    Q_OBJECT

private:
    static QString scriptVar(const ScriptVars &vars, const QString &name)
    {
        for (const auto &var : vars) {
            if (var.first == name) {
                return var.second;
            }
        }
        return QString();
    }

private slots:
    void testContainerProperties()
    {
        QCOMPARE(ContainerUtils::containerToString(DockerContainer::TgWsProxy), QStringLiteral("amnezia-tgwsproxy"));
        QCOMPARE(ContainerUtils::containerFromString("amnezia-tgwsproxy"), DockerContainer::TgWsProxy);
        QCOMPARE(ContainerUtils::defaultProtocol(DockerContainer::TgWsProxy), Proto::TgWsProxy);
        QCOMPARE(ContainerUtils::containerService(DockerContainer::TgWsProxy), ServiceType::Other);
        QVERIFY(!ContainerUtils::isShareable(DockerContainer::TgWsProxy));
        QCOMPARE(scriptFolder(DockerContainer::TgWsProxy), QStringLiteral("tg_ws_proxy"));
        QVERIFY(!scriptData(ProtocolScriptType::dockerfile, DockerContainer::TgWsProxy).isEmpty());
        QVERIFY(!scriptData(ProtocolScriptType::container_startup, DockerContainer::TgWsProxy).isEmpty());
        QCOMPARE(ProtocolUtils::defaultPort(Proto::TgWsProxy), 1443);
        QCOMPARE(ProtocolUtils::defaultTransportProto(Proto::TgWsProxy), TransportProto::Tcp);
    }

    void testGeneratedConfig()
    {
        TgWsProxyInstaller installer;
        const ContainerConfig config = installer.generateConfig(DockerContainer::TgWsProxy, 2443, TransportProto::Tcp);
        const auto *tgWsProxyConfig = config.getTgWsProxyProtocolConfig();
        QVERIFY(tgWsProxyConfig);
        QCOMPARE(tgWsProxyConfig->port, QStringLiteral("2443"));
        QVERIFY(QRegularExpression(QStringLiteral("^[0-9a-f]{32}$")).match(tgWsProxyConfig->secret).hasMatch());
        QVERIFY(tgWsProxyConfig->fakeTlsDomain.isEmpty());

        const ContainerConfig other = installer.generateConfig(DockerContainer::TgWsProxy, 2443, TransportProto::Tcp);
        QVERIFY(other.getTgWsProxyProtocolConfig()->secret != tgWsProxyConfig->secret);
    }

    void testJsonRoundTrip()
    {
        TgWsProxyProtocolConfig config;
        config.port = QStringLiteral("443");
        config.secret = QStringLiteral("0123456789abcdef0123456789abcdef");
        config.fakeTlsDomain = QStringLiteral("dzen.ru");

        const ProtocolConfig restored = ProtocolConfig::fromJson(config.toJson(), Proto::TgWsProxy);
        QCOMPARE(restored.type(), Proto::TgWsProxy);
        const auto *restoredConfig = restored.as<TgWsProxyProtocolConfig>();
        QVERIFY(restoredConfig);
        QCOMPARE(restoredConfig->port, config.port);
        QCOMPARE(restoredConfig->secret, config.secret);
        QCOMPARE(restoredConfig->fakeTlsDomain, config.fakeTlsDomain);
    }

    void testProxyLink()
    {
        TgWsProxyProtocolConfig config;
        config.port = QStringLiteral("443");
        config.secret = QStringLiteral("0123456789abcdef0123456789abcdef");

        QCOMPARE(config.proxyLink(QStringLiteral("vpn.example.com")),
                 QStringLiteral("tg://proxy?server=vpn.example.com&port=443&secret=dd0123456789abcdef0123456789abcdef"));

        config.fakeTlsDomain = QStringLiteral("dzen.ru");
        QCOMPARE(config.proxyLink(QStringLiteral("vpn.example.com")),
                 QStringLiteral("tg://proxy?server=vpn.example.com&port=443&secret=ee0123456789abcdef0123456789abcdef647a656e2e7275"));

        QVERIFY(config.proxyLink(QString()).isEmpty());
    }

    void testScriptVars()
    {
        ContainerConfig containerConfig;
        TgWsProxyProtocolConfig config;
        config.secret = QStringLiteral("0123456789abcdef0123456789abcdef");
        config.fakeTlsDomain = QStringLiteral("dzen.ru");
        containerConfig.protocolConfig = config;

        const ScriptVars vars = genTgWsProxyVars(containerConfig);
        QCOMPARE(scriptVar(vars, "$TG_WS_PROXY_PORT"), QStringLiteral("1443"));
        QCOMPARE(scriptVar(vars, "$TG_WS_PROXY_SECRET"), config.secret);
        QCOMPARE(scriptVar(vars, "$TG_WS_PROXY_FAKE_TLS_DOMAIN"), config.fakeTlsDomain);
    }
};

QTEST_MAIN(TestTgWsProxyContainer)
#include "testTgWsProxyContainer.moc"
