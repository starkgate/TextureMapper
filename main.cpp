#include "main.h"
#include "utility_csv.h"
#include "utility_sqlite.h"
#include "ui_mainwindow.h"
#include "utility_logger.h"
#include <QDir>
#include <QtWidgets/QFileDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    option_copy = this->findChild<QCheckBox*>("option_copy");
    option_rename = this->findChild<QCheckBox*>("option_rename");
    option_standalone = this->findChild<QCheckBox*>("option_standalone");

    combobox_game = this->findChild<QComboBox*>("combobox_game");
    text_edit_left = this->findChild<QTextEdit*>("text_edit_left");
    text_edit_right = this->findChild<QTextEdit*>("text_edit_right");

    button_go = this->findChild<QPushButton*>("button_go");
    file_chooser_dst = this->findChild<QPushButton*>("file_chooser_dst");
    file_chooser_src = this->findChild<QPushButton*>("file_chooser_src");

    QObject::connect(button_go, &QPushButton::clicked, this, &MainWindow::on_button_go_clicked);
    QObject::connect(file_chooser_dst, &QPushButton::clicked, this, &MainWindow::on_file_chooser_dst_clicked);
    QObject::connect(file_chooser_src, &QPushButton::clicked, this, &MainWindow::on_file_chooser_src_clicked);
}

MainWindow::~MainWindow() {
    sqlite_term();
    delete ui;
}

void MainWindow::on_file_chooser_dst_clicked() {
    // TODO set default output to same directory
    qWarning("dst click");
}

void MainWindow::on_file_chooser_src_clicked() {
    QStringList fileNames = QFileDialog::getOpenFileNames(this, "Select Files", QDir::currentPath(), "Image Files (*.png *.jpg *.bmp *.dds *.tga)");
    ui->text_edit_left->setText(fileNames.join("\n"));
    // TODO set output path
    // TODO save original paths of selected files
    // TODO format files (remove path + extension), keep only filename
}

void MainWindow::on_button_go_clicked() {
    qWarning("go click");
}

void MyThread::run() {
    qDebug("Started initialization thread...");
    sqlite_init();
    qDebug("Initialized Texture Mapper successfully !");
    // TODO progress bar if GUI starts before DB and DB isn't ready ?
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    path_bin = "bin/";
    file_database = path_bin + "database.db";

    path_log = "log/";
    QDir(QDir::currentPath()).mkdir(path_log);
    file_log = new QFile(path_log + QDateTime::currentDateTime().toString("yyyy-MM-dd") + "-TextureMapper.log");
    file_log->open(QFile::WriteOnly); // reset the file if it already exists

    buffer_log.reset(file_log);
    buffer_log.data()->open(QFile::Append | QFile::Text | QFile::WriteOnly);
    qInstallMessageHandler(messageHandler);

    MyThread thread_init;
    thread_init.start(); // separate thread for database creation / update

    MainWindow w;
    w.show();

    return app.exec();
}