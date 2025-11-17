#include <QDebug>
#include <QSettings>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>

#include "FirmwareDialog.h"
#include "ui_FirmwareDialog.h"

#include "ConsoleView.h"

#include "scanconst.h"


FirmwareDialog::FirmwareDialog(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::FirmwareDialog)
{
    ui_->setupUi(this);
    serialPortComboBox = new SerialportsComboBox(this);
    ui_->horizontalLayout->insertWidget(0, serialPortComboBox);


    ui_->buttonUpdate->setProperty("isConnected", false);

    connect(serialPortComboBox, SIGNAL(activated(const QString &)), SLOT(serialPortComboBox_changed(const QString &)));
    connect(ui_->buttonUpdate, SIGNAL(clicked()), SLOT(buttonUpdate_triggered()));
    connect(ui_->buttonFile, SIGNAL(clicked()), SLOT(buttonFile_triggered()));
}

FirmwareDialog::~FirmwareDialog()
{
    QSettings settings("editor.conf", QSettings::IniFormat, this);
    delete ui_;
}

void FirmwareDialog::firmwareAppendInfo(WriteState *state)
{
    qDebug() << "boot info" << state->message;

    ui_->labelState->setText(state->message);
    ui_->labelProgress->setText(QString::number(state->progress));
    ui_->labelSize->setText(QString::number(state->size));

    consoleAppendText(state->console);

    if(state->status == W_ERROR || state->status == W_SUCCESS){
        emit serialportClosing();
        return;
    }
}


void FirmwareDialog::serialPortComboBox_changed(const QString& name)
{
    qDebug() << "on_serialPortComboBox_changed: "  << name;
    serialPortComboBox->selectedPortName = name;
}

void FirmwareDialog::buttonUpdate_triggered() {
    if(isFileOpened){
        emit serialportOpening(serialPortComboBox->selectedPortName);
    }
}

void FirmwareDialog::buttonFile_triggered() {
    QSettings settings("editor.conf", QSettings::IniFormat, this);
    QString dir = settings.value("LastOpenDirFirmware").toString();

    //QDir dir = QDir::currentPath();

    QString caption = tr("Открыть файл прошивки");
    QString filter = tr("Firmware file (*.bin)");
    QString fileName = QFileDialog::getOpenFileName(this, caption, dir, filter);

    if (!loadFile(fileName)) {
        isFileOpened = false;
      return;
    }

    isFileOpened = true;
    ui_->labelState->setText("Укажите порт");
    dir = QFileInfo(fileName).dir().path();
    settings.setValue("LastOpenDirFirmware", dir);
    settings.setValue("LastFileFirmware", fileName);
}

void FirmwareDialog::serialPortOpenedChanged(bool isOpen) {
    ui_->buttonUpdate->setProperty("isConnected", isOpen);

    if(ui_->buttonUpdate->property("isConnected") == true) {

        if(isFileOpened){
            //ui_->buttonUpdate->setText("Disconnect");
            serialPortComboBox->setEnabled(false);


            emit firmwareLoading(fileName_);
        }
    }
    else {
        //ui_->buttonUpdate->setText("Connect");
        serialPortComboBox->setEnabled(true);
    }
}

bool FirmwareDialog::loadFile(const QString &fileName) {
    if (fileName.isEmpty()) {
        return false;
    }

    QFile file(fileName);
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        QString message = tr("Could not open %1: %2.").arg(fileName)
                                                      .arg(file.errorString());
        QMessageBox::critical(this,
                              QCoreApplication::applicationName(),
                              message,
                              QMessageBox::Ok);
        return false;
    }

    ui_->labelProgress->setText("0");
    ui_->labelSize->setText(QString::number(file.size()));

    fileName_ = fileName;
    ui_->filePathEdit->setText(fileName_);
    return true;
}

void FirmwareDialog::consoleAppendText(const QString &txt) {
    ui_->console->append(txt);
}
