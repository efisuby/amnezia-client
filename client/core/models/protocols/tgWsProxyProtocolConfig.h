#ifndef TGWSPROXYPROTOCOLCONFIG_H
#define TGWSPROXYPROTOCOLCONFIG_H

#include <QJsonObject>
#include <QString>

namespace amnezia
{

struct TgWsProxyProtocolConfig {
    QString port;
    QString secret;
    QString fakeTlsDomain;

    QString proxyLink(const QString &hostName) const;

    QJsonObject toJson() const;
    static TgWsProxyProtocolConfig fromJson(const QJsonObject &json);
};

} // namespace amnezia

#endif // TGWSPROXYPROTOCOLCONFIG_H
