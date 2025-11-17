#ifndef FIRMWAREDIALOG_H
#define FIRMWAREDIALOG_H

#include <QDialog>
#include <QString>
#include <QComboBox>
#include <QPushButton>
#include <QSerialPortInfo>
#include "settings.h"
#include "scanconst.h"

namespace Ui {
  class FirmwareDialog;
}

class SerialportsComboBox : public QComboBox
{
Q_OBJECT
public:
    explicit SerialportsComboBox(QWidget *parent = nullptr){
        selectedPortName = "";
        showPopup();
    }

public:
    virtual void showPopup()
    {
        while (count() > 0) { removeItem(0); }

        QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
        QList<QSerialPortInfo>::iterator p;

        Settings settings;
        settings.init();

        clear();

        for (auto p = list.begin(); p != list.end(); ++p)
        {
            addItem(p->portName());
            if(selectedPortName == "" && p->portName() == settings.serialPortName){
                setCurrentText(p->portName());
                selectedPortName = p->portName();
            }
            else if(selectedPortName == p->portName()){
                setCurrentText(p->portName());
            }
        }

        QComboBox::showPopup();
    }

public:
    QString selectedPortName;

};

class FirmwareDialog: public QDialog {
    Q_OBJECT


public:
    explicit FirmwareDialog(QWidget *parent = 0);
    ~FirmwareDialog() override;

public slots:
    void firmwareAppendInfo(WriteState *state);
    void serialPortOpenedChanged(bool isOpen);
    void serialPortComboBox_changed(const QString& name);
    void buttonUpdate_triggered();
    void buttonFile_triggered();


    bool loadFile(const QString &fileName);
    void consoleAppendText(const QString& txt);



signals:
    void firmwareLoading(QString script);
    void serialportOpening(const QString &portname);
    void serialportClosing();


private:
    Ui::FirmwareDialog *ui_;
    SerialportsComboBox *serialPortComboBox;

    QString fileName_;
    bool isFileOpened=false;
};

#endif // FIRMWAREDIALOG_H
