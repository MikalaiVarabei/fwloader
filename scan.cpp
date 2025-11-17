#include <QDebug>
#include <QTimer>
#include <QFile>
#include <QDateTime>


#include "scan.h"


Scan::Scan(QObject *parent) : QObject(parent)
{
    serialport = new Serialport(this);
    connect(serialport, &Serialport::initBuffer, this, &Scan::initBuffer);
    connect(serialport, &Serialport::serialRxBufferCanged, this, &Scan::seriaRxlBufferCanged);

    connect(serialport, &Serialport::serialPortOpenedChanged, this, [=](bool isopen){ serialPortOpenedChanged(isopen); });
    connect(&scanBootloader, &ScanBootloader::writeInfo, this, [=](WriteState *state){emit writeInfoFirmware(state);});
    connect(&scanBootloader,   &ScanBootloader::sendFrame, this, &Scan::sendFrame);
    serialport->init();

    timer_timeout = new QTimer(this);
    connect(timer_timeout, &QTimer::timeout, this, &Scan::timeout);
    timer_timeout->setInterval(500);

//    QTimer *timer = new QTimer(this);
//    connect(timer, &QTimer::timeout, this, &Scan::timeout_test);
//    timer->start(100);
}

Scan::~Scan()
{
//    QFile file("test.bin");
//    if (!file.open(QIODevice::WriteOnly)){
//        qDebug() << "file not open";
//    }

//    file.write(bin);
//     file.flush();
//     file.close();
    emit endThread();
}

void Scan::timeout_test(){
    QString txt = "time: " + QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
}

void Scan::timeout()
{
    timer_timeout->stop();
    ptr_frame = 0;
    qDebug() << "timeout()";
}

void Scan::initBuffer(Buffer *rx, Buffer *tx)
{
    this->rx = rx;
    this->tx = tx;
    ptr_frame = 0;
}

void Scan::seriaRxlBufferCanged()
{
//    QString sdata;
//    QString str_data;
    for(;;){

        //***************** debug ************************

//            if((uint8_t)rx->buffer[rx->ptr_out] <= 0xF) sdata.append('0');
//            sdata.append(QString::number((uint8_t)rx->buffer[rx->ptr_out], 16).toUpper());
//            sdata.append(' ');

//            //str_data.append(rx->buffer[rx->ptr_out]);
        //************************************************

        switch (ptr_frame) {
        case 0:
            if(rx->buffer[rx->ptr_out] == RX_PREFIX){ //встретили начало кадра
                prefix_frame = RX_PREFIX;
                frame.crc = ScanConst::getCrc(0, RX_PREFIX);
                timer_timeout->start(800);
                ptr_frame = 1;
            }
            break;

        case 1:
            if(rx->buffer[rx->ptr_out] == DEV_ADDRESS){
                address_frame = DEV_ADDRESS;
                frame.crc = ScanConst::getCrc(frame.crc, DEV_ADDRESS);
                ptr_frame = 2;
            }
            else if(rx->buffer[rx->ptr_out] == RX_PREFIX){ //встретили начало кадра
                prefix_frame = RX_PREFIX;
                frame.crc = ScanConst::getCrc(0, RX_PREFIX);
                timer_timeout->start(800);
                ptr_frame = 1;
            }
            else {
                ptr_frame = 0;
            }
            break;

        case 2:
            frame.cmd = static_cast<cmd_t>(0xFF & rx->buffer[rx->ptr_out]);
            frame.crc = ScanConst::getCrc(frame.crc, frame.cmd);
            ptr_frame = 3;
            break;

        case 3:
            if(frame.cmd == Cmd::B_INFO ||
                    frame.cmd == Cmd::B_MODE ||
                    frame.cmd == Cmd::B_FLASH ||
                    frame.cmd == Cmd::B_WRITE ||
                    frame.cmd == Cmd::B_SIGNATURE)
            {
                ptr_frame = 6;
            }
            else
            {
                timer_timeout->stop();
                ptr_frame = 0;
                break;
            }

            frame.prefix = prefix_frame;
            frame.address = address_frame;
            frame.size = rx->buffer[rx->ptr_out];
            frame.crc = ScanConst::getCrc(frame.crc, frame.size);
            frame.index = 0;
            break;


        case 6:
            frame.data[frame.index] = rx->buffer[rx->ptr_out];
            frame.index++;
            //qDebug() << "index" << frame.index;
            if(frame.index == frame.size){
                timer_timeout->stop();
                ptr_frame = 0;
                if(frame.crc == static_cast<unsigned char>(0xFF & frame.data[frame.index-1])){
                    //qDebug() << "cmd:" << QString::number(frame.cmd, 16).toUpper();
                    scanBootloader.swReceivedFromDevice(&frame);
                }
            }
            else{
                frame.crc = ScanConst::getCrc(frame.crc, static_cast<unsigned char>(0xFF & frame.data[frame.index-1]));
            }

            //qInfo() << "crc" << QString::number((unsigned char)frame.crc,16) << QString::number((unsigned char)frame.data[frame.index-1], 16);

            break;

        default:
            ptr_frame = 0;
            break;
        }

        //rx->ptr_out++;
        if(++rx->ptr_out >= DATA_SIZE) rx->ptr_out = 0;
        if(rx->ptr_out == rx->ptr_in) {
//            qDebug() << ">" << sdata;
//            //qDebug() << ">" << str_data;
            break;
        }
    }

    //qDebug() << "ptr_out" << rx->ptr_out;
}

void Scan::sendFrame(Frame *fr)
{
    tx->buffer[0] = fr->prefix;
    tx->buffer[1] = fr->address;
    tx->buffer[2] = fr->cmd;
    tx->buffer[3] = fr->size;

    for(tx->ptr_in = 0; (uint8_t)tx->ptr_in < fr->size-1; tx->ptr_in++)
    {
        tx->buffer[4+tx->ptr_in] = fr->data[tx->ptr_in];
    }

    tx->buffer[3+fr->size] = fr->crc;

    //emit transmit(tx->buffer, fr->size+4);
    serialport->transmit(tx->buffer, fr->size+4);
}


//void Scan::scriptConsoleLogAppend(Frame *frame)
//{
//    QString data;
//    for(int i=0; i<frame->size-1; i++){
//        data.push_back(frame->data[i]);
//    }

//    auto jsObj = QJsonObject(
//    {
//        qMakePair(QString("time"), QString(QDateTime::currentDateTime().toString("hh:mm:ss.zzz"))),
//        qMakePair(QString("dataAscii"), data)
//    });

//    emit scriptConsoleLog(jsObj);

////    jsArray.push_back(QJsonValue(jsObj));
//    //    emit logChanged();
//}

uint8_t Scan::calcCrc(uint8_t *pData, uint8_t length)
{
    uint8_t i, crc8;
    crc8 = 0x00;
    for(i = 0; i < length; i++) {
        crc8 = ScanConst::getCrc(crc8, pData[i]);
    }
    return crc8;
}
