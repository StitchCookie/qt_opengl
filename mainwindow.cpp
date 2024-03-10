#include "mainwindow.h"
#include "ui_mainwindow.h"

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
