#ifndef SOCKETTEST_H
#define SOCKETTEST_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QAbstractSocket>

class SocketTest : public QObject
{
    Q_OBJECT
public:
    explicit SocketTest(QObject *parent = 0);

    void Test();
    qint32 ArrayToInt(QByteArray source);

signals:

public slots:

//    void Connected();
//    void Disconnected();
//    void bytesWritten(qint64 bytes);
//    void readyRead();

private:
    QTcpSocket *socket;

};

#endif // SOCKETTEST_H
