#include "person.h"

Person::Person(QString l, QString f, int d, int m, int y)
    : lastName(l), firstName(f), day(d), month(m), year(y) {}

QString Person::getDate() const {
    return QString("%1.%2.%3")
    .arg(day, 2, 10, QChar('0'))
        .arg(month, 2, 10, QChar('0'))
        .arg(year);
}

QString Person::getLastName() const { return lastName; }
QString Person::getFirstName() const { return firstName; }
