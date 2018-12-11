#ifndef TEXTUREMAPPERCQT_COMMON_HPP
#define TEXTUREMAPPERCQT_COMMON_HPP

#include <QtCore/QFile>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QTextEdit>

#define DEBUG true // set to true for more debug

/*
 * Files
 */

QString path_bin;
QString path_log;
const QString file_database("database.db");
QFile *file_log;
QScopedPointer<QFile> buffer_log;

/*
 * SQLite
 */

QSqlDatabase database;

/*
 * Widgets
 */

QPushButton *button_go;
QCheckBox *option_rename;
QCheckBox *option_standalone;
QCheckBox *option_copy;
QComboBox *combobox_game;
QTextEdit *text_edit_right;
QTextEdit *text_edit_left;
QPushButton *file_chooser_dst;
QPushButton *file_chooser_src;

QString path_dest;

/*
 * Urls
 */

const QUrl url_version("https://raw.githubusercontent.com/CreeperLava/TextureMapper/master/version.json");
const QUrl url_vanilla("https://raw.githubusercontent.com/CreeperLava/TextureMapper/master/bin/vanilla.csv");
const QUrl url_duplicates("https://raw.githubusercontent.com/CreeperLava/TextureMapper/master/bin/duplicates.csv");

#endif