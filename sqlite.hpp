#ifndef TEXTUREMAPPERCQT_UTILITY_SQLITE_H
#define TEXTUREMAPPERCQT_UTILITY_SQLITE_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtCore/QDir>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include "logger.hpp"
#include "common.hpp"

/*
 * SQLite
 */

const QUrl url_vanilla("https://raw.githubusercontent.com/CreeperLava/TextureMapper/master/bin/vanilla.csv");
const QUrl url_duplicates("https://raw.githubusercontent.com/CreeperLava/TextureMapper/master/bin/duplicates.csv");

const QString query_create_database_duplicates(
        "create table duplicates ("
        "groupid INTEGER,"
        "game INTEGER,"
        "crc INTEGER,"
        "name TEXT,"
        "size_x INTEGER,"
        "size_y INTEGER,"
        "grade INTEGER,"
        "format TEXT,"
        "notes TEXT,"
        "FOREIGN KEY(game, crc) REFERENCES vanilla(game, crc))");

const QString query_index1_database_duplicates(
        "create index index_crc_duplicates on duplicates (crc)");

const QString query_index2_database_duplicates(
        "create index index_groupid_game on duplicates (groupid, game)");

const QString query_create_database_vanilla(
        "create table vanilla ("
        "game INTEGER,"
        "crc INTEGER,"
        "name TEXT,"
        "PRIMARY KEY(game, crc));");

const QString query_index_database_vanilla(
        "create index index_crc_vanilla on vanilla (crc);");

const QString query_vacuum("vacuum");

const QString query_standalone ("select crc, name from vanilla where crc=%1 and game=%2 limit 1");
const QString query_groupid ("select groupid from duplicates where crc=%1 limit 1");
const QString query_duplicates ("select crc, name, grade, notes from duplicates where groupid=%1 and game=%2");

/*
 * SQLite3 specific utility functions
 */

void MainWindow::sqlite_check_query(QSqlQuery &query) {
    if(!query.isActive()) {
        qCritical("Couldn't execute query: %s", query.executedQuery().toStdString().c_str());
        qCritical("Error: %s", query.lastError().text().toStdString().c_str());
    }
}

void MainWindow::sqlite_fill_database(QSqlQuery &query) {
    QNetworkAccessManager manager;
    QNetworkReply *response_duplicates = manager.get(QNetworkRequest(url_duplicates));
    QNetworkReply *response_vanilla = manager.get(QNetworkRequest(url_vanilla));

    QEventLoop event_dup;
    QEventLoop event_van;
    QObject::connect(response_duplicates, SIGNAL(finished()), &event_dup, SLOT(quit()));
    QObject::connect(response_vanilla, SIGNAL(finished()), &event_van, SLOT(quit()));
    event_dup.exec();
    event_van.exec();

    if(response_duplicates->error() != QNetworkReply::NetworkError::NoError) {
        qCritical("%d error encountered while downloading duplicate textures csv",
                  response_duplicates->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
    }

    if(response_vanilla->error() != QNetworkReply::NetworkError::NoError) {
        qCritical("%d error encountered while downloading vanilla textures csv",
                  response_vanilla->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
    }

    QByteArray dup = response_duplicates->readAll();
    QByteArray van = response_vanilla->readAll();

    database.transaction();

    QTextStream ts (&dup);
    ts.readLine(); // skip header
    while(!ts.atEnd()) {
        query.exec("INSERT INTO duplicates VALUES(" + ts.readLine() + ")");
    }

    QTextStream ts2 (&van);
    ts2.readLine(); // skip header
    while(!ts2.atEnd()) {
        query.exec("INSERT INTO vanilla VALUES(" + ts2.readLine() + ")");
    }

    database.commit();
}

void MainWindow::sqlite_create_database() {
    QSqlQuery query(database);
    query.setForwardOnly(true);

    query.exec(query_create_database_vanilla); sqlite_check_query(query);
    query.exec(query_index_database_vanilla); sqlite_check_query(query);

    query.exec(query_create_database_duplicates); sqlite_check_query(query);
    query.exec(query_index1_database_duplicates); sqlite_check_query(query);
    query.exec(query_index2_database_duplicates); sqlite_check_query(query);

    sqlite_fill_database(query);
    query.exec(query_vacuum);
    qDebug("Filled database successfully");
}

void MainWindow::sqlite_init() {
    qDebug("%s %s...", "Opening database at",
           QDir::toNativeSeparators(QDir(file_database).absolutePath()).toStdString().c_str());

    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(file_database);
    database.open();
    qDebug("Opened database successfully.");

    if(!QFile(file_database).exists() || QFile(file_database).size() == qint64(0)) {
        qWarning("SQLite database does not exist, creating it...");
        sqlite_create_database();
    } else {
        qDebug("Found database");
    }

    qDebug("Initialized SQLite successfully...");
}

void MainWindow::sqlite_term() {
    qDebug("%s %s...", "Closing database", database.connectionName().toStdString().c_str());
    database.close();
    qDebug("Closed database successfully.");

    qDebug("Terminated SQLite successfully...");
}

#endif //TEXTUREMAPPERCQT_UTILITY_SQLITE_H
