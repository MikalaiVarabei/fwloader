#ifndef SCAN_H
#define SCAN_H
#include <QDebug>
#include <QObject>
#include <QJsonObject>
#include "serialport.h"
#include "scanbootloader.h"
#include "scanconst.h"

class Scan : public QObject
{
    Q_OBJECT
    //Q_PROPERTY(QJsonArray log READ getLog NOTIFY logChanged)
public:
    //Q_PROPERTY(QList<QVariantMap> log READ getLogList NOTIFY logListChanged)

    explicit Scan(QObject *parent = nullptr);
    ~Scan();
    //QJsonArray getLog() {return jsArray;}

    QByteArray bin;

    Serialport *serialport;
    ScanBootloader scanBootloader;

signals:
    void serialPortOpenedChanged(bool isOpen);
    void writeInfoFirmware(WriteState *state);

public slots:
    void timeout();
    void initBuffer(Buffer *rx, Buffer *tx);
    void seriaRxlBufferCanged();
    void sendFrame(Frame *fr);
    void timeout_test();

signals:
    void scSystemInfo(Frame *frame);
    void scModeInfo(Frame *frame);
    void scEraseInfo(Frame *frame);
    void scWriteInfo(Frame *frame);
    void logChanged();
    void endThread();

private:
    QTimer *timer_timeout;
    Buffer *rx;
    Buffer *tx;

    int ptr_frame=0;
    char prefix_frame;
    char address_frame;

    frame_t frame;

    uint8_t calcCrc(uint8_t* pData, uint8_t length);
};

#endif // SCAN_H
