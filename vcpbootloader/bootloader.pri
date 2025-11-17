INCLUDEPATH += $$PWD

RESOURCES += $$PWD/bootloader.qrc

SOURCES += \
    $$PWD/src/ConsoleView.cpp \
    $$PWD/src/FirmwareDialog.cpp

HEADERS += \
    $$PWD/src/ConsoleView.h \
    $$PWD/src/FirmwareDialog.h


FORMS += \
    $$PWD/src/FirmwareDialog.ui
