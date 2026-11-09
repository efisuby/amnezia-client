#include <QTest>

#include "core/models/selfhosted/selfHostedAdminServerConfig.h"
#include "core/utils/constants/protocolConstants.h"
#include "core/utils/containers/containerUtils.h"
#include "core/utils/protocolEnum.h"

using namespace amnezia;

class TestDnsAdblockContainer : public QObject
{
    Q_OBJECT

private slots:
    void testContainerName()
    {
        QCOMPARE(ContainerUtils::containerToString(DockerContainer::DnsAdblock), QStringLiteral("amnezia-dnsadblock"));
        QCOMPARE(ContainerUtils::containerFromString("amnezia-dnsadblock"), DockerContainer::DnsAdblock);
        QCOMPARE(ContainerUtils::containerFromString("amnezia-dns"), DockerContainer::Dns);
    }

    void testDnsContainerProperties()
    {
        QVERIFY(ContainerUtils::isDnsContainer(DockerContainer::Dns));
        QVERIFY(ContainerUtils::isDnsContainer(DockerContainer::DnsAdblock));
        QVERIFY(!ContainerUtils::isDnsContainer(DockerContainer::Sftp));

        QCOMPARE(ContainerUtils::defaultProtocol(DockerContainer::DnsAdblock), Proto::Dns);
        QCOMPARE(ContainerUtils::containerService(DockerContainer::DnsAdblock), ServiceType::Other);
        QVERIFY(!ContainerUtils::isShareable(DockerContainer::DnsAdblock));
    }

    void testAmneziaDnsAddressWithAdblockContainer()
    {
        SelfHostedAdminServerConfig config;
        config.containers.insert(DockerContainer::DnsAdblock, ContainerConfig());

        const QPair<QString, QString> enabled = config.getDnsPair(true, "1.1.1.1", "1.0.0.1");
        QCOMPARE(enabled.first, QString::fromLatin1(protocols::dns::amneziaDnsIp));
        QCOMPARE(enabled.second, QStringLiteral("1.0.0.1"));

        const QPair<QString, QString> disabled = config.getDnsPair(false, "1.1.1.1", "1.0.0.1");
        QCOMPARE(disabled.first, QStringLiteral("1.1.1.1"));
    }
};

QTEST_MAIN(TestDnsAdblockContainer)
#include "testDnsAdblockContainer.moc"
