#include "socketcommunicator.h"
#include <complex>
#include <qfile.h>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>


QTime dieTime;
int mode;

SocketCommunicator::SocketCommunicator(QObject *parent) :
    QObject(parent)
{
    m_pConnection = new QTcpSocket(this);

}

void SocketCommunicator::readyRead()
{

    QByteArray currentByteArray;

    // QString filename = "C:\\Users\\bots2rec\\Documents\\Data.per";

    QString filename = "/home/charaf_eddine/MyTCPClient-/Raw_Radar_Data/Data.per";
    QFile tempFile(filename);
    tempFile.open(QIODevice::WriteOnly | QIODevice::Append);
    while(m_pConnection->bytesAvailable())
    {
        currentByteArray = m_pConnection->readAll();
        qDebug() << "Current size of buffer: " << currentByteArray.size();
        tempFile.write(currentByteArray);
    }

    if(dieTime <= QTime::currentTime())
    {
      disconnect(m_pConnection, SIGNAL(readyRead()), 0, 0);
      m_pConnection->close();
      tempFile.close();
      readAndProcessFromFile(mode);
    }

    tempFile.close();
}

void SocketCommunicator::captureRawRadarData(int selectedMode, int timeInSeconds)
{

    // "192.168.23.12" is always the same as the NUC has this static IP address
    m_pConnection->connectToHost("192.168.23.12", 10120);

    dieTime = QTime::currentTime().addSecs(timeInSeconds);
    mode = selectedMode;

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




/*
 * Note: This method reads the raw data that is stored in local file and converts it into usable datatypes.
 * Assumption: C:\\Users\\bots2rec\\Documents\\Data.per is the local file where the stream is stored
 *
 *
 * Algorithm:
 *  1. Read all data from the local file "C:\\Users\\bots2rec\\Documents\\Data.per" into a QByteArray
 *  2. In a loop continue the following computation (if the size of the ByteArray is greater than 4)
 *      2a. Extract first 4 bytes of the array to know the size of first packet.
 *      2b. Copy the first packet into another local variable
 *      2c. Extract following information from the current packet:
 *              1. Angle (bytes 12 - 14)
 *              2. Number of Entries (bytes 14 - 16)
 *              3. Based on Number of Entries (n), we extract bytes corresponding to distance array, real value and imaginary values (to compute intensity)
 *              4. Compute intensity based on the real and imaginary values
 *      2d. Remove the currentPacket bytes from the main QByteArray
 *
 *  Note: We currently are not storing the output in any file. We just print it.
 */
void SocketCommunicator::readAndProcessFromFile(int selectedOption)
{
    QByteArray wholeByteArray;
    // QString filename = "C:\\Users\\bots2rec\\Documents\\Data.per";

    QString filename = "/home/charaf_eddine/MyTCPClient-/Raw_Radar_Data/Data.per";
    QFile tempFile(filename);
    tempFile.open(QIODevice::ReadOnly);
    wholeByteArray = tempFile.readAll();

    quint32 current_packet_size;
    quint16 angle;
    quint16 Number_of_Entries;
    QByteArray currentPacket;
    quint16 current_packet_id;
    quint16 current_crc_ccid;
    quint32 current_measurement_id;

    QJsonArray jsonArrayOfAllValues;


    qDebug() << "Read the file and the size is : " << wholeByteArray.size();
    int j=0;
    while(wholeByteArray.size() > 4)
    {
        // Following variables are for removing the duplicates
        qint32 previousDistance = 0;
        double highestIntensity = 0;

        QJsonArray jsonArrayForThisAngle;
        qDebug() << "============================Start of the Loop==================================";
        /*
         * Variant 1: QByteArray -> QDataStream -> Unsigned int
         */
        QByteArray tempByteArray = wholeByteArray.mid(0,4);
        QDataStream tempStreamForSize(&tempByteArray,QIODevice::ReadWrite);
        tempStreamForSize.setByteOrder(QDataStream::LittleEndian);
        tempStreamForSize >> current_packet_size;

        /*
         * Variant 1: QByteArray -> QDataStream -> Unsigned int
         */
        QByteArray tempByteArrayForID = wholeByteArray.mid(4,2);
        QDataStream tempStreamForID(&tempByteArrayForID,QIODevice::ReadWrite);
        tempStreamForID.setByteOrder(QDataStream::LittleEndian);
        tempStreamForID >> current_packet_id;

        QByteArray tempByteArrayForCRC = wholeByteArray.mid(6,2);
        QDataStream tempStreamForCRC(&tempByteArrayForCRC,QIODevice::ReadWrite);
        tempStreamForCRC.setByteOrder(QDataStream::LittleEndian);
        tempStreamForCRC >> current_crc_ccid;



        QByteArray tempByteArrayForMeasInd = wholeByteArray.mid(8,4);
        QDataStream tempStreamForMeasInd(&tempByteArrayForMeasInd,QIODevice::ReadWrite);
        tempStreamForMeasInd.setByteOrder(QDataStream::LittleEndian);
        tempStreamForMeasInd >> current_measurement_id;

        //extracting current packet from the whole file byte array
        currentPacket = wholeByteArray.mid(0,current_packet_size);


        //Compute Angle Based on variant 1
        QByteArray tempByteArrayForAngle = currentPacket.mid(12,2);
        QDataStream tempStreamForAngle(&tempByteArrayForAngle,QIODevice::ReadWrite);
        tempStreamForAngle.setByteOrder(QDataStream::LittleEndian);
        tempStreamForAngle >> angle;

        //Compute number of Entries based on variant 1
        QByteArray tempByteArrayForEntries = currentPacket.mid(14,2);
        QDataStream tempStreamForEntries(&tempByteArrayForEntries,QIODevice::ReadWrite);
        tempStreamForEntries.setByteOrder(QDataStream::LittleEndian);
        tempStreamForEntries >> Number_of_Entries;

        if(angle > 1999)
          break;

        qDebug() << "Angle:" << angle;
        qDebug() << "Entries:" << Number_of_Entries;

        quint32 DistanceArray[Number_of_Entries];
        quint32 realValueArray[Number_of_Entries];
        quint32 imaginaryValueArray[Number_of_Entries];
        double intensityArray[Number_of_Entries];

        QStringList stringListforDistances;
        QStringList stringListforIntensities;


        for(int i=0; i<Number_of_Entries; i++)
        {

           qint32 currentDistance;
           uint32_t tempReal;
           uint32_t tempImag;


           QByteArray tempByteArrayForDistance = currentPacket.mid(16+(12*i),4);
           QDataStream tempStreamForDistance(&tempByteArrayForDistance,QIODevice::ReadWrite);
           tempStreamForDistance.setByteOrder(QDataStream::LittleEndian);
           tempStreamForDistance >> currentDistance;

           QByteArray tempByteArrayForReal = currentPacket.mid(20+(12*i),4);
           QDataStream tempStreamForReal(&tempByteArrayForReal,QIODevice::ReadWrite);
           tempStreamForReal.setByteOrder(QDataStream::LittleEndian);
           tempStreamForReal >> tempReal;

           QByteArray tempByteArrayForImag = currentPacket.mid(24+(12*i),4);
           QDataStream tempStreamForImag(&tempByteArrayForImag,QIODevice::ReadWrite);
           tempStreamForImag.setByteOrder(QDataStream::LittleEndian);
           tempStreamForImag >> tempImag;

           long double unpackedReal = unpack754(tempReal, 32, 8);
           long double unpackedImag = unpack754(tempImag,32,8);

           DistanceArray[i] = currentDistance;
           realValueArray[i] = unpackedReal;
           imaginaryValueArray[i] = unpackedImag;

           QString tempStringForReal;
           tempStringForReal.append(QString::number((double) unpackedReal, 'f'));
           QString tempStringForImag;
           tempStringForImag.append(QString::number((double) unpackedImag, 'f'));

           QString tempStringForDistance;
           tempStringForDistance = QString::number(currentDistance);
           QString tempStringForIntensities;



           auto spectrum = std::complex<float>{unpackedReal, unpackedImag};
           auto currentIntensity = std::abs(spectrum);
           intensityArray[i] = currentIntensity;
           tempStringForIntensities.append(QString::number((double) currentIntensity, 'f'));

           if((selectedOption == 2)&&(stringListforDistances.size()>0))
           {
             if(previousDistance == currentDistance)
             {
                if(highestIntensity < currentIntensity)
                {
                  highestIntensity = currentIntensity;
                  stringListforDistances.removeLast();
                  stringListforIntensities.removeLast();
                  stringListforDistances.append(tempStringForDistance);
                  stringListforIntensities.append(tempStringForIntensities);
                }
                else
                {
                  // Do nothing as the highest intensity for the current distance is already there.
                }

             }
             else
             {
               stringListforDistances.append(tempStringForDistance);
               stringListforIntensities.append(tempStringForIntensities);
               highestIntensity = currentIntensity;
             }
           }
           else if(stringListforDistances.size() == 0)
           {
             highestIntensity = currentIntensity;
             stringListforDistances.append(tempStringForDistance);
             stringListforIntensities.append(tempStringForIntensities);
           }
           else
           {
            stringListforDistances.append(tempStringForDistance);
            stringListforIntensities.append(tempStringForIntensities);
           }


           previousDistance = currentDistance;

        }


        if(selectedOption == 2)
        {
          Number_of_Entries = stringListforDistances.size();
        }

        qDebug() << "Stream Processed!";
        qDebug() << "Packet ID" << current_packet_id;
        qDebug() << "CRC-CCITT" << current_crc_ccid;
        qDebug() << "Measurement ID" << current_measurement_id;
        qDebug() << "size:" << current_packet_size;
        qDebug() << "Angle:" << angle;
        qDebug() << "No. of Entries:" << Number_of_Entries;
        qDebug() << "Distances:" << DistanceArray;
        qDebug() << "Real:" << realValueArray;
        qDebug() << "Imaginary:" << imaginaryValueArray;
        qDebug() << "Computed Intensity:" << intensityArray;

        qDebug() << "============================End of the Loop==================================";

        jsonArrayForThisAngle.append(angle);
        jsonArrayForThisAngle.append(Number_of_Entries);

        for(int l=0; l < stringListforDistances.count(); ++l){
            jsonArrayForThisAngle.append(stringListforDistances.at(l));
        }
        for(int m=0; m < stringListforIntensities.count(); ++m){
            jsonArrayForThisAngle.append(stringListforIntensities.at(m));
        }

        jsonArrayOfAllValues.insert(j,jsonArrayForThisAngle);
        jsonArrayForThisAngle.empty();
        //removing the initial processed byte array
        int whole_size = wholeByteArray.size();
        wholeByteArray = wholeByteArray.right(whole_size - current_packet_size);
        j++;
    }

    QJsonDocument finalJsonDocument(jsonArrayOfAllValues);
    // QFile finalJsonFile("C:\\Users\\bots2rec\\Documents\\radarValues.json");

    QDateTime datTime;
    QString jsonFileName = "/home/charaf_eddine/MyTCPClient-/Preprocessed_Radar_Data/data_" + datTime.currentDateTime().toString() + ".json";
    QFile finalJsonFile(jsonFileName);

    finalJsonFile.open(QIODevice::WriteOnly);
    finalJsonFile.write(finalJsonDocument.toJson());
    tempFile.close();
}

/*
 * The following function unpack754(uint64_t i, unsigned bits, unsigned expbits) is used to unpack the float from the uint that was taken from the data stream.
 * This method is implemented based on IEEE - 754 speification
 */

long double SocketCommunicator::unpack754(uint64_t i, unsigned bits, unsigned expbits)
{
    long double result;
    long long shift;
    unsigned bias;
    unsigned significandbits = bits - expbits - 1; // -1 for sign bit

    if (i == 0) return 0.0;

    // pull the significand
    result = (i&((1LL<<significandbits)-1)); // mask
    result /= (1LL<<significandbits); // convert back to float
    result += 1.0f; // add the one back on

    // deal with the exponent
    bias = (1<<(expbits-1)) - 1;
    shift = ((i>>significandbits)&((1LL<<expbits)-1)) - bias;
    while(shift > 0) { result *= 2.0; shift--; }
    while(shift < 0) { result /= 2.0; shift++; }

    // sign it
    result *= (i>>(bits-1))&1? -1.0: 1.0;

    return result;
}

// All following methods are not used currently. Having it just for reference purposes.

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

// This method was used to process the realtime buffer

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
    qDebug() << "Packet ID:" << packetID;
    qDebug() << "Angle:" << angle;
    qDebug() << "No. of Entries:" << Entries;
    qDebug() << "Distances:" << DistanceArray;
    qDebug() << "Real:" << realValueArray;
    qDebug() << "Imaginary:" << imaginaryValueArray;
    qDebug() << "Computed Intensity:" << intensityArray;
}

