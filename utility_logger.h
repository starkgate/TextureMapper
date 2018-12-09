#ifndef TEXTUREMAPPERCQT_LOGGER_H
#define TEXTUREMAPPERCQT_LOGGER_H

#include <fstream>
#include <iostream>
#include <string.h>
#include <QtCore/QDateTime>
#include <QTextStream>
#include "main.h"
#include "common.hpp"

using namespace std;

void stream(QTextStream &s1, QTextStream &s2, const QString &msg) {
    s1 << msg;
    s2 << msg;
}

#define stream(X) (stream(s1,s2,X))

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QTextStream s1(buffer_log.data()); // log to file
    QTextStream s2((type < 2) ? stdout : stderr); // log to console

    stream(QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss.zzz]"));
    switch (type) {
        case QtInfoMsg:     stream("[INFO] "); break;
        case QtDebugMsg:    stream("[DEBG] "); break;
        case QtWarningMsg:  stream("[WARN] "); break;
        case QtCriticalMsg: stream("[CRIT] "); break;
        case QtFatalMsg:    stream("[FATL] "); break;
    }

    if(DEBUG) {
        s1 << context.function << ":" << context.line << "\t";
        s2 << context.function << ":" << context.line << "\t";
    }

    stream(msg);
    stream("\n");
    s1.flush();
    s2.flush(); // Clear the buffered data
}

#endif