#ifndef SIGNALVALUEMAP_H
#define SIGNALVALUEMAP_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QTimer>
#include <QThread>
#include "SignalValue.h"  // Include SignalValue class header
#include "CanWorker.h"    // Include CanWorker class header
#include "SignalInfo.h"
#include "QCanDbcFileParser"
#include "QCanMessageDescription"
#include "QCanSignalDescription"

class SignalValueMap : public QObject {
    Q_OBJECT
    Q_PROPERTY(QList<SignalValue*> signalValueList READ signalValueList NOTIFY signalValueMapChanged)

public:
    SignalValueMap();

    QList<SignalValue*> signalValueList() const;
    QMap<QString, SignalInfo> m_signalValueMap;

signals:
    void signalValueMapChanged();

public slots:
    void updateDictionaryValues();

private:
    void processFrames(const QCanBusFrame& frame);
    void populateInitialDictionary();
    void updateSignalValueList();

private:
    QList<SignalValue*> m_signalValueList;
    QTimer m_updateTimer;
    CanWorker* m_worker;
    QThread* m_workerThread;
    QCanDbcFileParser fileParser;
};

#endif // SIGNALVALUEMAP_H
