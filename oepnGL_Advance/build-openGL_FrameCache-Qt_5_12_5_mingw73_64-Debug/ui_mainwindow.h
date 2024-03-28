/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include <copenglwidget.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action_Rect;
    QAction *action_clear;
    QAction *action_xuanran;
    QAction *actionModel;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    COpenGlWidget *openGLWidget;
    QToolBar *toolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 600);
        MainWindow->setMinimumSize(QSize(800, 600));
        action_Rect = new QAction(MainWindow);
        action_Rect->setObjectName(QString::fromUtf8("action_Rect"));
        action_Rect->setCheckable(false);
        action_Rect->setEnabled(true);
        action_clear = new QAction(MainWindow);
        action_clear->setObjectName(QString::fromUtf8("action_clear"));
        action_xuanran = new QAction(MainWindow);
        action_xuanran->setObjectName(QString::fromUtf8("action_xuanran"));
        actionModel = new QAction(MainWindow);
        actionModel->setObjectName(QString::fromUtf8("actionModel"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        openGLWidget = new COpenGlWidget(centralwidget);
        openGLWidget->setObjectName(QString::fromUtf8("openGLWidget"));

        gridLayout->addWidget(openGLWidget, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        toolBar->addAction(action_Rect);
        toolBar->addAction(action_clear);
        toolBar->addAction(action_xuanran);
        toolBar->addAction(actionModel);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "openGL\346\267\261\345\272\246\346\265\213\350\257\225", nullptr));
        action_Rect->setText(QApplication::translate("MainWindow", "\347\273\230\345\210\266", nullptr));
#ifndef QT_NO_TOOLTIP
        action_Rect->setToolTip(QApplication::translate("MainWindow", "\347\224\273\344\270\200\344\270\252\347\237\251\345\275\242", nullptr));
#endif // QT_NO_TOOLTIP
        action_clear->setText(QApplication::translate("MainWindow", "\346\270\205\351\231\244", nullptr));
#ifndef QT_NO_TOOLTIP
        action_clear->setToolTip(QApplication::translate("MainWindow", "\346\270\205\351\231\244\347\237\251\345\275\242", nullptr));
#endif // QT_NO_TOOLTIP
        action_xuanran->setText(QApplication::translate("MainWindow", "\346\270\262\346\237\223", nullptr));
#ifndef QT_NO_TOOLTIP
        action_xuanran->setToolTip(QApplication::translate("MainWindow", "\346\270\262\346\237\223\347\237\251\345\275\242", nullptr));
#endif // QT_NO_TOOLTIP
        actionModel->setText(QApplication::translate("MainWindow", "\346\250\241\345\236\213", nullptr));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
