/********************************************************************************
** Form generated from reading UI file 'FactSim.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FACTSIM_H
#define UI_FACTSIM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FactSimClass
{
public:
    QMenuBar *menuBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *FactSimClass)
    {
        if (FactSimClass->objectName().isEmpty())
            FactSimClass->setObjectName(QStringLiteral("FactSimClass"));
        FactSimClass->resize(600, 400);
        menuBar = new QMenuBar(FactSimClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        FactSimClass->setMenuBar(menuBar);
        centralWidget = new QWidget(FactSimClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        FactSimClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(FactSimClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        FactSimClass->setStatusBar(statusBar);

        retranslateUi(FactSimClass);

        QMetaObject::connectSlotsByName(FactSimClass);
    } // setupUi

    void retranslateUi(QMainWindow *FactSimClass)
    {
        FactSimClass->setWindowTitle(QApplication::translate("FactSimClass", "FactSim", 0));
    } // retranslateUi

};

namespace Ui {
    class FactSimClass: public Ui_FactSimClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FACTSIM_H
