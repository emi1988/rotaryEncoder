#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include "rotaryEncoder.h"
#include "gpiocontrol.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    gpioControl *myGpioControl;

    int m_knobValue;

    bool m_flagPinAchanged;
    bool m_flagPinBchanged;

    int m_pinAValue;
    int m_pinBValue;

    int m_lastSeq;

    void initGpios();

private slots:

  void gpioChanged(int pin, int state);
};

#endif // MAINWINDOW_H
