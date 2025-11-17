#include <QCoreApplication>
#include <QSerialPortInfo>
#include <QString>
#include <QDebug>
#include <QTimer>


#include "serialport.h"
#include "settings.h"
#include "scan.h"

#define VID 0x483
#define PID 0x5740



Serialport::Serialport(QObject *parent) :
    QObject(parent)
{
    serialport = new QSerialPort(this);
    deviceBaudRate = QSerialPort::Baud115200;

    connect(serialport, &QSerialPort::errorOccurred, this, &Serialport::handleError, Qt::QueuedConnection);
    connect(serialport, &QSerialPort::readyRead, this, &Serialport::readData, Qt::QueuedConnection);

    checkSerialPorts();
    qDebug() << "Serialport";
}

Serialport::~Serialport()
{
    if(!savingPort.isEmpty()){
        Settings settings;
        settings.setSerialPortName(savingPort);
        qDebug() << "save port name:" << savingPort;
    }
    qDebug() << "~Serialport";
}

void Serialport::init()
{
    emit initBuffer(&rx, &tx);
}

void Serialport::checkSerialPorts()
{
    Settings settings;
    settings.init();

    if(scanSerialPorts() > 0){
        int index = 0;
        for(QString name: availablePortsName){
            qDebug() << name << settings.serialPortName;

            index++;
            if(name != settings.serialPortName) continue;
            indexSerialPort = index-1;
            break;
        }
        portName = availablePorts.at(indexSerialPort).portName().toLocal8Bit().data();
    }
}

void Serialport::handleError(QSerialPort::SerialPortError error)
{
    qDebug() <<"Error: " << error << serialport->errorString();
}

void Serialport::readData()
{
    //qDebug() << serialport->bytesAvailable();
    uint32_t numRead = 0;
    //qDebug() << "ptr_in" << rx.ptr_in;
    for(;;){
        numRead = serialport->read(&rx.buffer[rx.ptr_in], DATA_SIZE - rx.ptr_in);
        rx.ptr_in += numRead;
        if(rx.ptr_in >= DATA_SIZE) {
            rx.ptr_in -= DATA_SIZE;
            //qDebug() << "ptr_in0" << rx.ptr_in;
            //emit serialRxBufferCanged();
        }

        if (numRead == 0 ) break;
    }
    //qDebug() << "ptr_in" << rx.ptr_in;
    emit serialRxBufferCanged();
}


int Serialport::scanSerialPorts()
{
    int retcode = 0;

    QSerialPortInfo spi;
    QList<QSerialPortInfo> asp = spi.availablePorts();

    availablePorts.clear();
    availablePortsName.clear();

    for (QSerialPortInfo sp: asp)
    {
        if(sp.isBusy()) continue;
//        qDebug() << "> Port #" << retcode + 1;
//        qDebug() << "- systemLocation: " << sp.systemLocation();
//        qDebug() << "- desc: " << sp.description();
//        qDebug() << "- port name: " << sp.portName();
//        qDebug() << "- manufacturer: " << sp.manufacturer();
//        qDebug() << "- standard BaudRates: " << sp.standardBaudRates();
//        qDebug() << "- isBusy: "<< sp.isBusy() ;
//        qDebug() << "- serialNumber: "<< sp.serialNumber() ;
//        qDebug() << "- vendorIdentifier: "<< sp.vendorIdentifier() ;
//        qDebug() << "- productIdentifier: "<< sp.productIdentifier() ;
//        qDebug() << "> Scanning for serial port" <<retcode <<": " << sp.portName().toLocal8Bit().data();
        //if(sp.vendorIdentifier() == VID && sp.productIdentifier() == PID) {
            availablePorts.push_back(sp);
            availablePortsName.push_back(sp.portName().toLocal8Bit().data());
            retcode++;
        //}
    }

    emit availablePortsNameChanged();

    return retcode;
}


int Serialport::sendString(QString packetStr)
{
    int writeStatus = -1;
    int packetLength = packetStr.length();
    QByteArray ba = packetStr.toLocal8Bit();
    const char *packet = ba.data();
    if (serialport->isOpen() == true) {
        if (packet != nullptr && packetLength > 0) {
            writeStatus = serialport->write(packet, packetLength);
            if (writeStatus < 0) {
                qDebug() << "Cannot write to serial port: write() failed with, error code!" <<portName << errno;
            }
        }
        else {
            qDebug() << "Cannot write to serial port: invalid packet buffer or size!" << portName;
        }
    }
    else {
        qDebug() << "Cannot write to serial port: invalid device!" << portName;
    }

    return writeStatus;
}

uint32_t Serialport::transmit(char *packet, uint32_t packetLength)
{
    uint32_t writeStatus = -1;

    if (serialport->isOpen() == true) {
        if (packet != nullptr && packetLength > 0) {
            writeStatus = serialport->write(packet, packetLength);
            //writeStatus = write(ttyDeviceFileDescriptor, packet, packetLength);


            //***************** debug ************************
//            QString sdata;

//            for(uint32_t i=0; i<packetLength; i++){
//                if((uint8_t)packet[i] <= 0xF) sdata.append('0');
//                sdata.append(QString::number((uint8_t)packet[i], 16).toUpper());
//                sdata.append(' ');
//                if(i == 3 || i == 6 || i == 6+8 || i == 6+8+8 || i == 6+8+8+8 || i == 6+8+8+8+8) sdata.append(' ');
//            }

//            qDebug() << "<" << sdata;
            //************************************************


            if (writeStatus < 0) {
                qDebug() << "Cannot write to serial port: write() failed with, error code!" <<portName << errno;
            }
        }
        else {
            qDebug() << "Cannot write to serial port: invalid packet buffer or size!" << portName;
        }
    }
    else {
        qDebug() << "Cannot write to serial port: invalid device!" << portName;
    }

    return writeStatus;
}

int Serialport::reciev(unsigned char *packet, int packetLength)
{
    int readStatus = -1;

    if (serialport->isOpen() == true) {
        if (packet != nullptr && packetLength > 0) {
            memset(packet, 0, packetLength);
            readStatus = serialport->read((char *)packet, packetLength);
            //readStatus = read(ttyDeviceFileDescriptor, packet, packetLength);

            if (readStatus < 0) {
                qDebug() << "Cannot read from serial port: read() failed with, error code !" << portName << errno;
            }
        }
        else {
            qDebug() << "Cannot read from serial port: invalid packet buffer or size!" << portName;
        }
    }
    else {
        qDebug() << "Cannot read from serial port: invalid device!" << portName;
    }

    return readStatus;
}

void Serialport::open(const QString &portname)
{
    close();

    if(serialport->isOpen()){
        qWarning() << "error open port" << serialport->portName() << ", busy!";
        return;
    }

    portName = portname;

    // Open tty device
    serialport->setPortName(portName);
    serialport->setBaudRate(deviceBaudRate);
    serialport->setDataBits(QSerialPort::Data8);
    serialport->setParity(QSerialPort::NoParity);
    serialport->setStopBits(QSerialPort::OneStop);
    serialport->setFlowControl(QSerialPort::NoFlowControl);
    serialport->setReadBufferSize(16384);


    if (serialport->open(QIODevice::ReadWrite) == false) {
        qDebug() << serialport->errorString();
        qDebug() << "Unable to open device on m_serial port: " << serialport->errorString();
        return;
    }

    // Set newtio attributes

    if (deviceBaudRate < 1) {
        qDebug() << "Unable to set baud rate to '%i'bps: invalid value" << deviceBaudRate;
        return;
    }

    savingPort = portName;
    serialPortOpened = true;
    emit serialPortOpenedChanged(true);
    return;
}

void Serialport::close()
{
    if (serialport->isOpen() == true){
        serialport->close();
    }
    serialPortOpened = false;
    emit serialPortOpenedChanged(false);
}
