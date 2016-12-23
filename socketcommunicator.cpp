#include "socketcommunicator.h"
#include <complex>
#include <qfile.h>
#include <QDataStream>



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


/*
 * Note: This method reads the raw data that is stored in local file and converts it into usable datatypes.
 * Assumption: C:\\Users\\bots2rec\\Documents\\Data.per is the local file where the stream is stored
 *
 */
void SocketCommunicator::readAndProcessFromFile()
{
    QByteArray currentByteArray;

    QString filename = "C:\\Users\\bots2rec\\Documents\\Data.per";
    QFile tempFile(filename);
    tempFile.open(QIODevice::ReadOnly);
    currentByteArray = tempFile.readAll();

    quint32 size;
    QByteArray buffer;
    QByteArray completePacket;
    quint16 angle;
    quint16 Entries;
    QByteArray distances;
    QByteArray realValues;
    QByteArray imaginaryValues;
    QByteArray currentPacket;
    bool ok = false;


    qDebug() << "Read the file and the size is : " << currentByteArray.size();
    while(currentByteArray.size() > 4)
    {

        /*
         * Variant 1: QByteArray -> QDataStream -> Unsigned int
         */
        QByteArray tempByteArray = currentByteArray.mid(0,4);
        QDataStream tempStreamForSize(&tempByteArray,QIODevice::ReadWrite);
        tempStreamForSize.setByteOrder(QDataStream::LittleEndian);
        tempStreamForSize >> size;
        qDebug() << "Size of current Packet(Via variant 1): " << size;


        /*
         * Variant 2: QByteArray -> Unsigned int Direct conversion
         * (check rawToInt(QByteArray arry) method for details)
         *
         */

        size = rawToInt(currentByteArray.mid(0,4));
        qDebug() << "Size of current Packet(Via variant 2): " << size;


        /*
         * Variant 3: QByteArray -> int conversion given by the methods in QByteArray
         */

         size = currentByteArray.mid(0,4).toInt(&ok);
         qDebug() << "Size of current Packet(Via variant 3): " << size;



        //Again reallocating to variant 1
        tempStreamForSize >> size;

        //extracting current packet from the whole file byte array
        currentPacket = currentByteArray.mid(0,size);


        //get angle of the entry
        qDebug() << "Angle Byte Array" << currentPacket.mid(12,2);
        qDebug() << "Entry Byte Array" << currentPacket.mid(14,2);

        //Compute Angle Based on variant 1
        QByteArray tempByteArrayForAngle = currentByteArray.mid(12,2);
        QDataStream tempStreamForAngle(&tempByteArrayForAngle,QIODevice::ReadWrite);
        tempStreamForAngle.setByteOrder(QDataStream::LittleEndian);
        tempStreamForAngle >> angle;

        //Compute number of Entries based on variant 1
        QByteArray tempByteArrayForEntries = currentPacket.mid(14,2);
        QDataStream tempStreamForEntries(&tempByteArrayForEntries,QIODevice::ReadWrite);
        tempStreamForEntries.setByteOrder(QDataStream::LittleEndian);
        tempStreamForEntries >> Entries;

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


        //removing the initial processed byte array
        int temp1 = currentByteArray.size();
        currentByteArray = currentByteArray.right(temp1 - size);
    }
    tempFile.close();
}


/*
 * Note: This method converts raw data in a QByteArray into an integer.
 * Assumption: Input array is of size 4
 *
 */
int SocketCommunicator::rawToInt(QByteArray arry)
{
    int i = (arry.at(3) << 24) | (arry.at(2) << 16) | (arry.at(1) << 8) | (arry.at(0));
    return i;
}

/*
 * Note: This method converts raw data in a QByteArray into a short integer.
 * Assumption: Input array is of size 2
 *
 */
short SocketCommunicator::rawToShort(QByteArray arry)
{
    short s = (arry[0] << 8) | (arry[1]);
    return s;
}


/*
 * Note: This method converts a given QByteArray to a QDataStream.
 * This is done in order to facilitate easy conversion of raw data to different datatypes.
 */
QDataStream SocketCommunicator::ByteArrayToDataStream(QByteArray arry)
{
//    QDataStream tempStream(&arry, QIODevice::ReadWrite);
//    tempStream.setByteOrder(QDataStream::LittleEndian);
//    return tempStream;
}


