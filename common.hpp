#ifndef TEXTUREMAPPERCQT_COMMON_HPP
#define TEXTUREMAPPERCQT_COMMON_HPP

#include <QtCore/QFile>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QTextEdit>

QFile *file_log;
QScopedPointer<QFile> buffer_log;

QString hash_from_int(QVariant number) {
    return QString("%1").arg(number.toUInt(), 8, 16, QChar('0')).toUpper().prepend("0x");
}


QString int_to_grade(const QVariant &g) {
    switch (g.toInt()) {
        case -1: return "<font color=\"yellow\">u</font>";
        case 0: return "<font>a</font>";
        case 1: return "<font color=\"orange\">b</font>";
        case 2: return "<font color=\"red\">c</font>";
        case 3: return "<font>s</font>";
    }
    return nullptr;
}

#endif
