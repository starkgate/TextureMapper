#include "main.h"
#include "utility_csv.h"
#include "utility_sqlite.h"
#include "ui_mainwindow.h"
#include "utility_logger.h"
#include <QDir>

void initialize() {
    sqlite_init();
}

void finish() {
    sqlite_term();
}

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    finish();
    delete ui;
}

void MyThread::run() {
    qDebug("Started initialization thread...");
    initialize();
    qDebug("Initialized Texture Mapper successfully !");
    // TODO progress bar if GUI starts before DB and DB isn't ready ?
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // TODO extract to init paths
    path_bin = "bin/";
    file_database_duplicates = path_bin + "database_duplicates.db";
    file_database_vanilla = path_bin + "database_vanilla.db";

    // TODO extract to init log and put init log in threaded init
    path_log = "log/";
    QDir(QDir::currentPath()).mkdir(path_log);
    file_log = new QFile(path_log + QDateTime::currentDateTime().toString("yyyy-MM-dd") + "-TextureMapper.log");

    buffer_log.reset(file_log);
    buffer_log.data()->open(QFile::Append | QFile::Text | QFile::WriteOnly);
    qInstallMessageHandler(messageHandler);

    MyThread thread_init;
    thread_init.start(); // separate thread for database creation / update

    MainWindow w;
    w.show();

    return app.exec();
}