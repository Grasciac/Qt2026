#include "pupil.h"

Pupil::Pupil(QString l, QString f, QString m, int d, int mo, int y)
    : Person(l, f, d, mo, y), middleName(m) {}

QString Pupil::craft() {
    return lastName + " " + firstName + " " + middleName;
}

QString Pupil::getMiddleName() const {
    return middleName;
}

QString Pupil::getFormattedDate() const
{
    return QString("%1.%2.%3")
    .arg(day, 2, 10, QChar('0'))
        .arg(month, 2, 10, QChar('0'))
        .arg(year);
}
