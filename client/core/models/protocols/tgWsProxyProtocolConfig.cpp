#include "tgWsProxyProtocolConfig.h"

#include <QUrlQuery>

#include "core/utils/constants/configKeys.h"
#include "core/utils/constants/protocolConstants.h"

namespace amnezia
{

QString TgWsProxyProtocolConfig::proxyLink(const QString &hostName) const
{
    if (hostName.isEmpty() || secret.isEmpty()) {
        return QString();
    }

    const QString clientSecret = fakeTlsDomain.isEmpty()
            ? QStringLiteral("dd") + secret
            : QStringLiteral("ee") + secret + QString::fromLatin1(fakeTlsDomain.toUtf8().toHex());

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("server"), hostName);
    query.addQueryItem(QStringLiteral("port"), port.isEmpty() ? QString(protocols::tgWsProxy::defaultPort) : port);
    query.addQueryItem(QStringLiteral("secret"), clientSecret);
    return QStringLiteral("tg://proxy?") + query.toString(QUrl::FullyEncoded);
}

QJsonObject TgWsProxyProtocolConfig::toJson() const
{
    QJsonObject obj;
    if (!port.isEmpty()) {
        obj[configKey::port] = port;
    }
    if (!secret.isEmpty()) {
        obj[protocols::tgWsProxy::secretKey] = secret;
    }
    if (!fakeTlsDomain.isEmpty()) {
        obj[protocols::tgWsProxy::fakeTlsDomainKey] = fakeTlsDomain;
    }
    return obj;
}

TgWsProxyProtocolConfig TgWsProxyProtocolConfig::fromJson(const QJsonObject &json)
{
    TgWsProxyProtocolConfig config;
    config.port = json.value(configKey::port).toString();
    config.secret = json.value(protocols::tgWsProxy::secretKey).toString();
    config.fakeTlsDomain = json.value(protocols::tgWsProxy::fakeTlsDomainKey).toString();
    return config;
}

} // namespace amnezia
