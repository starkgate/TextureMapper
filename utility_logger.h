#ifndef TEXTUREMAPPERCQT_LOGGER_H
#define TEXTUREMAPPERCQT_LOGGER_H

#include <fstream>
#include <iostream>
#include <string.h>

#define FATAL_ERROR_LOG -1

using namespace std;

const char *log_file = "debug.log";
ofstream cdbg;

const string currentDateTime() {
    time_t now = time(nullptr);
    struct tm tstruct{};
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "[%Y-%m-%d.%X] ", &tstruct);

    return buf;
}

/*
 * Logging utility
 */

void log(basic_string<char> message) {
    message = currentDateTime() + message;
    cerr << message << endl;
    cdbg << message << endl;
}

void log(const char * message) {
    log((basic_string<char>) message);
}

void log(const char * m1, const char * m2) {
    log((basic_string<char>) m1 + (basic_string<char>) m2);
}

void log_init() {
    cdbg.open(log_file);
    if(!cdbg.is_open()) {
        cerr << "Error opening debug file" << endl;
        exit(FATAL_ERROR_LOG);
    }
    log("Opened log successfully...");
}

void log_term() {
    log("Terminating log...");
    cdbg.close();
}

#endif