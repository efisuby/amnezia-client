#include "tgWsProxyInstaller.h"

#include <QRandomGenerator>

#include "core/models/protocols/tgWsProxyProtocolConfig.h"
#include "core/utils/constants/protocolConstants.h"
#include "core/utils/selfhosted/sshSession.h"

using namespace amnezia;

TgWsProxyInstaller::TgWsProxyInstaller(QObject *parent) : InstallerBase(parent)
{
}

ContainerConfig TgWsProxyInstaller::generateConfig(DockerContainer container, int port, TransportProto transportProto)
{
    ContainerConfig config = createBaseConfig(container, port, transportProto);

    if (auto *tgWsProxyConfig = config.getTgWsProxyProtocolConfig()) {
        QByteArray secret(16, Qt::Uninitialized);
        QRandomGenerator::system()->generate(secret.begin(), secret.end());
        tgWsProxyConfig->secret = QString::fromLatin1(secret.toHex());
    }

    return config;
}

ErrorCode TgWsProxyInstaller::extractConfigFromContainer(DockerContainer container, const ServerCredentials &credentials,
                                                         SshSession *sshSession, ContainerConfig &config)
{
    auto *tgWsProxyConfig = config.getTgWsProxyProtocolConfig();
    if (!tgWsProxyConfig || !sshSession) {
        return ErrorCode::NoError;
    }

    ErrorCode errorCode = ErrorCode::NoError;
    const QString secret = QString::fromUtf8(sshSession->getTextFileFromContainer(
            container, credentials, QString::fromLatin1(protocols::tgWsProxy::secretPath), errorCode)).trimmed();
    if (errorCode == ErrorCode::NoError && !secret.isEmpty()) {
        tgWsProxyConfig->secret = secret;
    }

    const QString fakeTlsDomain = QString::fromUtf8(sshSession->getTextFileFromContainer(
            container, credentials, QString::fromLatin1(protocols::tgWsProxy::fakeTlsDomainPath), errorCode)).trimmed();
    if (errorCode == ErrorCode::NoError) {
        tgWsProxyConfig->fakeTlsDomain = fakeTlsDomain;
    }

    return ErrorCode::NoError;
}
