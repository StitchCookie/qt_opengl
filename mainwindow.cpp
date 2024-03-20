#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    QString path = QFileDialog::getOpenFileName(this,"请选择openGL模型文件","../modelResource","OBJ (*.obj);;FBX(*.fbx);;ALL FILES( *.* )");
    ui->openGLWidget->loadModel(path.toStdString());
}
