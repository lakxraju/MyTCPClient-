#include<QMainWindow>
#include<QPushButton>
#include<QHBoxLayout>
#include<QMessageBox>

class CoreWindow : public QMainWindow
{
  Q_OBJECT
public:
    CoreWindow(){}
    ~CoreWindow(){}

    void decorate()
        {

          QPushButton *button = new QPushButton(this);

          button->setText("Click To get Data From the Radar");

          QObject::connect(button, SIGNAL(clicked()),this, SLOT(clickedSlot()));

          button->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
          button->setMinimumWidth(300);
          button->setMinimumHeight(100);

          QWidget* centralWidget = new QWidget(this);
          centralWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

          QHBoxLayout* layout = new QHBoxLayout(centralWidget);

          layout->addWidget(button);

          setCentralWidget(centralWidget);
          setWindowTitle("TCP Communication Client");
          show();
        }

public slots:
    void clickedSlot()
         {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Retrieving Data..");
            msgBox.setText("You Clicked "+ ((QPushButton*)sender())->text());
            msgBox.exec();
         }

};
