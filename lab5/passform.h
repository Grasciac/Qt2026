#ifndef PASSFORM_H
#define PASSFORM_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include "person.h"

class PassForm : public QWidget
{
    Q_OBJECT

public:
    explicit PassForm(Person* p, int index, QWidget *parent = nullptr);

signals:
    void personPrinted(int index);

private:
    Person* person;
    int rowIndex;

    QLabel *nameLabel;
    QLabel *dateLabel;
    QPushButton *printBtn;
    QPushButton *cancelBtn;

private slots:
    void printClicked();
    void cancelClicked();
};

#endif
