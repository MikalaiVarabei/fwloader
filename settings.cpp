#include "settings.h"
#include "configs.h"

#define PAWN_COMPILERFLAG "Pawn/CompilerFlags"
#define PAWN_SCRIPTSUFFIX "Pawn/ScriptSuffix"

#define SERIALPORT_NAME "Serialport/Name"


Settings::Settings(QObject *parent) : QObject(parent)
{
}

void Settings::init()
{
    QSettings settings("settings.conf", QSettings::IniFormat, this);

    compilerFlags = settings.value(PAWN_COMPILERFLAG, "").toString();
    if(compilerFlags.isEmpty()) {
        compilerFlags = PAWN_DEFAULT_COMPILER_FLAGS;
        settings.setValue(PAWN_COMPILERFLAG, compilerFlags);
        qDebug() <<  "set compilerFlags";
    }

    scriptSuffix = settings.value(PAWN_SCRIPTSUFFIX, "").toString();
    if(scriptSuffix.isEmpty()) {
        scriptSuffix = PAWN_DEFAULT_SCRIPTSUFFIX;
        settings.setValue(PAWN_SCRIPTSUFFIX, scriptSuffix);
        qDebug() <<  "set scriptSuffix";
    }

    serialPortName = settings.value(SERIALPORT_NAME, "").toString();
    if(serialPortName.isEmpty()) {
        serialPortName = SERIALPORT_DEFAULT_NAME;
        settings.setValue(SERIALPORT_NAME, serialPortName);
        qDebug() <<  "set serialPortName";
    }
}

void Settings::setSerialPortName(const QString &value)
{
    QSettings settings("settings.conf", QSettings::IniFormat, this);
    serialPortName = value;
    settings.setValue(SERIALPORT_NAME, serialPortName);
}
