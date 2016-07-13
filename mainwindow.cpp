#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QThread>
#include <QTimer>
#include <QDebug>

#define pinA 4
#define pinB 17

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_flagPinAchanged = false;
    m_flagPinBchanged = false;

    m_lastSeq = -1;

    m_knobValue = 0;

    initGpios();


}

MainWindow::~MainWindow()
{
    delete ui;

}

void MainWindow::initGpios()
{
    myGpioControl = new gpioControl();

    QList<int> inputPins;

    inputPins << pinA << pinB;
    myGpioControl->GPIOInitInputs(inputPins);

    QThread *t = new QThread();
    QTimer *gpioTimer = new QTimer();

    QObject::connect(gpioTimer, SIGNAL(timeout()), myGpioControl, SLOT(checkGpio()));
    //QObject::connect(gpioTimer, SIGNAL(timeout()), this, SLOT(test()));

    QObject::connect(myGpioControl, SIGNAL(gpioInputChanged(int, int)), this, SLOT(gpioChanged(int,int)));

    gpioTimer->start(1);

    myGpioControl->moveToThread(t);

    t->start();

}

void MainWindow::gpioChanged(int pin, int state)
{

    //qDebug() << " main: pin "<< pin << "changed to: "<< state;

    if(pin == pinA)
    {
        m_pinAValue = state;
        m_flagPinAchanged = true;
    }
    if(pin == pinB)
    {
        m_pinBValue = state;
        m_flagPinBchanged = true;
    }

  //  if(m_flagPinAchanged & m_flagPinBchanged)
  //  {
        m_flagPinAchanged = false;
        m_flagPinBchanged = false;

        int currentSeq = m_pinAValue ^ m_pinBValue | m_pinBValue << 1;

        qDebug() << "current Seq: " << currentSeq;

        if(m_lastSeq != -1)
        {
            int deltaSeq = (currentSeq - m_lastSeq);

            if((deltaSeq == 1) | (deltaSeq == -3))
            {
               m_knobValue --;
            }
            else if((deltaSeq == -1) | (deltaSeq == 3))
            {
                m_knobValue ++;
            }
            else
            {
                qDebug() << "deltaSeq: " << deltaSeq;
            }

            ui->labelKnob1->setText(QString::number(m_knobValue));

            m_lastSeq = currentSeq;
        }
        else
        {
            //first run
            m_lastSeq = currentSeq;

        }
  //  }

}
