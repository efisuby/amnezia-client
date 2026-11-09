#include "tgWsProxyConfigModel.h"

using namespace amnezia;

TgWsProxyConfigModel::TgWsProxyConfigModel(QObject *parent) : QObject(parent)
{
}

QString TgWsProxyConfigModel::port() const
{
    return m_protocolConfig.port;
}

void TgWsProxyConfigModel::setPort(const QString &value)
{
    if (m_protocolConfig.port == value) {
        return;
    }
    m_protocolConfig.port = value;
    emit changed();
}

QString TgWsProxyConfigModel::secret() const
{
    return m_protocolConfig.secret;
}

QString TgWsProxyConfigModel::fakeTlsDomain() const
{
    return m_protocolConfig.fakeTlsDomain;
}

void TgWsProxyConfigModel::setFakeTlsDomain(const QString &value)
{
    if (m_protocolConfig.fakeTlsDomain == value) {
        return;
    }
    m_protocolConfig.fakeTlsDomain = value;
    emit changed();
}

QString TgWsProxyConfigModel::proxyLink(const QString &hostName) const
{
    return m_protocolConfig.proxyLink(hostName);
}

void TgWsProxyConfigModel::updateModel(DockerContainer container, const TgWsProxyProtocolConfig &protocolConfig)
{
    Q_UNUSED(container)
    m_protocolConfig = protocolConfig;
    emit changed();
}

TgWsProxyProtocolConfig TgWsProxyConfigModel::getProtocolConfig()
{
    return m_protocolConfig;
}
