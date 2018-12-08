#ifndef TEXTUREMAPPERCQT_UTILITY_SQLITE_H
#define TEXTUREMAPPERCQT_UTILITY_SQLITE_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include "utility_logger.h"

QSqlDatabase *database_alot;
QSqlDatabase *database_vanilla;
const char *file_database_alot = "database_alot.db";
const char *file_database_vanilla = "database_vanilla.db";

/*
 * SQLite3 specific utility functions
 */

void sqlite_open(QSqlDatabase * db, const char * path) {
    *db = QSqlDatabase::addDatabase("QSQLITE", path);
    db->setDatabaseName(path);
    db->open();
    log("Opened database successfully...");
}

void sqlite_close(QSqlDatabase * db) {
    db->close();
    log("Closed database successfully...");
}

void sqlite_init() {
    sqlite_open(database_alot, file_database_alot);
    sqlite_open(database_vanilla, file_database_vanilla);
    log("Initialized SQLite successfully...");
}

void sqlite_term() {
    sqlite_close(database_alot);
    sqlite_close(database_vanilla);
    log("Terminated SQLite successfully...");
}

void sqlite_create() {
    /*
    def create_db
      @dupes_db.execute <<-SQL
        create table textures (
          groupid int,
          game int,
          crc varchar(10),
          name varchar(100),
          size_x int,
          size_y int,
          grade char,
          format varchar(10),
          PRIMARY KEY(groupid, game, crc)
        );
      SQL
      update_db

      # add indexes for faster searches
      @dupes_db.execute('create index index_crc on textures (crc)')
      @dupes_db.execute('create index index_groupid_game on textures (groupid, game)')
    end
    */
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
