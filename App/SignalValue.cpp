#include "SignalValue.h"

SignalValue::SignalValue() : m_value(0) {}

QString SignalValue::key() const
{
    return m_key;
}

void SignalValue::setKey(const QString &key)
{
    if (m_key != key) {
        m_key = key;
        emit keyChanged();
    }
}

double SignalValue::value() const
{
    return m_value;
}

void SignalValue::setValue(double value)
{
    if (m_value != value) {
        m_value = value;
        emit valueChanged();
    }
}

QString SignalValue::description() const
{
    return m_description;
}

void SignalValue::setDescription(const QString &description)
{
    if (m_description != description) {
        m_description = description;
        emit descriptionChanged();
    }
}

QString SignalValue::comment() const
{
    return m_comment;
}

void SignalValue::setComment(const QString &comment)
{
    if (m_comment != comment) {
        m_comment = comment;
        emit commentChanged();
    }
}
