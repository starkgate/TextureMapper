#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QMainWindow>
#include <QApplication>
#include <QtSql/QSqlDatabase>
#include <QtCore/QThread>
#include <QtCore/QArgument>
#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QTextEdit>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void on_file_chooser_dst_clicked();
    void on_file_chooser_src_clicked();
    void on_button_go_clicked();
    void on_button_clear_clicked();
    QList<QList<QVariant>> get_duplicates_from_hash(QString hash);

    void sqlite_init();
    void sqlite_term();
    void sqlite_create_database();
    void sqlite_fill_database(QSqlQuery &query);
    void sqlite_check_query(QSqlQuery &query);

    static void run_init_thread(MainWindow *w);

    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    /*
     * Widgets
     */

    const QPushButton *button_go;
    const QPushButton *button_clear;
    const QCheckBox *option_standalone;
    const QCheckBox *option_copy;
    const QComboBox *combobox_game;
    QTextEdit *text_edit_right;
    const QTextEdit *text_edit_left;
    const QPushButton *file_chooser_dst;
    const QPushButton *file_chooser_src;
    QSqlDatabase database;
    const QString file_database = "database.db";

    QString path_dest = QDir::currentPath();
    QFileInfoList file_paths;
    QRegularExpression regex_hash = QRegularExpression("(0x[a-fA-F0-9]{8})");

    std::thread thread_init;

    ~MainWindow() override;
    Ui::MainWindow *ui;

};

#endif
