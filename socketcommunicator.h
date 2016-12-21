#ifndef SOCKETCOMMUNICATOR_H
#define SOCKETCOMMUNICATOR_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QAbstractSocket>
#include <QFile>

class SocketCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit SocketCommunicator(QObject *parent = 0);
    QTcpSocket *m_pConnection;
    void ProcessMessage(QByteArray msg);
    void Test();
    void readAndProcessFromFile();
    int rawToInt(QByteArray arr);
    short rawToShort(QByteArray arr);

signals:

public slots:

    void Connected();
    void Disconnected();
    void readyRead();

private:


};

#endif // SOCKETCOMMUNICATOR_H
