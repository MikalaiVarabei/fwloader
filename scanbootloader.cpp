#include "scanbootloader.h"
#include <QDebug>

static char tag[] = "ScanBootloader";

ScanBootloader::ScanBootloader(QObject *parent) : QObject(parent)
{
    qDebug() << tag;

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ScanBootloader::state_task);
}

void ScanBootloader::write(const QString& path)
{
    qDebug() << "ScanBootloader::write";
    file = new QFile(path);
    if(!file->exists() || !file->open(QIODevice::ReadOnly)){
        qDebug() << "error open file:" << path;
        timer->stop();
        return;
    }

    qDebug() << "file open:" << path;
    qDebug() << "file size:" << file->size() <<"B";

    sizeFirmware = file->size();
    if(sizeFirmware >= MAX_SIZE_FIRMWARE || sizeFirmware < 1){
        qDebug() << "error size file:" << sizeFirmware <<"B";
        timer->stop();
        return;
    }

    writeState.progress = 0;
    writeState.size = sizeFirmware;

    memset(firmware, 0xFF, MAX_SIZE_FIRMWARE);
    uint32_t readed = file->read((char*)firmware, MAX_SIZE_FIRMWARE);
    if(readed != sizeFirmware){
        qDebug() << "error read file:" << readed;
        timer->stop();
        return;
    }

    crcFirmware = 0;
    for(uint32_t i=0; i<sizeFirmware; i++){
        crcFirmware = ScanConst::getCrc(crcFirmware, firmware[i]);
    }

    qDebug() << "file read complete" << readed << ", crc" << crcFirmware;

    retryCount = 1; // будем делать 2 попытки связи
    state = GetVersionBoot; //проверяем версию бутлодера
    timer->start(10);

    pos = 0;
//    for(;;){
//        memcpy(data, &firmware[pos], W_DATA_SIZE);

//        if(pos+W_DATA_SIZE < sizeFirmware){
//            pos += W_DATA_SIZE;
//            readed = W_DATA_SIZE;
//        }
//        else {
//            readed = sizeFirmware-pos;
//        }

//        //***************** debug ************************
//        QString sdata;
//        for(uint32_t i=0; i<readed; i++){
//            sdata.append('0');sdata.append('x');
//            if(data[i] <= 0x0f) sdata.append('0');
//            sdata.append(QString::number(data[i], 16));sdata.append(',');
//            sdata.append(' ');
//            if(i==7) sdata.append(' ');
//        }

//        qDebug() << sdata;
//        //************************************************

//        if(readed < W_DATA_SIZE){
//            break;
//        }
//    }
    return;
//    emit writed();
}

void ScanBootloader::sendGetVersionBootloader()
{
    tx.prefix = TX_PREFIX;
    tx.address = DEV_ADDRESS;
    tx.cmd = Cmd::B_INFO;
    tx.size = 1;
    setCrc(&tx);
    sendFrame(&tx);
}

bool ScanBootloader::checReceivedBootInfo()
{


    if(rx.cmd == Cmd::B_INFO){

        systemInfo = "ID: \"";
        systemInfo += rx.data[0];
        systemInfo += rx.data[1];
        systemInfo += rx.data[2];
        systemInfo += rx.data[3];
        systemInfo += "\"\n";
        systemInfo += "protokol v" + QString::number(rx.data[4]) + "." + QString::number(rx.data[5]) + ", \n";
        systemInfo += "software v" + QString::number(rx.data[6]) + "." + QString::number(rx.data[7]) + ", \n";
        systemInfo += "hardware v" + QString::number(rx.data[8]) + "." + QString::number(rx.data[9]) + "\n";


    }

    return ((rx.cmd == Cmd::B_INFO) &&
        (rx.data[4] == bootloaderDeviceProtocolVersion[0]) &&
        (rx.data[5] <= bootloaderDeviceProtocolVersion[1]));
}

void ScanBootloader::sendBootloaderMode()
{
    tx.prefix = TX_PREFIX;
    tx.address = DEV_ADDRESS;
    tx.cmd = Cmd::B_MODE;
    tx.size = 2;
    tx.data[0] = (char)(MODE_BOOT & 0xFF);
    setCrc(&tx);
    sendFrame(&tx);
}

bool ScanBootloader::checkReceivedMode()
{
    modeDevice = STATES;

    if(rx.cmd == B_MODE) {
       if(rx.data[0] == (char)(MODE_BOOT & 0xFF)){
            modeDevice = MODE_BOOT;
       }
       if(rx.data[0] == (char)(MODE_APP & 0xFF)){
           modeDevice = MODE_APP;
      }
    }

    return (rx.cmd == Cmd::B_MODE);
}

void ScanBootloader::sendStateScript()
{
    tx.prefix = TX_PREFIX;
    tx.address = DEV_ADDRESS;
    tx.cmd = Cmd::B_MODE;
    tx.size = 2;
    tx.data[0] = (char)(Mode::STATES & 0xFF);
    setCrc(&tx);
    sendFrame(&tx);
}

void ScanBootloader::swReceivedFromDevice(Frame *fr)
{
    rx.prefix = fr->prefix;
    rx.address = fr->address;
    rx.cmd = fr->cmd;
    rx.size = fr->size;
    for(int i=0; i<rx.size-1; i++){
        rx.data[i] = fr->data[i];
    }
    rx.crc = fr->crc;
    isReceived = true;
}

void ScanBootloader::sendUnlockScript()
{
    tx.prefix = TX_PREFIX;
    tx.address = DEV_ADDRESS;
    tx.cmd = Cmd::B_FLASH;
    tx.size = 2;
    tx.data[0] = (char)FlashRequest::UNLOC;
    setCrc(&tx);
    sendFrame(&tx);
}

bool ScanBootloader::checReceivedUnlock()
{
    flashState = static_cast<flashResponse_t>(0xFF & rx.data[0]);
    return (rx.cmd == Cmd::B_FLASH && rx.data[0] == FlashResponse::UNLOCKED);
}

void ScanBootloader::sendEraseScript()
{
    tx.prefix = TX_PREFIX;
    tx.address = DEV_ADDRESS;
    tx.cmd = Cmd::B_FLASH;
    tx.size = 2;
    tx.data[0] = (char)FlashRequest::ERASE;
    setCrc(&tx);
    sendFrame(&tx);
}

bool ScanBootloader::checReceivedErased()
{
    flashState = static_cast<flashResponse_t>(0xFF & rx.data[0]);
    return (rx.cmd == Cmd::B_FLASH && rx.data[0] == FlashResponse::ERASED);
}

bool ScanBootloader::checReceivedErasing()
{
    flashState = static_cast<flashResponse_t>(0xFF & rx.data[0]);
    return (rx.cmd == Cmd::B_FLASH && rx.data[0] == FlashResponse::ERASE_BUSY);
}

bool ScanBootloader::checReceivedErasingError()
{
    flashState = static_cast<flashResponse_t>(0xFF & rx.data[0]);
    return (rx.cmd == Cmd::B_FLASH && rx.data[0] == FlashResponse::ERASE_ERROR);
}

void ScanBootloader::sendWriteSignature()
{
    tx.prefix = TX_PREFIX;
    tx.address = DEV_ADDRESS;
    tx.cmd = Cmd::B_SIGNATURE;
    tx.size = 17;
    tx.data[0] = 0xFF;
    tx.data[1] = 0xFF;
    tx.data[2] = 0xFF;
    tx.data[3] = 0xFF;
    tx.data[4] = 0xFF;
    tx.data[5] = 0xFF;
    tx.data[6] = 0xFF;
    tx.data[7] = 0xFF;
    tx.data[8] = (char)((sizeFirmware & 0xFF000000) >> 24);
    tx.data[9] = (char)((sizeFirmware & 0xFF0000) >> 16);
    tx.data[10] = (char)((sizeFirmware & 0xFF00) >> 8);
    tx.data[11] = (char)(sizeFirmware & 0xFF);
    tx.data[12] = 0x00;
    tx.data[13] = 0x00;
    tx.data[14] = 0x00;
    tx.data[15] = crcFirmware;
    setCrc(&tx);
    sendFrame(&tx);
}

uint8_t ScanBootloader::checkReceivedWriteSignature()
{
    if(rx.cmd == Cmd::B_SIGNATURE)
    {
        return rx.data[0];
    }
    return 0x0;
}

void ScanBootloader::writeFirmwareInit()
{
    address = 0;
}

void ScanBootloader::sendWriteFirmware()
{
    tx.prefix = TX_PREFIX;
    tx.address = DEV_ADDRESS;
    tx.cmd = Cmd::B_WRITE;
    tx.size = 36;
    tx.data[0] = (char)((address & 0xFF0000) >> 16);
    tx.data[1] = (char)((address & 0xFF00) >> 8);
    tx.data[2] = (char)(address & 0xFF);
    incAddress = setDataBuffer(tx.data, 3, 32, address);
    setCrc(&tx);
    sendFrame(&tx);
}

uint8_t ScanBootloader::checkReceivedWriteFirmware()
{
    if( (rx.cmd == Cmd::B_WRITE) &&
        (rx.data[0] == (char)((address & 0xFF0000) >> 16)) &&
        (rx.data[1] == (char)((address & 0xFF00) >> 8)) &&
        (rx.data[2] == (char)(address & 0xFF)))
    {
        return rx.data[3];
    }
    return 0x0;
}



int32_t ScanBootloader::setNextAddres()
{
    address = incAddress;
    return (sizeFirmware - address);
}

void ScanBootloader::sendCheckFirmware()
{
    tx.prefix = TX_PREFIX;
    tx.address = DEV_ADDRESS;
    tx.cmd = Cmd::B_FLASH;
    tx.size = 2;
    tx.data[0] = (char)(FlashRequest::CRC);
    setCrc(&tx);
    sendFrame(&tx);
}

bool ScanBootloader::checReceivedCheckFirmware()
{
    flashState = static_cast<flashResponse_t>(0xFF & rx.data[0]);
    return (rx.cmd == Cmd::B_FLASH && rx.data[0] == FlashResponse::CRC_OK);
}

void ScanBootloader::sendStartApp()
{
    tx.prefix = TX_PREFIX;
    tx.address = DEV_ADDRESS;
    tx.cmd = B_FLASH;
    tx.size = 2;
    tx.data[0] = (char)JUMP;
    setCrc(&tx);
    sendFrame(&tx);
}

bool ScanBootloader::checReceivedStartApp()
{
    flashState = static_cast<flashResponse_t>(0xFF & rx.data[0]);
    return (rx.cmd == Cmd::B_FLASH);
}

/*
 * data - указатель на массив данных
 * offset - количесво байт адреса
 * size - количесво байт данных для записи
 * address - адрес блока
 * */
uint32_t ScanBootloader::setDataBuffer(char *data, uint32_t offset, uint32_t size, uint32_t address)
{
    for(uint32_t i = 0; i < size; i++)
    {
        if (address < sizeFirmware) data[offset + i] = firmware[address++];
        else data[offset + i] = 0xFF;
    }
    return address;
}

void ScanBootloader::setCrc(Frame *fr)
{
    fr->crc = ScanConst::getCrc(0x00, fr->prefix);
    fr->crc = ScanConst::getCrc(fr->crc, fr->address);
    fr->crc = ScanConst::getCrc(fr->crc, fr->cmd);
    fr->crc = ScanConst::getCrc(fr->crc, fr->size);
    for(int i=0; i<fr->size-1; i++){
        fr->crc = ScanConst::getCrc(fr->crc, fr->data[i]);
    }
}


void ScanBootloader::state_task()
{
    switch (state)
    {
        case Ready:
            break;

        // ********* CheckVersion *********
        case GetVersionBoot:
            timeout = 100;
            state = VersionBootWait;
            sendGetVersionBootloader();
            writeState.status = W_STARTING;
            writeState.message = "Подключение";
            writeState.console = writeState.message;
            emit writeInfo(&writeState);
            break;

        case VersionBootWait:
            if (isReceived)
            {
                timeout = -5;
                isReceived = false;
                if (checReceivedBootInfo()) {
                    state = SetModeBoot;
                    retryCount = 3;
                }
                else
                {
                    writeState.status = W_ERROR;
                    writeState.message = "Несовместимый протокол";
                    writeState.console = writeState.message;
                    emit writeInfo(&writeState);
                    state = Event;
                }
                writeState.status = W_CONNECTED;
                writeState.message = "Соединение установлено";
                writeState.console = systemInfo + writeState.message;
                emit writeInfo(&writeState);
                break;
            }
            if (timeout > 0) break;
            if(retryCount > 0)
            {
                retryCount--;
                state = GetVersionBoot;
                break;
            }
            break;

        case SetModeBoot:
            timeout = 100;
            state = SetModeBootWait;
            sendBootloaderMode();
            break;

        case SetModeBootWait:
            if (isReceived)
            {
                timeout = -5;
                isReceived = false;
                if(checkReceivedMode()) {
                    if(modeDevice == MODE_BOOT){
                        state = Unlock;
                        retryCount = 3;
                    }
                    else if(modeDevice == MODE_APP){
                        writeState.status = W_SUCCESS;
                        writeState.message = "Перевод устройсва в режим бутлоадера";
                        writeState.console = "Перевод устройсва в режим Boot, \nчерез 2сек. повторите попытку";
                        emit writeInfo(&writeState);
                        state = Event;
                    }
                }
                else
                {
                    writeState.status = W_ERROR;
                    writeState.message = "Ошибка ответа";
                    writeState.console = writeState.message;
                    emit writeInfo(&writeState);
                    state = Event;
                }
                break;
            }
            if (timeout > 0) break;
            if (retryCount > 0)
            {
                retryCount--;
                state = SetModeBoot;
                break;
            }
            writeState.status = W_ERROR;
            writeState.message = "Не удалось перейти в режим Boot";
            writeState.console = writeState.message;
            emit writeInfo(&writeState);
            state = Event;
            break;

        // ********* Unlock *********
        case Unlock:
            timeout = 100;
            state = UnlockWait;
            sendUnlockScript();
            break;

        case UnlockWait:
            if (isReceived)
            {
                timeout = -5;
                isReceived = false;
                if (checReceivedUnlock()) {
                    state = Erase;
                    retryCount = 3;
                }
                else
                {
                    writeState.status = W_ERROR;
                    writeState.message = "Заблокировано для записи";
                    writeState.console = writeState.message;
                    emit writeInfo(&writeState);
                    state = Event;
                }
                break;
            }
            if (timeout > 0) break;
            if (retryCount > 0)
            {
                retryCount--;
                state = Unlock;
                break;
            }
            break;

        // ********* Erase *********
        case Erase:
            writeState.status = W_WRITING;
            writeState.message = "Очистка памяти";
            writeState.console = writeState.message;
            writeState.console += (retryCount == 1 ? " ..." : (retryCount == 2 ? " .." : " ."));
            emit writeInfo(&writeState);
            timeout = 100;
            state = EraseWait;
            sendEraseScript();
            break;

        case EraseWait:
            if (isReceived)
            {
                timeout = -5;
                isReceived = false;
                /*
                    в зависимости от типа загрузчика стирание может производиться после проверки
                    цифровой подписи после отправки нулевого адреса памяти,
                    при этом ответ устройсва на команду "очистить" будет - "очистка завершена"
                */
                if (checReceivedErased()) state = WriteFirmwareInit;
                else if (checReceivedErasing()) state = Erase;
                else if (checReceivedErasingError())
                {
                    writeState.status = W_ERROR;
                    writeState.message = "Ошибка стирания";
                    writeState.console = writeState.message;
                    emit writeInfo(&writeState);
                    state = Event;
                }
                else
                {
                    writeState.status = W_ERROR;
                    writeState.message = "Ошибка протокола";
                    writeState.console = writeState.message;
                    emit writeInfo(&writeState);
                    state = Event;
                }
                break;
            }
            if (timeout > 0) break;
            if (retryCount > 0)
            {
                retryCount--;
                state = Erase;
                break;
            }
            break;

        // ********** WriteFirmware *********
        case WriteFirmwareInit:
            writeState.status = W_WRITING;
            writeState.message = "Подготовка к записи";
            writeState.console = writeState.message;
            emit writeInfo(&writeState);
            writeFirmwareInit();
            state = WriteSignature;
            retryCount = 10;
            waitErase = 25;
            break;

        case WriteSignature:
            timeout = 50;
            state = WriteSignatureWait;
            sendWriteSignature();
            break;

        case WriteSignatureWait:
            if (isReceived)
            {
                timeout = -5;
                isReceived = false;
                if(checkReceivedWriteSignature() == WriteResponse::SUCCESS)
                {
                    state = WriteFirmware;
                    retryCount = 5;
                }
                else if (checkReceivedWriteSignature() == WriteResponse::BUSY) //отправляем повторно
                {
                    retryCount--;
                    state = WriteSignature;
                }
                else {
                    writeState.status = W_ERROR;
                    writeState.message = "Ошибка записи подписи";
                    writeState.console = writeState.message;
                    emit writeInfo(&writeState);
                    state = Event;
                }
                break;
            }
            if (timeout > 0) break;
            if (retryCount > 0)
            {
                retryCount--;
                state = WriteSignature;
                break;
            }
            break;

        case WriteFirmware:
            timeout = 50;
            state = WriteFirmwareWait;
            sendWriteFirmware();
            break;

        case WriteFirmwareWait:
            if (isReceived)
            {
                timeout = -5;
                isReceived = false;
                if(checkReceivedWriteFirmware() == WriteResponse::SUCCESS)
                {

                    if (setNextAddres() > 0) //записываем следующие 32 байта
                    {
                        writeState.status = W_WRITING;
                        writeState.progress = address;
                        double size = (double)address*100/sizeFirmware;
                        writeState.message = "Запись данных "+QString::number(size, 'q', 1)+"%";
                        writeState.console = '\r' + writeState.message;
                        emit writeInfo(&writeState);
                        qDebug() << address <<"/" <<sizeFirmware << "Byte";
                        state = WriteFirmware;
                        retryCount = 5;
                    }
                    else if(setNextAddres() == 0) //дошли до конца
                    {
                        writeState.status = W_WRITING;
                        writeState.progress = sizeFirmware;
                        writeState.message = "Проверка флеш";
                        writeState.console = writeState.message;
                        emit writeInfo(&writeState);
                        qDebug() << address <<"/" <<sizeFirmware << "Byte";
                        state = CheckFirmware;
                        retryCount = 5;
                    }
                    else
                    {
                        writeState.status = W_ERROR;
                        writeState.message = "Ошибка размера буфера";
                        writeState.console = writeState.message;
                        emit writeInfo(&writeState);
                        state = Event;
                    }
                }
                else if (checkReceivedWriteFirmware() == WriteResponse::BUSY) //отправляем повторно
                {
                    //стирание флешь производится после проверки цифровой подписи
                    //необходимо больше времени
                    writeState.status = W_WRITING;
                    writeState.message = "Стирание";
                    writeState.console = writeState.message;
                    emit writeInfo(&writeState);
                    retryCount = waitErase--;
                    state = WriteFirmware;
                }
                else
                {

                    writeState.status = W_ERROR;
                    if (checkReceivedWriteFirmware() == WriteResponse::ERROR_ADDRESS) {
                        writeState.message = "Ошибка 2, не верно задан адрес";
                    }
                    else if (checkReceivedWriteFirmware() == WriteResponse::ERROR_WRITE) {
                        writeState.message = "Ошибка 3, данные не записаны";
                    }
                    else if (checkReceivedWriteFirmware() == WriteResponse::ERROR_ERASE) {
                        writeState.message = "Ошибка 4, память не очищена";
                    }
                    else if (checkReceivedWriteFirmware() == WriteResponse::ERROR_SYGNATURE) {
                        writeState.message = "Ошибка 5, неверная подпись";
                    }
                    else {
                        writeState.message = "Неизвестная ошибка";
                    }

                    writeState.console = writeState.message;
                    emit writeInfo(&writeState);
                    state = Event;
                }
                break;
            }
            if (timeout > 0) break;
            if (retryCount > 0)
            {
                retryCount--;
                state = WriteFirmware;
                break;
            }
            break;

        // ********** CheckFirmware *********
        case CheckFirmware:
            timeout = 1000;
            state = CheckFirmwareWait;
            sendCheckFirmware();
            break;

        case CheckFirmwareWait:
            if (isReceived)
            {
                timeout = 0;
                isReceived = false;
                if (checReceivedCheckFirmware())
                {
                    writeState.status = W_WRITING;
                    writeState.message = "\b: ok\n";
                    writeState.message += "Переход к управляющей программе";
                    writeState.console = writeState.message;
                    emit writeInfo(&writeState);
                    state = GotoApp;
                    retryCount = 2;
                }
                else
                {
                    writeState.status = W_ERROR;
                    writeState.message = "Ошибка проверки управляющей программы";
                    writeState.console = writeState.message;
                    emit writeInfo(&writeState);
                    state = Event;
                }
                break;
            }
            if (timeout > 0) break;
            if (retryCount > 0)
            {
                retryCount--;
                state = CheckFirmware;
                break;
            }
            break;


        case GotoApp:
            timeout = 100;
            state = GotoAppWait;
            sendStartApp();
            break;

        case GotoAppWait:
            if (isReceived)
            {
                timeout = -5;
                isReceived = false;
                if (checReceivedStartApp())
                {
                    if(flashState == FlashResponse::JUMP_OK) {
                        writeState.status = W_SUCCESS;
                        writeState.message = "Успешно";
                    }
                    else if(flashState == FlashResponse::JUMP_DENIED) {
                        writeState.status = W_SUCCESS;
                        writeState.message = "Отклонено";
                    }
                    else {
                        writeState.status = W_ERROR;
                        writeState.message = "Ошибка перехода к управляющей программе 0x" + QString::number(flashState, 16);
                    }

                    writeState.console = writeState.message;
                    emit writeInfo(&writeState);
                    state = Event;
                }
                else
                {
                    writeState.status = W_ERROR;
                    writeState.message = "Ошибка ответа";
                    writeState.console = writeState.message;
                    emit writeInfo(&writeState);
                    state = Event;
                }
                break;
            }
            if (timeout > 0) break;
            if (retryCount > 0)
            {
                retryCount--;
                state = GotoApp;
                break;
            }
            break;

        case Event:
            timeout = -5;
            state = Ready;
            //CloseComPort();
            //Program.form.Invoke((MethodInvoker)(() => MessageBox.Show(textEVent)));
            //MessageBox.Show(textEVent);
            break;

        default:
            state = Ready;
            break;
    }

    if(timeout > -5) timeout--;
    if(timeout == -4)
    {
        timeout = -5;
        writeState.status = W_ERROR;
        writeState.message = "Нет ответа от устройсва";
        writeState.console = writeState.message;
        emit writeInfo(&writeState);
        state = Event;
    }
}
