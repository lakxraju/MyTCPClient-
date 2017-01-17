#include <QCoreApplication>
#include <QMainWindow>
#include "corewindow.h"
#include <socketcommunicator.h>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    SocketCommunicator cTest;
    int selectedMode = 2;
    int timeInSeconds = 5;
    //selectedMode = 1;
    /*
     * Note: The following line is to get raw data from the radar and store it in a local file C:\\Users\\bots2rec\\Documents\\Data.per.
     *       Uncomment if new data has to be fetched
     *
     *  Output file location: /home/charaf_eddine/MyTCPClient-/Raw_Radar_Data/
    */

    /*
     *
     * selectedMode: 1 -> to get All values from radar
     *                 2 -> to get only the spiked values without duplicates
     *  Output file location: /home/charaf_eddine/MyTCPClient-/Preprocessed_Radar_Data
     *
     * timeInSeconds: Duration to capture the RADAR data
     */

    cTest.captureAndPreProcessRadarData(selectedMode, timeInSeconds);

    return app.exec();

}
