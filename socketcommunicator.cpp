#include "socketcommunicator.h"
#include <complex>


SocketCommunicator::SocketCommunicator(QObject *parent) :
    QObject(parent)
{
    m_pConnection = new QTcpSocket(this);
}

void SocketCommunicator::readyRead()
{
    // m_pConnection is a QTcpSocket

    while(m_pConnection->bytesAvailable())
    {
        QByteArray buffer;

        int dataSize;
        m_pConnection->read((char*)&dataSize, sizeof(quint32));
        qDebug() << "Data Size:" << dataSize;

        buffer = m_pConnection->read(dataSize);

        while(buffer.size() < dataSize) // only part of the message has been received
        {
            m_pConnection->waitForReadyRead(); // alternatively, store the buffer and wait for the next readyRead()
            buffer.append(m_pConnection->read(dataSize - buffer.size())); // append the remaining bytes of the message
        }

        qDebug() << "\tMessage Received:" << buffer;

        // Do something with the message
        ProcessMessage(buffer);
    }
}

void SocketCommunicator::Test()
{

    m_pConnection->connectToHost("192.168.23.12", 10120);


    connect(m_pConnection,SIGNAL(connected()), this, SLOT(Connected()));
    connect(m_pConnection,SIGNAL(disconnected()),this, SLOT(Disconnected()));
    connect(m_pConnection,SIGNAL(readyRead()),this, SLOT(readyRead()));
}

void SocketCommunicator::Connected()
{
    qDebug() << "Connected with the Remote Socket";
}

void SocketCommunicator::Disconnected()
{
    qDebug() << "Disconnected from the remote socket";
}

void SocketCommunicator::ProcessMessage(QByteArray buffer)
{
    qDebug() << "Inside Process Message!";

    qint32 completePacketSize;
    QByteArray completePacket;
    qint32 angle;
    qint32 Entries;
    QByteArray distances;
    QByteArray realValues;
    QByteArray imaginaryValues;
    bool ok = false;

    completePacketSize = buffer.mid(0,4).toInt(&ok);
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
    qDebug() << "size:" << completePacketSize;
    qDebug() << "Angle:" << angle;
    qDebug() << "No. of Entries:" << Entries;
    qDebug() << "Distances:" << DistanceArray;
    qDebug() << "Real:" << realValueArray;
    qDebug() << "Imaginary:" << imaginaryValueArray;
    qDebug() << "Computed Intensity:" << intensityArray;
}

