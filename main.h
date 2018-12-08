#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QtCore/QThread>
#include <QtCore/QArgument>

class MyThread : public QThread {
Q_OBJECT

protected:
    void run();
};

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

private:
    Ui::MainWindow *ui;
};

int qt_init(int argc, char *argv[]);
void initialize();
void finish();

#endif
