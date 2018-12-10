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
QString file_database;
QFile *file_log;
QScopedPointer<QFile> buffer_log;

QSqlDatabase database;

QPushButton *button_go;
QCheckBox *option_rename;
QCheckBox *option_standalone;
QCheckBox *option_copy;
QComboBox *combobox_game;
QTextEdit *text_edit_right;
QTextEdit *text_edit_left;
QPushButton *file_chooser_dst;
QPushButton *file_chooser_src;

#endif //TEXTUREMAPPERCQT_COMMON_HPP
