#ifndef PERSON_H
#define PERSON_H

#include <QString>

struct Person
{
protected:
    QString lastName;
    QString firstName;
    int day, month, year;

public:
    int getDay() const { return day; }
    int getMonth() const { return month; }
    int getYear() const { return year; }
    Person(QString l, QString f, int d, int m, int y);
    virtual ~Person() {}

    virtual QString craft() = 0;

    virtual QString getFormattedDate() const = 0;

    QString getDate() const;
    QString getLastName() const;
    QString getFirstName() const;
};

#endif
