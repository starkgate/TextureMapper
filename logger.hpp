#ifndef TEXTUREMAPPERCQT_LOGGER_H
#define TEXTUREMAPPERCQT_LOGGER_H

#include <fstream>
#include <iostream>
#include <QtCore/QDateTime>
#include <QTextStream>
#include "main.h"
#include "common.hpp"

#define DEBUG true // set to true for more debug

using namespace std;

void stream(QTextStream &s1, QTextStream &s2, const QString &msg) {
    s1 << msg;
    s2 << msg;
}

void MainWindow::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    if(!DEBUG && type < 2) // skip debug level and below messages
        return;

    QTextStream s1(buffer_log.data()); // log to file
    QTextStream s2((type < 2) ? stdout : stderr); // log to console

    stream(s1, s2, QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss.zzz]"));
    switch (type) {
        case QtInfoMsg:     stream(s1, s2, "[INFO] "); break;
        case QtDebugMsg:    stream(s1, s2, "[DEBG] "); break;
        case QtWarningMsg:  stream(s1, s2, "[WARN] "); break;
        case QtCriticalMsg: stream(s1, s2, "[CRIT] "); break;
        case QtFatalMsg:    stream(s1, s2, "[FATL] "); break;
    }

    if(DEBUG) {
        s1 << context.function << ":" << context.line << "\t";
        s2 << context.function << ":" << context.line << "\t";
    }

    stream(s1, s2, msg);
    stream(s1, s2, "\n");
    s1.flush();
    s2.flush(); // Clear the buffered data
}

#endif
