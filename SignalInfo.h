#ifndef SIGNALINFO_H
#define SIGNALINFO_H

#include <QString>

struct SignalInfo {
    double value;         // The signal value
    QString comment;      // The signal's comment
    QString description;  // The signal's description
    bool IsValid; // this is required to validate if thhis key is valid or not depending on the CAN trafiic --> if there is no traffic then set to False

    // Constructor for convenience
    SignalInfo(double v = 0.0, const QString& c = "", const QString& d = "", const bool& e = false)
        : value(v), comment(c), description(d),IsValid(e) {}
};

#endif // SIGNALINFO_H
