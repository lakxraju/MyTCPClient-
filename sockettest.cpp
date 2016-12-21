#include "sockettest.h"
#include <complex>

SocketTest::SocketTest(QObject *parent) :
    QObject(parent)
{
}

void SocketTest::Test()
{
    socket = new QTcpSocket(this);

  //  connect(socket, SIGNAL(connected()), this, SLOT(Connected()));
//    connect(socket,SIGNAL(disconnected()),this, SLOT(Disconnected()));
//    connect(socket,SIGNAL(readyRead()),this, SLOT(readyRead()));
//    connect(socket, SIGNAL(bytesWritten(qint64)),this, SLOT(bytesWritten(qint64)));


    socket->connectToHost("192.168.23.12", 10120);
    qint32 size;
    QByteArray buffer;
    QByteArray completePacket;
    qint32 angle;
    qint32 Entries;
    QByteArray distances;
    QByteArray realValues;
    QByteArray imaginaryValues;
    bool ok = false;


    while(socket->bytesAvailable() > 0)
    {
        buffer.append(socket->readAll());
        while(buffer.size()>4)
        {
            size = buffer.mid(0,4).toInt(&ok);
            if(buffer.size() > (size+3))
            {
                completePacket = buffer.mid(0,size+4);
                //get angle of the entry
                angle = buffer.mid(12,2).toInt(&ok);
                Entries = buffer.mid(14,2).toInt(&ok);


                qDebug() << "Angle:" << angle;
                qDebug() << "Entries:" << Entries;
                qint32 sizeOfAllEntries = 4*Entries;
                qint32 seekPosition = 12 * Entries;

                float DistanceArray[Entries];
                float realValueArray[Entries];
                float imaginaryValueArray[Entries];
                double intensityArray[Entries];

                distances = buffer.mid(16+seekPosition,sizeOfAllEntries);
                realValues = buffer.mid(20+seekPosition,sizeOfAllEntries);
                imaginaryValues = buffer.mid(24+seekPosition,sizeOfAllEntries);

                bool ok = false;
                for(int i=0; i<Entries; i++)
                {
                   DistanceArray[i] = buffer.mid(16+seekPosition+(i*4),4).toFloat(&ok);
                   realValueArray[i] = buffer.mid(16+seekPosition+(i*4),4).toFloat(&ok);
                   imaginaryValueArray[i] = buffer.mid(16+seekPosition+(i*4),4).toFloat(&ok);

                   auto spectrum = std::complex<float>{buffer.mid(16+seekPosition+(i*4),4).toFloat(&ok), buffer.mid(16+seekPosition+(i*4),4).toFloat(&ok)};
                   auto intensity = std::abs(spectrum);
                   intensityArray[i] = intensity;
                }

                qDebug() << "Stream Processed!";
                qDebug() << "size:" << size;
                qDebug() << "Angle:" << angle;
                qDebug() << "No. of Entries:" << Entries;
                qDebug() << "Distances:" << DistanceArray;
                qDebug() << "Real:" << realValueArray;
                qDebug() << "Imaginary:" << imaginaryValueArray;
                qDebug() << "Computed Intensity:" << intensityArray;

            }
        }
    }

}

/*

qint32 SocketTest::ArrayToInt(QByteArray source)
{
    qint32 temp;
    QDataStream data(&source, QIODevice::ReadWrite);
    data >> temp;
    return temp;
}
void SocketTest::Connected()
{
    qDebug() << "Connected!";
}

void SocketTest::Disconnected()
{
    qDebug() << "Disconnected! Bye Bye :-)";
}

void SocketTest::bytesWritten(qint64 bytes)
{
    //Do nothing since we are writing anything in the socket
}

void SocketTest::readyRead()
{
    qDebug() << "Reading...";
    qDebug() << socket->readAll();
}
*/
