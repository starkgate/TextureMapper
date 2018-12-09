#ifndef TEXTUREMAPPERCQT_UTILITY_SQLITE_H
#define TEXTUREMAPPERCQT_UTILITY_SQLITE_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtCore/QDir>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include "utility_logger.h"
#include "common.hpp"

const QString query_create_database_duplicates(
        "create table duplicates ("
        "groupid int,"
        "game int,"
        "crc varchar(8),"
        "name varchar(70),"
        "size_x int,"
        "size_y int,"
        "grade char,"
        "format varchar(10),"
        "PRIMARY KEY(game, crc))");

const QString query_index1_database_duplicates(
        "create index index_crc on duplicates (crc)");

const QString query_index2_database_duplicates(
        "create index index_groupid_game on duplicates (groupid, game)");

const QString query_create_database_vanilla(
        "create table vanilla ("
        "game int,"
        "crc varchar(8),"
        "name varchar(70),"
        "PRIMARY KEY(game, crc));");

const QString query_index_database_vanilla(
        "create index index_crc on vanilla (crc)");

const QString query_vacuum("vacuum");

const QUrl url_vanilla("https://raw.githubusercontent.com/CreeperLava/TextureMapper/master/bin/vanilla.csv");
const QUrl url_duplicates("https://raw.githubusercontent.com/CreeperLava/TextureMapper/master/bin/duplicates.csv");

/*
 * SQLite3 specific utility functions
 */

// TODO check if threadable
void sqlite_fill_database_duplicates(QSqlQuery &query) {
    QNetworkAccessManager manager;
    QNetworkReply *response = manager.get(QNetworkRequest(url_duplicates));
    QEventLoop event;
    QObject::connect(response, SIGNAL(finished()), &event, SLOT(quit()));
    event.exec();
    if(response->error() != QNetworkReply::NetworkError::NoError) {
        qCritical("%d error encountered while downloading duplicate textures csv",
                response->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
    } else {
        qDebug("Downloaded vanilla textures csv");
    }

    query.exec(".mode csv");
    query.exec(".import " + response->readAll() + " duplicates");
    qDebug("Filled duplicate textures database");
}

void sqlite_create_database_duplicates() {
    database_duplicates = QSqlDatabase::addDatabase("QSQLITE", file_database_duplicates);

    QSqlQuery query(database_duplicates);
    query.exec(query_create_database_duplicates);
    query.exec(query_index1_database_duplicates);
    query.exec(query_index2_database_duplicates);
    sqlite_fill_database_duplicates(query);
    query.exec(query_vacuum);
}

void sqlite_fill_database_vanilla(QSqlQuery &query) {
    QNetworkAccessManager manager;
    QNetworkReply *response = manager.get(QNetworkRequest(url_vanilla));
    QEventLoop event;
    QObject::connect(response, SIGNAL(finished()), &event, SLOT(quit()));
    event.exec();
    if(response->error() != QNetworkReply::NetworkError::NoError) {
        qCritical("%d error encountered while downloading vanilla textures csv",
                  response->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
    } else {
        qDebug("Downloaded vanilla textures csv");
    }

    query.exec(".mode csv");
    query.exec(".import " + response->readAll() + " vanilla");
    qDebug("Filled vanilla textures database");
}

void sqlite_create_database_vanilla() {
    database_vanilla = QSqlDatabase::addDatabase("QSQLITE", file_database_vanilla);

    QSqlQuery query(database_vanilla);
    query.exec(query_create_database_vanilla);
    query.exec(query_index_database_vanilla);
    sqlite_fill_database_vanilla(query);
    query.exec(query_vacuum);
}

void sqlite_open(QSqlDatabase &db, const QString &path) {
    qDebug("%s %s...", "Opening database at",
            QDir::toNativeSeparators(QDir(path).absolutePath()).toStdString().c_str());
    db = QSqlDatabase::addDatabase("QSQLITE", path);
    db.setDatabaseName(path);
    db.open();
    qDebug("Opened database successfully.");
}

void sqlite_close(QSqlDatabase &db) {
    qDebug("%s %s...", "Closing database", db.connectionName().toStdString().c_str());
    db.close();
    qDebug("Closed database successfully.");
}

void sqlite_init() {
    if(QDir(file_database_duplicates).exists()) {
        sqlite_open(database_duplicates, file_database_duplicates);
    } else {
        qWarning("Duplicate database does not exist, creating it...");
        sqlite_create_database_duplicates();
    }

    if(QDir(file_database_vanilla).exists()) {
        sqlite_open(database_vanilla, file_database_vanilla);
    } else {
        qWarning("Vanilla database does not exist, creating it...");
        sqlite_create_database_vanilla();
    }

    qDebug("Initialized SQLite successfully...");
}

void sqlite_term() {
    sqlite_close(database_duplicates);
    sqlite_close(database_vanilla);
    qDebug("Terminated SQLite successfully...");
}

void sqlite_update() {
    /*
    def update_db
      # 2x faster than CSV.parse and much more efficient for memory (doesn't pull the whole file into RAM)
      @dupes_db.transaction
      CSV.foreach(@texture_map, headers: true) do |row|
        unless @dupes_db.execute("select * from textures where groupid=#{row[0]} and game=#{row[1]} and crc=#{row[2]} limit 1").empty?
          @dupes_db.execute('insert into textures values ( ?, ?, ?, ?, ?, ?, ?, ? )', row[0..7])
        end # unless they're already present
      end
      @dupes_db.commit
      @dupes_db.execute('vacuum')
    end
    */
}

#endif //TEXTUREMAPPERCQT_UTILITY_SQLITE_H
