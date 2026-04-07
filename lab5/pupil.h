#ifndef PUPIL_H
#define PUPIL_H

#include "person.h"

class Pupil : public Person
{
    QString middleName;


public:
    Pupil(QString l, QString f, QString m, int d, int mo, int y);

    QString craft() override;
    QString getMiddleName() const;
    QString getFormattedDate() const override;
};

#endif
