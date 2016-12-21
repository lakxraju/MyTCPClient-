#include <QApplication>
#include <QMainWindow>
#include "corewindow.h"
#include <socketcommunicator.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
//    CoreWindow window;

//    window.setWindowTitle(QString::fromUtf8("MainWindow"));
//    window.resize(450,300);
//    window.decorate();

    SocketCommunicator cTest;
    //cTest.Test();
    cTest.readAndProcessFromFile();


    return app.exec();

}
