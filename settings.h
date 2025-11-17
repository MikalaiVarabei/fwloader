#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtCore>
#include <QObject>

class Settings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString compilerFlags MEMBER compilerFlags)

public:
    explicit Settings(QObject *parent = nullptr);
    Q_INVOKABLE void init();

    QString compilerFlags;
    QString scriptSuffix;

    QString serialPortName;

    void setSerialPortName(const QString &value);

signals:

private:
};

#endif // SETTINGS_H
