/********************************************************************************
** Form generated from reading UI file 'FirmwareDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FIRMWAREDIALOG_H
#define UI_FIRMWAREDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <vcpbootloader/src/ConsoleView.h>

QT_BEGIN_NAMESPACE

class Ui_FirmwareDialog
{
public:
    QAction *actionbuttonFile;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBoxSerialPort;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QGroupBox *groupBoxControl;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *buttonUpdate;
    QPushButton *buttonFile;
    QLineEdit *filePathEdit;
    QHBoxLayout *horizontalLayout_labels;
    QLabel *labelState;
    QSpacerItem *horizontalSpacer_5;
    QSpacerItem *horizontalSpacer_4;
    QSpacerItem *horizontalSpacer_2;
    QLabel *labelProgress;
    QLabel *labelSeparator;
    QLabel *labelSize;
    QSpacerItem *horizontalSpacer_3;
    ConsoleView *console;

    void setupUi(QDialog *FirmwareDialog)
    {
        if (FirmwareDialog->objectName().isEmpty())
            FirmwareDialog->setObjectName(QString::fromUtf8("FirmwareDialog"));
        FirmwareDialog->resize(800, 138);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(FirmwareDialog->sizePolicy().hasHeightForWidth());
        FirmwareDialog->setSizePolicy(sizePolicy);
        FirmwareDialog->setMaximumSize(QSize(800, 138));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/assets/images/boot.ico"), QSize(), QIcon::Normal, QIcon::Off);
        FirmwareDialog->setWindowIcon(icon);
        FirmwareDialog->setModal(false);
        actionbuttonFile = new QAction(FirmwareDialog);
        actionbuttonFile->setObjectName(QString::fromUtf8("actionbuttonFile"));
        actionbuttonFile->setIconVisibleInMenu(true);
        horizontalLayout_3 = new QHBoxLayout(FirmwareDialog);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        groupBoxSerialPort = new QGroupBox(FirmwareDialog);
        groupBoxSerialPort->setObjectName(QString::fromUtf8("groupBoxSerialPort"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(groupBoxSerialPort->sizePolicy().hasHeightForWidth());
        groupBoxSerialPort->setSizePolicy(sizePolicy1);
        horizontalLayout = new QHBoxLayout(groupBoxSerialPort);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(437, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addWidget(groupBoxSerialPort);

        groupBoxControl = new QGroupBox(FirmwareDialog);
        groupBoxControl->setObjectName(QString::fromUtf8("groupBoxControl"));
        sizePolicy1.setHeightForWidth(groupBoxControl->sizePolicy().hasHeightForWidth());
        groupBoxControl->setSizePolicy(sizePolicy1);
        horizontalLayout_2 = new QHBoxLayout(groupBoxControl);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        buttonUpdate = new QPushButton(groupBoxControl);
        buttonUpdate->setObjectName(QString::fromUtf8("buttonUpdate"));

        horizontalLayout_2->addWidget(buttonUpdate);

        buttonFile = new QPushButton(groupBoxControl);
        buttonFile->setObjectName(QString::fromUtf8("buttonFile"));

        horizontalLayout_2->addWidget(buttonFile);

        filePathEdit = new QLineEdit(groupBoxControl);
        filePathEdit->setObjectName(QString::fromUtf8("filePathEdit"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(filePathEdit->sizePolicy().hasHeightForWidth());
        filePathEdit->setSizePolicy(sizePolicy2);

        horizontalLayout_2->addWidget(filePathEdit);


        verticalLayout->addWidget(groupBoxControl);

        horizontalLayout_labels = new QHBoxLayout();
        horizontalLayout_labels->setSpacing(0);
        horizontalLayout_labels->setObjectName(QString::fromUtf8("horizontalLayout_labels"));
        labelState = new QLabel(FirmwareDialog);
        labelState->setObjectName(QString::fromUtf8("labelState"));

        horizontalLayout_labels->addWidget(labelState);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_labels->addItem(horizontalSpacer_5);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_labels->addItem(horizontalSpacer_4);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_labels->addItem(horizontalSpacer_2);

        labelProgress = new QLabel(FirmwareDialog);
        labelProgress->setObjectName(QString::fromUtf8("labelProgress"));

        horizontalLayout_labels->addWidget(labelProgress);

        labelSeparator = new QLabel(FirmwareDialog);
        labelSeparator->setObjectName(QString::fromUtf8("labelSeparator"));

        horizontalLayout_labels->addWidget(labelSeparator);

        labelSize = new QLabel(FirmwareDialog);
        labelSize->setObjectName(QString::fromUtf8("labelSize"));

        horizontalLayout_labels->addWidget(labelSize);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_labels->addItem(horizontalSpacer_3);


        verticalLayout->addLayout(horizontalLayout_labels);


        horizontalLayout_3->addLayout(verticalLayout);

        console = new ConsoleView(FirmwareDialog);
        console->setObjectName(QString::fromUtf8("console"));

        horizontalLayout_3->addWidget(console);


        retranslateUi(FirmwareDialog);

        QMetaObject::connectSlotsByName(FirmwareDialog);
    } // setupUi

    void retranslateUi(QDialog *FirmwareDialog)
    {
        FirmwareDialog->setWindowTitle(QCoreApplication::translate("FirmwareDialog", "Firmware Loader", nullptr));
        actionbuttonFile->setText(QCoreApplication::translate("FirmwareDialog", "buttonFile", nullptr));
#if QT_CONFIG(tooltip)
        actionbuttonFile->setToolTip(QCoreApplication::translate("FirmwareDialog", "buttonFile", nullptr));
#endif // QT_CONFIG(tooltip)
        groupBoxSerialPort->setTitle(QString());
        groupBoxControl->setTitle(QString());
        buttonUpdate->setText(QCoreApplication::translate("FirmwareDialog", "Load", nullptr));
        buttonFile->setText(QCoreApplication::translate("FirmwareDialog", "File", nullptr));
        labelState->setText(QCoreApplication::translate("FirmwareDialog", "\320\243\320\272\320\260\320\266\320\270\321\202\320\265 \320\277\321\203\321\202\321\214 \320\272 \321\204\320\260\320\271\320\273\321\203 \320\277\321\200\320\276\321\210\320\270\320\262\320\272\320\270", nullptr));
        labelProgress->setText(QCoreApplication::translate("FirmwareDialog", "0", nullptr));
        labelSeparator->setText(QCoreApplication::translate("FirmwareDialog", "/", nullptr));
        labelSize->setText(QCoreApplication::translate("FirmwareDialog", "0", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FirmwareDialog: public Ui_FirmwareDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FIRMWAREDIALOG_H
