#ifndef TEXTUREMAPPERCQT_COMMON_HPP
#define TEXTUREMAPPERCQT_COMMON_HPP

#include <QtCore/QFile>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QTextEdit>

#define DEBUG false // set to true for more debug

QString toGrade(QVariant g) {
    switch (g.toInt()) {
        case -1: return "<font>s</font>";
        case 0: return "<font>a</font>";
        case 1: return "<font color=\"yellow\">b</font>";
        case 2: return "<font color=\"red\">c</font>";
    }
}

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

MainWindow *w;
QPushButton *button_go;
QPushButton *button_clear;
QCheckBox *option_standalone;
QCheckBox *option_copy;
QComboBox *combobox_game;
QTextEdit *text_edit_right;
QTextEdit *text_edit_left;
QPushButton *file_chooser_dst;
QPushButton *file_chooser_src;

QString path_dest(QDir::currentPath());
QFileInfoList file_paths;
QRegularExpression regex_hash("(0x[a-fA-F0-9]{8})");

/*
 * Urls
 */

const QUrl url_version("https://raw.githubusercontent.com/CreeperLava/TextureMapper/master/version.json");
const QUrl url_vanilla("https://raw.githubusercontent.com/CreeperLava/TextureMapper/master/bin/vanilla.csv");
const QUrl url_duplicates("https://raw.githubusercontent.com/CreeperLava/TextureMapper/master/bin/duplicates.csv");

#endif