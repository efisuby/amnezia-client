#include <QMetaEnum>
#include <QTest>

#include "core/utils/containers/containerUtils.h"
#include "core/utils/selfhosted/scriptsRegistry.h"

using namespace amnezia;

class TestServerMigrationStatePaths : public QObject
{
    Q_OBJECT

private slots:
    void testStatePathsAreUsedByContainerScripts()
    {
        const QMetaEnum containers = QMetaEnum::fromType<DockerContainer>();
        for (int i = 0; i < containers.keyCount(); ++i) {
            const auto container = static_cast<DockerContainer>(containers.value(i));
            const QStringList paths = ContainerUtils::migrationStatePaths(container);
            if (paths.isEmpty()) {
                continue;
            }

            QString scripts;
            for (const auto type : { ProtocolScriptType::dockerfile, ProtocolScriptType::run_container,
                                     ProtocolScriptType::configure_container, ProtocolScriptType::container_startup }) {
                scripts += scriptData(type, container);
            }
            for (const QString &path : paths) {
                QVERIFY2(scripts.contains(path), qPrintable(ContainerUtils::containerToString(container) + " " + path));
            }
        }
    }

    void testContainersWithoutState()
    {
        QVERIFY(ContainerUtils::migrationStatePaths(DockerContainer::Ipsec).isEmpty());
        QVERIFY(ContainerUtils::migrationStatePaths(DockerContainer::Dns).isEmpty());
        QVERIFY(ContainerUtils::migrationStatePaths(DockerContainer::DnsAdblock).isEmpty());
        QVERIFY(ContainerUtils::migrationStatePaths(DockerContainer::Sftp).isEmpty());
    }

    void testKeysAndUsersArePreserved()
    {
        QCOMPARE(ContainerUtils::migrationStatePaths(DockerContainer::Awg2), QStringList { "/opt/amnezia/awg" });
        QCOMPARE(ContainerUtils::migrationStatePaths(DockerContainer::Xray), QStringList { "/opt/amnezia/xray" });
        QCOMPARE(ContainerUtils::migrationStatePaths(DockerContainer::MtProxy), QStringList { "/data" });
    }
};

QTEST_MAIN(TestServerMigrationStatePaths)
#include "testServerMigrationStatePaths.moc"
