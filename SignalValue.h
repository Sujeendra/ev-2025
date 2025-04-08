#ifndef SIGNALVALUE_H
#define SIGNALVALUE_H

#include <QObject>
#include <QString>

class SignalValue : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString key READ key WRITE setKey NOTIFY keyChanged)
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString comment READ comment WRITE setComment NOTIFY commentChanged)

public:
    SignalValue();

    QString key() const;
    void setKey(const QString &key);

    double value() const;
    void setValue(double value);

    QString description() const;
    void setDescription(const QString &description);

    QString comment() const;
    void setComment(const QString &comment);

signals:
    void keyChanged();
    void valueChanged();
    void descriptionChanged();
    void commentChanged();

private:
    QString m_key;
    double m_value;
    QString m_description;
    QString m_comment;
};

#endif // SIGNALVALUE_H
