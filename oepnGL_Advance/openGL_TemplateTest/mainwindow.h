#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector4D>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_Rect_triggered();

    void on_action_clear_triggered();

    void on_action_xuanran_triggered();

    void on_actionModel_triggered();

    void slots_getMousePos(QVector4D pos);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
