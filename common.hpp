#ifndef TEXTUREMAPPERCQT_COMMON_HPP
#define TEXTUREMAPPERCQT_COMMON_HPP

#include <QtCore/QFile>

#define DEBUG true // set to true for more debug

/*
 * Files
 */
QString path_bin;
QString path_log;
QString file_database_duplicates;
QString file_database_vanilla;
QFile *file_log;
QScopedPointer<QFile> buffer_log;

QSqlDatabase database_duplicates;
QSqlDatabase database_vanilla;

#endif //TEXTUREMAPPERCQT_COMMON_HPP
