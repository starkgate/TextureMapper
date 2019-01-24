#include "sqlite.hpp"
#include "main.h"
#include "ui_texturemapper.h"
#include "logger.hpp"
#include <QDir>
#include <QtWidgets/QFileDialog>
#include <QtGui/QStandardItem>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QFontDatabase>
#include <QtCore/QStringBuilder>

const QUrl url_version = QUrl("https://raw.githubusercontent.com/CreeperLava/TextureMapper/master/version.json");

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    // LOGGING
    QString path_log = "log/";
    QDir(QDir::currentPath()).mkdir(path_log);
    file_log = new QFile(path_log + QDateTime::currentDateTime().toString("yyyy-MM-dd") + "-TextureMapper.log");
    file_log->open(QFile::WriteOnly); // reset the file if it already exists

    buffer_log.reset(file_log);
    buffer_log.data()->open(QFile::Append | QFile::Text | QFile::WriteOnly);
    qInstallMessageHandler(MainWindow::messageHandler);

    // THREAD INIT
    thread_init = std::thread(run_init_thread, this); // separate thread for database creation / update

    // UI
    ui->setupUi(this);

    option_copy = this->findChild<QCheckBox *>("option_copy");
    option_names = this->findChild<QCheckBox *>("option_names");

    combobox_game = this->findChild<QComboBox *>("combobox_game");
    text_edit_left = this->findChild<QTextEdit *>("text_edit_left");
    text_edit_right = this->findChild<QTextEdit *>("text_edit_right");
    text_edit_right->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    line_edit_search = this->findChild<QLineEdit *>("line_edit_search");

    button_go = this->findChild<QPushButton *>("button_go");
    button_clear = this->findChild<QPushButton *>("button_clear");
    file_chooser_dst = this->findChild<QPushButton *>("file_chooser_dst");
    file_chooser_src = this->findChild<QPushButton *>("file_chooser_src");

    QObject::connect(button_go, &QPushButton::clicked, this, &MainWindow::on_button_go_clicked);
    QObject::connect(button_clear, &QPushButton::clicked, this, &MainWindow::on_button_clear_clicked);
    QObject::connect(file_chooser_dst, &QPushButton::clicked, this, &MainWindow::on_file_chooser_dst_clicked);
    QObject::connect(file_chooser_src, &QPushButton::clicked, this, &MainWindow::on_file_chooser_src_clicked);
    QObject::connect(line_edit_search, &QLineEdit::textEdited, this, &MainWindow::on_line_edit_text_edited);
}

MainWindow::~MainWindow() {
    sqlite_term();
    file_log->close();
    if(thread_copy.joinable()) {
        qDebug("Waiting for copy thread to be done...");
        thread_copy.join();
    }
    delete ui;
}
void MainWindow::on_line_edit_text_edited(const QString &str) {
    if(tmp_filter.isEmpty()) { // backup the old content of text edit right
        tmp_filter = ui->text_edit_right->toPlainText().split("\n");
    }

    if(!str.isEmpty()) { // if line edit is empty, we reset right pane
        QStringList res;
        for(QString s : tmp_filter)
            if (s.contains(str, Qt::CaseInsensitive))
                res += s;
        ui->text_edit_right->setText(res.join("\n"));
    } else {
        ui->text_edit_right->setText(tmp_filter.join("\n"));
    }
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
        tmp_filter.clear();
        ui->text_edit_left->setText(res);
        ui->text_edit_left->setReadOnly(true);
        ui->option_copy->setEnabled(true);
        ui->file_chooser_dst->setEnabled(true);

        qDebug("Selected %d files, text edit is now read-only", file_paths.size());
    } else {
        qWarning("No files selected");
    }
}

QList<QList<QVariant>> MainWindow::get_duplicates_from_hash(const QString hash) {
    QString groupid;
    QList<QList<QVariant>> matches; // { crc, name, grade, notes }
    QString selected_game(QString::number(ui->combobox_game->currentIndex() + 1));
    QSqlQuery query(database);

    query.exec(query_standalone.arg(hash, selected_game));
    if (query.next()) { // found standalone match
        matches.append({hash_from_int(query.value(0)), query.value(1), 3, "" });
    } else {
        qWarning("Texture not found in vanilla database, skipping...");
        return matches;
    }

    query.exec(query_groupid.arg(hash)); // get groupid from hash
    if (query.next()) { // iterate through the results
        groupid = query.value(0).toString();
        query.exec(query_duplicates.arg(groupid, selected_game, hash)); // get duplicates from groupid
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

void MainWindow::generate_file_paths() {
    // if we're in string mode, generate "fake" QFileInfos to fill file_paths
    file_paths.clear();
    for(QString s : ui->text_edit_left->toPlainText().split("\n", QString::SkipEmptyParts)) {
        file_paths.append(QFileInfo(s));
    }
}

QList<QString> MainWindow::get_hashes_from_name(const QString name) {
    QSqlQuery query(database);
    QList<QString> matches;

    query.exec(query_name.arg(name)); // get all hashes that match the name
    while (query.next()) { // for each match, get all reusable textures
        matches.append(query.value(0).toString());
    }
    return matches;
}

void MainWindow::on_button_go_clicked() {
    bool file_mode = ui->text_edit_left->isReadOnly();
    bool copy_mode = ui->option_copy->isChecked();
    bool names_mode = ui->option_names->isChecked();

    // CHECKS
    if(!file_mode) generate_file_paths();
    if(file_paths.isEmpty()) return;

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
        QList<QString> hashes;
        if (match.hasMatch()) {
            hashes.append(match.captured(0));
            qDebug("Found matching hash");
            names_mode = false;
        } else if(names_mode) {
            for(QString hash : get_hashes_from_name(entry.fileName())) {
                hashes.append(hash_from_int(hash));
            }
            qDebug("Found %d matching hashes", hashes.size());
        }

        QList<QList<QVariant>> search_results;
        for(QString hash : hashes) {
            search_results = get_duplicates_from_hash(hash);

            if(!search_results.isEmpty()) {
                ts << hash << (names_mode ? " <font color=\"orange\">(from name)</font><br>" : "<br>");
                for (QList<QVariant> search : search_results) {
                    QString crc = search[0].toString();
                    QString name = search[1].toString();
                    QString grade = int_to_grade(search[2]);
                    QString notes = search[3].toString();

                    ts << (QString("%1 (%2) %3, %4<br>")).arg(crc, grade, name, notes);

                    // copy files if copy is checked and we're in file mode
                    if (copy_mode && file_mode) {
                        QString file_name_dest = QDir(path_dest).filePath(QString("ME%1/%2_%3.%4")).arg(game, name, crc, entry.suffix());
                        copy_queue.enqueue(QPair(entry.absoluteFilePath(),QDir(path_dest).filePath(file_name_dest)));
                    }
                }
                ts << "<br>";
            }
        }
    }

    if(copy_mode) {
        thread_copy = std::thread(run_copy_thread, copy_queue);
    }

    ui->text_edit_right->setText(result);
}

void MainWindow::on_button_clear_clicked() {
    qDebug("Clearing data");
    ui->text_edit_right->clear();
    tmp_filter.clear();
    ui->text_edit_left->clear();
    ui->text_edit_left->setReadOnly(false);
    ui->option_copy->setEnabled(false);
    ui->file_chooser_dst->setEnabled(false);
    file_paths.clear();
}

void MainWindow::run_copy_thread(QQueue<QPair<QString, QString>> files) {
    QPair<QString, QString> file;
    while(!files.isEmpty()) {
        file = files.dequeue();
        qDebug("Copying %s to %s", file.first.toStdString().c_str(), file.second.toStdString().c_str());

        if(QFile(file.second).exists() && QFile(file.second).size() < QFile(file.first).size()) {
            qDebug("Destination already exists. Overwriting with larger file.");
            QFile::remove(file.second);
            QFile::copy(file.first, file.second);
        } else if(!QFile(file.second).exists()) {
            QFile::copy(file.first, file.second);
        } else {
            qDebug("Abort copy.");
        }
    }
}

void MainWindow::run_init_thread(MainWindow *w) {
    qDebug("Started initialization thread...");
    QNetworkAccessManager manager;
    QNetworkReply *response = manager.get(QNetworkRequest(url_version));

    QEventLoop event;
    QObject::connect(response, SIGNAL(finished()), &event, SLOT(quit()));
    event.exec();

    QFile version_file("version.json");
    QFile::remove("TextureMapper.exe~");

    if (response->error() != QNetworkReply::NetworkError::NoError) {
        qCritical("%d error encountered while downloading the version file",
                  response->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
    } else if (version_file.open(QFile::ReadWrite)) {
        QJsonObject local_json = QJsonDocument::fromJson(version_file.readAll()).object();
        QJsonObject remote_json = QJsonDocument::fromJson(response->readAll()).object();

        int local_db_version = local_json.value("database-version").toInt();
        int local_main_version = local_json.value("main-version").toInt();
        int remote_db_version = remote_json.value("database-version").toInt();
        int remote_main_version = remote_json.value("main-version").toInt();

        qDebug("Currently running : TextureMapper v%d, database v%d", local_main_version, local_db_version);
        if (local_db_version < remote_db_version || local_main_version < remote_main_version) {
            qDebug("Latest version : TextureMapper v%d, database v%d", remote_main_version, remote_db_version);

            // auto update database
            if (local_db_version < remote_db_version) {
                qCritical("Database is outdated, removing the old file for update...");

                QFile f = QFile("database.db");
                if (f.exists()) {
                    f.close();
                    f.remove();
                }

                local_db_version = remote_db_version;
            }

            // auto update executable
            if (local_main_version < remote_main_version) {
                qCritical("Texture Mapper is outdated, updating from %s...", url_executable.arg(remote_db_version).toStdString().c_str());

                QNetworkAccessManager manager;
                QNetworkRequest req(QUrl(url_executable.arg(remote_db_version)));
                req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
                QNetworkReply *response_exec = manager.get(req);

                QEventLoop event_exec;
                QObject::connect(response_exec, SIGNAL(finished()), &event_exec, SLOT(quit()));
                event_exec.exec();
                qDebug() << response_exec->size();

                if(response_exec->error() != QNetworkReply::NetworkError::NoError) {
                    qCritical("%d error encountered while downloading executable, abort update",
                              response_exec->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
                } else {
                    QFile::rename("TextureMapper.exe", "TextureMapper.exe~"); // rename current executable
                    QFile updateVersion("TextureMapper.exe");
                    if (!updateVersion.open(QIODevice::WriteOnly)) {
                        qCritical("Can't open executable update file");
                    } else {
                        updateVersion.write(response_exec->readAll());
                        updateVersion.close();
                        qDebug("Updated executable to version %d, restart for it to take effect...", remote_main_version);
                    }

                    local_main_version = remote_main_version;
                }
            }
        } else {
            qDebug("We are running the latest available version of the tools");
        }

        QJsonObject new_json;
        new_json.insert("database-version", local_db_version);
        new_json.insert("main-version", local_main_version);

        version_file.close();
        version_file.open(QFile::WriteOnly | QFile::Truncate);
        version_file.write(QJsonDocument(new_json).toJson());
    } else {
        qCritical("Couldn't check version of the tool");
    }

    version_file.close();
    w->sqlite_init();

    qDebug("Initialized Texture Mapper successfully !");
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow *w = new MainWindow();
    w->show();

    return app.exec();
}
