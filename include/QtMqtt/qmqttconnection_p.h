/******************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtMqtt module.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
******************************************************************************/

#ifndef QMQTTCONNECTION_P_H
#define QMQTTCONNECTION_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qmqttclient.h"
#include "qmqttcontrolpacket_p.h"
#include "qmqttmessage.h"
#include "qmqttsubscription.h"
#include <QtCore/QBuffer>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QSharedPointer>
#include <QtCore/QTimer>
#include <QtCore/QtEndian>

QT_BEGIN_NAMESPACE

class QMqttClientPrivate;

class Q_AUTOTEST_EXPORT QMqttConnection : public QObject
{
    Q_OBJECT
public:
    enum InternalConnectionState {
        BrokerDisconnected = 0,
        BrokerConnecting,
        BrokerWaitForConnectAck,
        BrokerConnected
    };

    explicit QMqttConnection(QObject *parent = 0);
    ~QMqttConnection() override;

    void setTransport(QIODevice *device, QMqttClient::TransportType transport);
    QIODevice *transport() const;

    bool ensureTransport(bool createSecureIfNeeded = false);
    bool ensureTransportOpen(const QString &sslPeerName = QString());

    bool sendControlConnect();
    qint32 sendControlPublish(const QMqttTopicName &topic, const QByteArray &message, quint8 qos = 0, bool retain = false);
    bool sendControlPublishAcknowledge(quint16 id);
    bool sendControlPublishRelease(quint16 id);
    bool sendControlPublishReceive(quint16 id);
    bool sendControlPublishComp(quint16 id);
    QMqttSubscription *sendControlSubscribe(const QMqttTopicFilter &topic, quint8 qos = 0);
    bool sendControlUnsubscribe(const QMqttTopicFilter &topic);
    bool sendControlPingRequest();
    bool sendControlDisconnect();

    void setClientPrivate(QMqttClientPrivate *clientPrivate);

    inline quint16 unusedPacketIdentifier() const;
    inline InternalConnectionState internalState() const { return m_internalState; }

    void cleanSubscriptions();

public Q_SLOTS:
    void transportConnectionEstablished();
    void transportConnectionClosed();
    void transportReadReady();

public:
    QIODevice *m_transport{nullptr};
    QMqttClient::TransportType m_transportType{QMqttClient::IODevice};
    bool m_ownTransport{false};
    QMqttClientPrivate *m_clientPrivate{nullptr};
private:
    Q_DISABLE_COPY(QMqttConnection)
    void someFuncToBeRemoved();
    void finalize_connack();
    void finalize_suback();
    void finalize_unsuback();
    void finalize_publish();
    void finalize_pubAckRecComp();
    void finalize_pubrel();
    void finalize_pingresp();
    void processData();
    void readBuffer(char *data, qint64 size);
    void closeConnection(QMqttClient::ClientError error);
    QByteArray readBuffer(qint64 size);
    QByteArray m_readBuffer;
    qint64 m_missingData{0};
    struct PublishData {
        quint8 qos;
        bool dup;
        bool retain;
    };
    PublishData m_currentPublish;
    QMqttControlPacket::PacketType m_currentPacket{QMqttControlPacket::UNKNOWN};

    bool writePacketToTransport(const QMqttControlPacket &p);
    QMap<quint16, QMqttSubscription *> m_pendingSubscriptionAck;
    QMap<quint16, QMqttSubscription *> m_pendingUnsubscriptions;
    QMap<QMqttTopicFilter, QMqttSubscription *> m_activeSubscriptions;
    QMap<quint16, QSharedPointer<QMqttControlPacket>> m_pendingMessages;
    QMap<quint16, QSharedPointer<QMqttControlPacket>> m_pendingReleaseMessages;
    InternalConnectionState m_internalState{BrokerDisconnected};
    QTimer m_pingTimer;
};

QT_END_NAMESPACE

#endif // QMQTTCONNECTION_P_H
