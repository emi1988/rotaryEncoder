#include "gpiocontrol.h"

#include <QApplication>
//#include <QFileSystemWatcher>
#include <QDebug>
#include <QThread>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


gpioControl::gpioControl() : QObject()
{
   wiringPiSetup();

    //m_FileSystemWatcher = new QFileSystemWatcher();

   // QObject::connect(m_FileSystemWatcher,SIGNAL(fileChanged(QString)) ,this,SLOT(gpioFileChanged(QString)));
}

int gpioControl::GPIOExport(int pin)
{
#define BUFFER_MAX 3
    char buffer[BUFFER_MAX];
    ssize_t bytes_written;
    int fd;

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (-1 == fd) {
        fprintf(stderr, "Failed to open export for writing!\n");
        return(-1);
    }

    bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
    write(fd, buffer, bytes_written);

    ::close(fd);
    return(0);

}

int gpioControl::GPIOUnexport(int pin)
{
    char buffer[BUFFER_MAX];
    ssize_t bytes_written;
    int fd;

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (-1 == fd) {
        fprintf(stderr, "Failed to open unexport for writing!\n");
        return(-1);
    }

    bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
    write(fd, buffer, bytes_written);
    ::close(fd);
    return(0);

}

int gpioControl::GPIODirection(int pin, int dir)
{

    static const char s_directions_str[]  = "in\0out";

#define DIRECTION_MAX 35
    char path[DIRECTION_MAX];
    int fd;

    snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
    fd = open(path, O_WRONLY);
    if (-1 == fd) {
        fprintf(stderr, "Failed to open gpio direction for writing!\n");
        return(-1);
    }

    if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3)) {
        fprintf(stderr, "Failed to set direction!\n");
        return(-1);
    }

    ::close(fd);
    return(0);

}

void gpioControl::GPIOPullUPDown(int pin, QString pulldirection)
{
    int dir;

    if(pulldirection.compare("up") == 0)
    {
        dir = PUD_UP;
    }
    else
    {
        dir =PUD_DOWN;
    }

    //pins are differen for the wiring pi lib
    if(pin == 17)
    {
        pin = 0;
    }
    else if( pin ==4)
    {
        pin =7;
    }
    else
    {
        qDebug() << "ERROR setting pull up/down pin not yet included in the mapping";
    }

    pullUpDnControl(pin, dir);

}

int gpioControl::GPIORead(int pin)
{

#define VALUE_MAX 30
    char path[VALUE_MAX];
    char value_str[3];
    int fd;

    snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_RDONLY);
    if (-1 == fd) {
        fprintf(stderr, "Failed to open gpio value for reading!\n");
        return(-1);
    }

    if (-1 == read(fd, value_str, 3)) {
        fprintf(stderr, "Failed to read value!\n");
        return(-1);
    }

    ::close(fd);

    return(atoi(value_str));

}

int gpioControl::GPIOWrite(int pin, int value)
{
    static const char s_values_str[] = "01";

    char path[VALUE_MAX];
    int fd;

    snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_WRONLY);
    if (-1 == fd) {
        fprintf(stderr, "Failed to open gpio value for writing!\n");
        return(-1);
    }

    if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1)) {
        fprintf(stderr, "Failed to write value!\n");
        return(-1);
    }

    ::close(fd);
    return(0);
}

int gpioControl::GPIOSetWatch(QList<stPin> pins)
{

    watchedGpioPins = pins;

    return 0;

}

int gpioControl::GPIOInitInputs(QList<int> pins)
{
    QList<stPin> pinList;

    for (int i = 0; i < pins.size(); ++i)
    {
        GPIOPullUPDown(pins.at(i), "up");

        GPIOExport(pins.at(i));
        GPIODirection(pins.at(i), IN);

        stPin currentPin;
        currentPin.pinNumber = pins.at(i);
        currentPin.value = 0;
        pinList.append(currentPin);

    }
    GPIOSetWatch(pinList);

    return 0;
}



void gpioControl::checkGpio()
{

    //qDebug() << "timer Pin check fired. Thread ID: "<<QThread::currentThreadId();

    for (int i = 0; i < watchedGpioPins.size(); ++i)
    {

        int state = GPIORead(watchedGpioPins.at(i).pinNumber);

        //check if state has changed
        if(state != watchedGpioPins.at(i).value)
        {
            //qDebug() << "signal changed at pin: "<< watchedGpioPins.at(i).pinNumber << " value: " << state;
            emit gpioInputChanged(watchedGpioPins.at(i).pinNumber, state);

            watchedGpioPins[i].value = state;


        }


    }

}

