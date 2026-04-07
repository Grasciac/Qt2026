#ifndef PERSON_H
#define PERSON_H

#include <QString>

class Person
{
protected:
    QString lastName;
    QString firstName;
    int day, month, year;

public:
    Person(QString l, QString f, int d, int m, int y);
    virtual ~Person() {}

    virtual QString craft() = 0;

    // 🔥 НОВОЕ
    virtual QString getFormattedDate() const = 0;

    QString getDate() const;
    QString getLastName() const;
    QString getFirstName() const;
};

#endif
