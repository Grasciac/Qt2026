#ifndef AMP_H
#define AMP_H

#include "person.h"

class AmP : public Person
{
    QString secondName;

public:
    AmP(QString f, QString s, QString l, int d, int m, int y);

    QString craft() override;
    QString getSecondName() const;
    QString getFormattedDate() const override;
};

#endif
