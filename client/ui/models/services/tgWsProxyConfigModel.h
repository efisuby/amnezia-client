#ifndef TGWSPROXYCONFIGMODEL_H
#define TGWSPROXYCONFIGMODEL_H

#include <QObject>

#include "core/models/protocols/tgWsProxyProtocolConfig.h"
#include "core/utils/containerEnum.h"

class TgWsProxyConfigModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString port READ port WRITE setPort NOTIFY changed)
    Q_PROPERTY(QString secret READ secret NOTIFY changed)
    Q_PROPERTY(QString fakeTlsDomain READ fakeTlsDomain WRITE setFakeTlsDomain NOTIFY changed)

public:
    explicit TgWsProxyConfigModel(QObject *parent = nullptr);

    QString port() const;
    void setPort(const QString &value);

    QString secret() const;

    QString fakeTlsDomain() const;
    void setFakeTlsDomain(const QString &value);

    Q_INVOKABLE QString proxyLink(const QString &hostName) const;

public slots:
    void updateModel(amnezia::DockerContainer container, const amnezia::TgWsProxyProtocolConfig &protocolConfig);
    amnezia::TgWsProxyProtocolConfig getProtocolConfig();

signals:
    void changed();

private:
    amnezia::TgWsProxyProtocolConfig m_protocolConfig;
};

#endif // TGWSPROXYCONFIGMODEL_H
