#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

#include <QMetaType>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qRegisterMetaType<QVector4D>("QVector4D");
    connect(ui->openGLWidget,&COpenGlWidget::signal_mousePickingPos,this,&MainWindow::slots_getMousePos);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_action_Rect_triggered()
{
    ui->openGLWidget->drawShape(COpenGlWidget::Rect);
}

void MainWindow::on_action_clear_triggered()
{
    ui->openGLWidget->drawShape(COpenGlWidget::None);
}

void MainWindow::on_action_xuanran_triggered()
{
    ui->openGLWidget->setWrirFame(ui->action_xuanran->isChecked());
}

void MainWindow::on_actionModel_triggered()
{
    QString path = QFileDialog::getOpenFileName(this,"请选择openGL模型文件","../../../modelResource","OBJ (*.obj);;FBX(*.fbx);;ALL FILES( *.* )");
    ui->openGLWidget->loadModel(path.toStdString());
}

void MainWindow::slots_getMousePos(QVector4D pos)
{
    ui->statusBar->setStyleSheet("font-size:14pt");
    QString worldPos = QString("世界坐标: X:%1 Y:%2 Z:%3 W:%4")
                           .arg(QString::number(pos.x(),'f',2))
                           .arg(QString::number(pos.y(),'f',2))
                           .arg(QString::number(pos.z(),'f',2))
                           .arg(QString::number(pos.w(),'f',2)) ;
    ui->statusBar->showMessage(worldPos);
}
