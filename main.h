#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QtCore/QThread>
#include <QtCore/QArgument>
#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>

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
    void on_file_chooser_dst_clicked();
    void on_file_chooser_src_clicked();
    void on_button_go_clicked();
    void on_button_clear_clicked();
    QList<QList<QVariant>> get_duplicates_from_hash(const QString hash);

    ~MainWindow();
    Ui::MainWindow *ui;

};

#endif
