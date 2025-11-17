#ifndef SCANBOOTLOADER_H
#define SCANBOOTLOADER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QTimer>

#include "scanconst.h"

#define MAX_SIZE_FIRMWARE (0xFFFFF)

class ScanBootloader : public QObject
{
    Q_OBJECT
public:
    explicit ScanBootloader(QObject *parent = nullptr);

public Q_SLOTS:
    void write(const QString& path);
    void swReceivedFromDevice(Frame *rx);
    void state_task();

signals:
    void writed();
    void writeInfo(WriteState *state);
    void sendFrame(Frame *rx);

private:
    void sendGetVersionBootloader();
    bool checReceivedBootInfo();
    void sendBootloaderMode();
    bool checkReceivedMode();
    void sendStateScript();
    void sendUnlockScript();
    bool checReceivedUnlock();
    void sendEraseScript();
    bool checReceivedErased();
    bool checReceivedErasing();
    bool checReceivedErasingError();
    void sendWriteSignature();
    uint8_t checkReceivedWriteSignature();
    void writeFirmwareInit();
    void sendWriteFirmware();
    uint8_t checkReceivedWriteFirmware();
    int32_t setNextAddres();
    void sendCheckFirmware();
    bool checReceivedCheckFirmware();
    void sendStartApp();
    bool checReceivedStartApp();
    uint32_t setDataBuffer(char *data, uint32_t offset, uint32_t size, uint32_t addr);
    void setCrc(Frame *frame);

    typedef enum {
        Ready,
        GetVersionBoot, VersionBootWait,
        SetModeBoot, SetModeBootWait,
        /*StatesScript, StatesScriptWait,*/
        Unlock, UnlockWait,
        Erase, EraseWait,
        GetState, GetStateWait,
        WriteSignature, WriteSignatureWait, WriteFirmwareInit, WriteFirmware, WriteFirmwareWait,
        CheckFirmware, CheckFirmwareWait,
        GotoApp, GotoAppWait,
        Event
    } state_t;

    QString message;
    QTimer *timer;

    QFile *file;
    uint32_t pos=0;

    frame_t rx;
    frame_t tx;

    writeState_t writeState;

    state_t state;
    mode_t modeDevice;
    flashResponse_t flashState;

    bool isReceived = false;
    int timeout = -5;
    int retryCount; //счетчик попыток
    int waitErase;

    uint8_t firmware[MAX_SIZE_FIRMWARE];
    uint32_t sizeFirmware=0;
    uint8_t crcFirmware=0;
    uint32_t address;
    uint32_t incAddress;

    QString systemInfo;

};

#endif // SCANBOOTLOADER_H
