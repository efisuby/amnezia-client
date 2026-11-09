#ifndef TGWSPROXYINSTALLER_H
#define TGWSPROXYINSTALLER_H

#include "installerBase.h"

class TgWsProxyInstaller : public InstallerBase
{
    Q_OBJECT
public:
    explicit TgWsProxyInstaller(QObject *parent = nullptr);

    amnezia::ContainerConfig generateConfig(amnezia::DockerContainer container, int port, amnezia::TransportProto transportProto) override;
    amnezia::ErrorCode extractConfigFromContainer(amnezia::DockerContainer container, const amnezia::ServerCredentials &credentials,
                                                  SshSession *sshSession, amnezia::ContainerConfig &config) override;
};

#endif // TGWSPROXYINSTALLER_H
