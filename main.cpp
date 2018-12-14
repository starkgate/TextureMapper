#include "main.h"
#include "lib/sqlite.hpp"
#include "ui_mainwindow.h"
#include "lib/logger.hpp"
#include <QDir>
#include <QtWidgets/QFileDialog>
#include <QtGui/QStandardItem>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QFontDatabase>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    option_copy = this->findChild<QCheckBox *>("option_copy");
    option_standalone = this->findChild<QCheckBox *>("option_standalone");

    combobox_game = this->findChild<QComboBox *>("combobox_game");
    text_edit_left = this->findChild<QTextEdit *>("text_edit_left");
    text_edit_right = this->findChild<QTextEdit *>("text_edit_right");
    text_edit_right->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    button_go = this->findChild<QPushButton *>("button_go");
    button_clear = this->findChild<QPushButton *>("button_clear");
    file_chooser_dst = this->findChild<QPushButton *>("file_chooser_dst");
    file_chooser_src = this->findChild<QPushButton *>("file_chooser_src");

    QObject::connect(button_go, &QPushButton::clicked, this, &MainWindow::on_button_go_clicked);
    QObject::connect(button_clear, &QPushButton::clicked, this, &MainWindow::on_button_clear_clicked);
    QObject::connect(file_chooser_dst, &QPushButton::clicked, this, &MainWindow::on_file_chooser_dst_clicked);
    QObject::connect(file_chooser_src, &QPushButton::clicked, this, &MainWindow::on_file_chooser_src_clicked);
}

MainWindow::~MainWindow() {
    sqlite_term();
    delete ui;
}

void MainWindow::on_file_chooser_dst_clicked() {
    qDebug("Selecting destination folder...");
    path_dest = QFileDialog::getExistingDirectory(
            this, "Select Destination Folder", path_dest, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    qDebug("Destination folder set to %s", path_dest.toStdString().c_str());
}

void MainWindow::on_file_chooser_src_clicked() {
    qDebug("Selecting files...");
    QStringList files = QFileDialog::getOpenFileNames(
            this, "Select Files", QDir::currentPath(), "Image Files (*.png *.jpg *.bmp *.dds *.tga)");

    if (!files.empty()) {
        file_paths.clear();
        for (QString file : files) file_paths << QFileInfo(file);
        path_dest = file_paths.value(0).absolutePath();

        QString res;
        QTextStream ts(&res);
        for (const QFileInfo &file : file_paths) {
            ts << file.fileName() << endl;
        }

        ui->text_edit_right->clear();
        ui->text_edit_left->setText(res);
        ui->text_edit_left->setReadOnly(true);
        ui->option_copy->setEnabled(true);
        ui->option_standalone->setEnabled(true);
        ui->file_chooser_dst->setEnabled(true);

        qDebug("Selected %d files, text edit is now read-only", file_paths.size());
    } else {
        qWarning("No files selected");
    }
}

QString hash_from_int(QVariant number) {
    return QString("%1").arg(number.toUInt(), 8, 16, QChar('0')).toUpper().prepend("0x");
}

QList<QList<QVariant>> MainWindow::get_duplicates_from_hash(const QString hash) {
    QString groupid;
    QList<QList<QVariant>> matches; // { crc, name, grade, notes }
    QString selected_game(QString::number(w->ui->combobox_game->currentIndex() + 1));
    QSqlQuery query(database);

    if (w->ui->option_standalone->isChecked()) { // if we are in standalone mode
        query.exec(query_standalone.arg(hash, selected_game));

        if (query.next()) { // found standalone match
            matches.append({hash_from_int(query.value(0)), query.value(1), 3, "" });
        } else {
            qWarning("Texture not found in vanilla database, skipping...");
            return matches;
        }
    }

    query.exec(query_groupid.arg(hash)); // get groupid from hash
    if (query.next()) { // iterate through the results
        groupid = query.value(0).toString();
        query.exec(query_duplicates.arg(groupid, selected_game)); // get duplicates from groupid
        while (query.next()) { // found duplicate match(es)
            QString crc = hash_from_int(query.value(0));
            if(crc != hash)
                matches.append({crc, query.value(1), query.value(2), query.value(3)});
        }
    } else { // if we find no match
        qDebug("No groupid matches %s", hash.toStdString().c_str());
    }

    return matches;
}

void MainWindow::on_button_go_clicked() {
    bool file_mode = ui->text_edit_left->isReadOnly();
    bool copy_mode = ui->option_copy->isChecked();

    if(!file_mode) { // if we're in string mode, fill the paths with "fake" QFileInfos
        file_paths.clear();
        for(QString s : ui->text_edit_left->toPlainText().split("\n", QString::SkipEmptyParts)) {
            file_paths.append(QFileInfo(s));
        }
    }

    if(file_paths.isEmpty()) {
        return;
    }

    QString result;
    QTextStream ts(&result, QIODevice::WriteOnly);
    QString game(QString::number(combobox_game->currentIndex() + 1));

    if(copy_mode) {
        QDir().mkpath((QDir(path_dest).filePath(QString("ME%1/")).arg(game)));
        qDebug("Creating path %s", (QDir(path_dest).filePath(QString("ME%1/")).arg(game)).toStdString().c_str());
    }

    qDebug("Checking duplicates...");
    for (const QFileInfo &entry : file_paths) { // iterate over the selected files
        QRegularExpressionMatch match = regex_hash.match(entry.fileName());

        // check presence of hash in filename
        if (match.hasMatch()) {
            QString hash = match.captured(0);

            // get duplicates
            QList<QList<QVariant>> search_results = get_duplicates_from_hash(hash);
            for (QList<QVariant> search : search_results) {

                QString crc = search[0].toString();
                QString name = search[1].toString();
                QString grade = toGrade(search[2]);
                QString notes = search[3].toString();

                ts << (QString("%1 -> %2 (%3) %4, %5<br>")).arg(hash, crc, grade, name, notes);

                // copy files if copy is checked and we're in file mode
                if (copy_mode && file_mode) {
                    QString file_name_dest = QDir(path_dest).filePath(QString("ME%1/%2_%3.%4")).arg(game, name, crc, entry.suffix());
                    QFile::copy(entry.absoluteFilePath(), QDir(path_dest).filePath(file_name_dest));
                    qDebug("Copying %s to %s", entry.absoluteFilePath().toStdString().c_str(), file_name_dest.toStdString().c_str());
                }
            }
            if(!search_results.isEmpty())
                ts << "<br>";
        }
    }

    ui->text_edit_right->setText(result);
}

void MainWindow::on_button_clear_clicked() {
    qDebug("Clearing data");
    ui->text_edit_right->clear();
    ui->text_edit_left->clear();
    ui->text_edit_left->setReadOnly(false);
    ui->option_copy->setEnabled(false);
    ui->option_standalone->setEnabled(false);
    ui->file_chooser_dst->setEnabled(false);
    file_paths.clear();
}

void MyThread::run() {
    qDebug("Started initialization thread...");
    QNetworkAccessManager manager;
    QNetworkReply *response = manager.get(QNetworkRequest(url_version));

    QEventLoop event;
    QObject::connect(response, SIGNAL(finished()), &event, SLOT(quit()));
    event.exec();

    QFile version_file("version.json");

    if (response->error() != QNetworkReply::NetworkError::NoError) {
        qCritical("%d error encountered while downloading the version file",
                  response->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
    } else if (version_file.open(QIODevice::ReadWrite)) {
        auto local_json = QJsonDocument::fromJson(version_file.readAll().toStdString().c_str()).object();
        auto remote_json = QJsonDocument::fromJson(response->readAll().toStdString().c_str()).object();

        auto local_db_version = local_json["database-version"].toInt();
        auto local_main_version = local_json["main-version"].toInt();
        auto remote_db_version = remote_json["database-version"].toInt();
        auto remote_main_version = remote_json["main-version"].toInt();

        qDebug("Currently running : TextureMapper v%d, database v%d", local_main_version, local_db_version);
        if (local_db_version < remote_db_version || local_main_version < remote_main_version) {
            qDebug("Latest version : TextureMapper v%d, database v%d", remote_main_version, remote_db_version);

            if (local_db_version < remote_db_version) {
                qCritical("Database is outdated, removing the old file for update...");

                QFile f(file_database);
                if (f.exists()) {
                    f.close();
                    f.remove();
                }

                local_json["database-version"] = remote_db_version;
                local_json["main-version"] = local_main_version;
                version_file.write(QJsonDocument(local_json).toJson());
            }

            if (local_main_version < remote_main_version) {
                qCritical("Texture Mapper is outdated, consider updating manually...");
                // TODO autoupdate
            }
        } else {
            qDebug("We are running the latest available version of the tools");
        }
    } else {
        qCritical("Couldn't check version of the tool");
    }

    version_file.close();
    sqlite_init();

    qDebug("Initialized Texture Mapper successfully !");
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    path_log = "log/";
    QDir(QDir::currentPath()).mkdir(path_log);
    file_log = new QFile(path_log + QDateTime::currentDateTime().toString("yyyy-MM-dd") + "-TextureMapper.log");
    file_log->open(QFile::WriteOnly); // reset the file if it already exists

    buffer_log.reset(file_log);
    buffer_log.data()->open(QFile::Append | QFile::Text | QFile::WriteOnly);
    qInstallMessageHandler(messageHandler);

    MyThread thread_init;
    thread_init.start(); // separate thread for database creation / update

    w = new MainWindow();
    w->show();

    return app.exec();
}