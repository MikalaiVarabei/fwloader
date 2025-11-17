#include "ConsoleView.h"

static QFont defaultFont() {
  #ifdef Q_OS_WINDOWS
    QFont font("Courier New");
  #else
    QFont font("Monospace");
  #endif
  font.setStyleHint(QFont::Monospace);
  return font;
}

ConsoleView::ConsoleView(QWidget *parent):
    QPlainTextEdit(parent)
{
    QFont font = defaultFont();
    setFont(font);
}

ConsoleView::~ConsoleView()
{
}

void ConsoleView::append(const QString &txt)
{
    if(txt.at(0) == '\r') { //удаляем строку
        QString ntxt = txt;
        ntxt.remove('\r');

        setFocus();
        QTextCursor storeCursorPos = textCursor();
        moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
        moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
        moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
        textCursor().removeSelectedText();
        textCursor().deletePreviousChar();
        setTextCursor(storeCursorPos);
        appendPlainText(ntxt);
        return;
    }
    else if(txt.at(0) == '\b') { //удаляем строку
        QString ntxt = txt;
        ntxt.remove('\b');
        insertPlainText(ntxt);
        return;
    }

    appendPlainText(txt);
}
