#include <QApplication>
#include <QMainWindow>
#include "corewindow.h"
#include <socketcommunicator.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    /*
     * CoreWindow window;
     * window.setWindowTitle(QString::fromUtf8("MainWindow"));
     * window.resize(450,300);
     * window.decorate();
     *
    */

    SocketCommunicator cTest;
    /*
     * Note: The following line is to get raw data from the radar and store it in a local file C:\\Users\\bots2rec\\Documents\\Data.per.
     *       Uncomment if new data has to be fetched
    */

    //cTest.Test();



    /*
     *  Note: The following line is to process the stored raw data from the local file. C:\\Users\\bots2rec\\Documents\\Data.per
     */

    int selectedMode = 1;

    /*
     * Uncomment the following line to get unique distance values with highest corresponding intensity
     *
     * selectedMode: 1 -> to get All values from radar
     *                 2 -> to get only the spiked values without duplicates
     */
     selectedMode = 2;

    cTest.readAndProcessFromFile(selectedMode);


    return app.exec();

}
