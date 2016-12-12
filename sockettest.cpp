#include "sockettest.h"

SocketTest::SocketTest(QObject *parent) :
    QObject(parent)
{
}

void SocketTest::Connect()
{
    socket = new QTcpSocket(this);
    socket->connectToHost("www.google.com", 80);
    qint64 returnValue;
    qint64 maxLength = 65535;
    char *readData;

    if(socket->waitForConnected(3000))
    {
        qDebug() << "Connected!";

        // send
        //socket->write("hello server\r\n\r\n\r\n\r\n");
        //socket->waitForBytesWritten(1000);
        //socket->waitForReadyRead(3000);
        qDebug() << "Reading: " << socket->bytesAvailable();

        returnValue = socket ->read(readData,maxLength);
                if(returnValue!=-1 && returnValue != 0)
                {
                    qDebug() << *readData;
                }

        socket->close();
    }
    else
    {
        qDebug() << "Not connected!";
    }

    // sent

    // got

    // closed
}
