#ifndef SOCKETCOMMUNICATOR_H
#define SOCKETCOMMUNICATOR_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QAbstractSocket>

class SocketCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit SocketCommunicator(QObject *parent = 0);
    QTcpSocket *m_pConnection;
    void ProcessMessage(QByteArray msg);
    void Test();

signals:

public slots:

    void Connected();
    void Disconnected();
    void readyRead();

private:


};

#endif // SOCKETCOMMUNICATOR_H
