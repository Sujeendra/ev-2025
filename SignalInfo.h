#ifndef SIGNALINFO_H
#define SIGNALINFO_H

#include <QString>

struct SignalInfo {
    double value;         // The signal value
    QString comment;      // The signal's comment
    QString description;  // The signal's description

    // Constructor for convenience
    SignalInfo(double v = 0.0, const QString& c = "", const QString& d = "")
        : value(v), comment(c), description(d) {}
};

#endif // SIGNALINFO_H
