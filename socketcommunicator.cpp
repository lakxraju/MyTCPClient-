#include "socketcommunicator.h"
#include <complex>
#include <qfile.h>



SocketCommunicator::SocketCommunicator(QObject *parent) :
    QObject(parent)
{
    m_pConnection = new QTcpSocket(this);

}

void SocketCommunicator::readyRead()
{
    // m_pConnection is a QTcpSocket

    /*while(m_pConnection->bytesAvailable())
    {


        QByteArray buffer;


        int dataSize;
        m_pConnection->read((char*)&dataSize, sizeof(quint32));
        qDebug() << "Data Size:" << dataSize;

        if(buffer.capacity() > dataSize)
            buffer = m_pConnection->read(dataSize);
        else
            buffer = m_pConnection->read(buffer.capacity());
        qDebug() << "Current Buffer Size: " << buffer.size();
        while(buffer.size() < dataSize) // only part of the message has been received
        {
            m_pConnection->waitForReadyRead(); // alternatively, store the buffer and wait for the next readyRead()
            qDebug() << "Reading next " << dataSize - buffer.size() << " bytes";
            buffer.append(m_pConnection->read(dataSize - buffer.size())); // append the remaining bytes of the message
        }

        qDebug() << "\tMessage Received:" << buffer;

        // Do something with the message
        ProcessMessage(buffer);
    }*/

    QByteArray currentByteArray;

    QString filename = "C:\\Users\\bots2rec\\Documents\\Data.per";
    QFile tempFile(filename);
    tempFile.open(QIODevice::WriteOnly | QIODevice::Append);
    while(m_pConnection->bytesAvailable())
    {
        currentByteArray = m_pConnection->readAll();
        qDebug() << "Current size of buffer: " << currentByteArray.size();
        tempFile.write(currentByteArray);
    }
    tempFile.close();
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
    QString packetID;
    QByteArray distances;
    QByteArray realValues;
    QByteArray imaginaryValues;
    bool ok = false;

    //completePacketSize = buffer.mid(0,4).toInt(&ok);

    packetID = QString(buffer.mid(0,2).toHex());

    //get angle of the entry
    angle = buffer.mid(8,2).toInt(&ok);

    Entries = buffer.mid(10,2).toInt(&ok);


    qDebug() << "Angle:" << angle;
    qDebug() << "Entries:" << Entries;
    qint32 sizeOfAllEntries = 4*Entries;
    qint32 seekPosition = 12 * Entries;

    float DistanceArray[Entries];
    float realValueArray[Entries];
    float imaginaryValueArray[Entries];
    double intensityArray[Entries];

    distances = buffer.mid(12+seekPosition,sizeOfAllEntries);
    realValues = buffer.mid(16+seekPosition,sizeOfAllEntries);
    imaginaryValues = buffer.mid(20+seekPosition,sizeOfAllEntries);

    for(int i=0; i<Entries; i++)
    {
       DistanceArray[i] = buffer.mid(12+seekPosition+(i*4),4).toFloat(&ok);
       realValueArray[i] = buffer.mid(12+seekPosition+(i*4),4).toFloat(&ok);
       imaginaryValueArray[i] = buffer.mid(12+seekPosition+(i*4),4).toFloat(&ok);

       auto spectrum = std::complex<float>{buffer.mid(12+seekPosition+(i*4),4).toFloat(&ok), buffer.mid(12+seekPosition+(i*4),4).toFloat(&ok)};
       auto intensity = std::abs(spectrum);
       intensityArray[i] = intensity;
    }

    qDebug() << "Stream Processed!";
   // qDebug() << "size:" << completePacketSize;
    qDebug() << "Packet ID:" << packetID;
    qDebug() << "Angle:" << angle;
    qDebug() << "No. of Entries:" << Entries;
    qDebug() << "Distances:" << DistanceArray;
    qDebug() << "Real:" << realValueArray;
    qDebug() << "Imaginary:" << imaginaryValueArray;
    qDebug() << "Computed Intensity:" << intensityArray;
}

void SocketCommunicator::readAndProcessFromFile()
{
    QByteArray currentByteArray;

    QString filename = "C:\\Users\\bots2rec\\Documents\\Data.per";
    QFile tempFile(filename);
    tempFile.open(QIODevice::ReadOnly);
    currentByteArray = tempFile.readAll();

    qint32 size;
    QByteArray buffer;
    QByteArray completePacket;
    qint32 angle;
    qint32 Entries;
    QByteArray distances;
    QByteArray realValues;
    QByteArray imaginaryValues;
    QByteArray currentPacket;
    bool ok = false;

    qDebug() << "Read the file and the size is : " << currentByteArray.size();
    while(currentByteArray.size() > 4)
    {
        size = rawToInt(currentByteArray.mid(0,4));

        qDebug() << "Size of current Packet: " << size;

        //get angle of the entry

        qDebug() << "Angle Byte Array" << currentPacket.mid(12,2);
        qDebug() << "Entry Byte Array" << currentPacket.mid(14,2);



        angle = rawToShort(currentPacket.mid(12,2));
        Entries = rawToShort(currentPacket.mid(14,2));


        qDebug() << "Angle:" << angle;
        qDebug() << "Entries:" << Entries;
        qint32 sizeOfAllEntries = 4*Entries;
        qint32 seekPosition = 12 * Entries;

        float DistanceArray[Entries];
        float realValueArray[Entries];
        float imaginaryValueArray[Entries];
        double intensityArray[Entries];

        distances = currentPacket.mid(16+seekPosition,sizeOfAllEntries);
        realValues = currentPacket.mid(20+seekPosition,sizeOfAllEntries);
        imaginaryValues = currentPacket.mid(24+seekPosition,sizeOfAllEntries);

        bool ok = false;
        for(int i=0; i<Entries; i++)
        {
           DistanceArray[i] = currentPacket.mid(16+seekPosition+(i*4),4).toFloat(&ok);
           realValueArray[i] = currentPacket.mid(16+seekPosition+(i*4),4).toFloat(&ok);
           imaginaryValueArray[i] = currentPacket.mid(16+seekPosition+(i*4),4).toFloat(&ok);

           auto spectrum = std::complex<float>{currentPacket.mid(16+seekPosition+(i*4),4).toFloat(&ok), currentPacket.mid(16+seekPosition+(i*4),4).toFloat(&ok)};
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
        currentPacket = currentByteArray.remove(0,size);
    }
    tempFile.close();
}

int SocketCommunicator::rawToInt(QByteArray arry)
{
    int i = (arry.at(3) << 24) | (arry.at(2) << 16) | (arry.at(1) << 8) | (arry.at(0));
    return i;
}

short SocketCommunicator::rawToShort(QByteArray arry)
{
    short s = (arry[0] << 8) | (arry[1]);
    return s;
}


