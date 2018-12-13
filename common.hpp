#ifndef TEXTUREMAPPERCQT_COMMON_HPP
#define TEXTUREMAPPERCQT_COMMON_HPP

#include <QtCore/QFile>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QTextEdit>

QString toGrade(const QVariant &g) {
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
const QPushButton *button_go;
const QPushButton *button_clear;
const QCheckBox *option_standalone;
const QCheckBox *option_copy;
const QComboBox *combobox_game;
QTextEdit *text_edit_right;
const QTextEdit *text_edit_left;
const QPushButton *file_chooser_dst;
const QPushButton *file_chooser_src;

static QString path_dest(QDir::currentPath());
static QFileInfoList file_paths;
static QRegularExpression regex_hash("(0x[a-fA-F0-9]{8})");

/*
 * Urls
 */

static const QUrl url_version("https://raw.githubusercontent.com/CreeperLava/TextureMapper/master/version.json");
static const QUrl url_vanilla("https://raw.githubusercontent.com/CreeperLava/TextureMapper/master/bin/vanilla.csv");
static const QUrl url_duplicates("https://raw.githubusercontent.com/CreeperLava/TextureMapper/master/bin/duplicates.csv");

#endif