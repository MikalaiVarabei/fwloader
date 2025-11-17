#ifndef CONSOLEVIEW_H
#define CONSOLEVIEW_H

#include <QPlainTextEdit>

class ConsoleView: public QPlainTextEdit {
    Q_OBJECT
public:
    explicit ConsoleView(QWidget *parent = 0);
    ~ConsoleView() override;

    void append(const QString &text);
};

#endif // CONSOLEVIEW_H
