#include "amp.h"

AmP::AmP(QString f, QString s, QString l, int d, int m, int y)
    : Person(l, f, d, m, y), secondName(s) {}

QString AmP::craft() {
    return firstName + " " + secondName + " " + lastName;
}

QString AmP::getSecondName() const {
    return secondName;
}

QString AmP::getFormattedDate() const
{
    return QString("%1-%2-%3")
    .arg(month, 2, 10, QChar('0'))
        .arg(day, 2, 10, QChar('0'))
        .arg(year);
}
