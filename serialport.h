#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QtSerialPort/QSerialPort>
#include <QLockFile>
#include <QObject>


#define DATA_SIZE   8192

struct Buffer{
    char buffer[DATA_SIZE];
    uint32_t  ptr_in=0;
    uint32_t  ptr_out=0;
};

class Serialport : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QStringList availablePortsName READ getAvailablePortsName NOTIFY availablePortsNameChanged)
    Q_PROPERTY(int indexSerialPort READ getIndexSerialPort NOTIFY indexSerialPortChanged)
    Q_PROPERTY(bool serialPortOpened READ getSerialPortOpened NOTIFY serialPortOpenedChanged)

    Q_INVOKABLE QStringList getAvailablePortsName(){return  availablePortsName;}
    Q_INVOKABLE int scanSerialPorts();
    Q_INVOKABLE int getIndexSerialPort() {return indexSerialPort;}
    Q_INVOKABLE bool getSerialPortOpened() {return serialPortOpened;}


    Serialport(QObject *parent = nullptr);
    ~Serialport();

    void init();

    void checkSerialPorts();

    int sendString(QString);
    uint32_t transmit(char *, uint32_t);
    int reciev(unsigned char *, int);

    QString m_checkPortName;
    QString portName;             //!< The name of the serial device computed from ttyDevicePath (ex: "ttyUSB0" or "COM1").
    int deviceBaudRate = 1000000;   //!< Speed of the serial link in baud. Default is 1M/s.
    int deviceLatencyTime;          //!< The value of this timer (in millisecond) should be carefully choosed depending on your OS and the speed of your serial port implementation.

    int deviceLockMode = 1;         //!< Method used to lock a serial device.
    QString deviceLockPath;         //!< The path to a "lock file" to lock serial interface against concurrent use by multiple programs.

    double packetStartTime = 0.0;   //!< Time (in millisecond) when the packet was sent.
    double packetWaitTime = 0.0;    //!< Time (in millisecond) to wait for an answer.

    Buffer rx;
    Buffer tx;




signals:
    void availablePortsNameChanged();
    void indexSerialPortChanged();
    void serialPortOpenedChanged(bool isOpen);
    void serialRxBufferCanged();
    void initBuffer(Buffer *rx, Buffer *tx);


public Q_SLOTS:
    void open(const QString& portname);
    void close();

    void handleError(QSerialPort::SerialPortError);
    void readData();

private:
    QSerialPort *serialport=nullptr;
    QList<QSerialPortInfo> availablePorts;
    QStringList availablePortsName;
    int indexSerialPort=0;
    bool serialPortOpened = false;
    QString savingPort;

};

#endif // SERIALPORT_H
