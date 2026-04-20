#include "passform.h"
#include "pupil.h"
#include "amp.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QDir>
#include <QDebug>

PassForm::PassForm(Person* p, int index, QWidget *parent)
    : QWidget(parent), person(p), rowIndex(index)
{
    setWindowTitle("Пропуск");
    resize(350, 200);
    setWindowFlags(Qt::Window);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // РАМКА
    QFrame *frame = new QFrame(this);
    frame->setFrameShape(QFrame::Box);
    frame->setLineWidth(2);

    QHBoxLayout *frameLayout = new QHBoxLayout(frame);

    QLabel *photo = new QLabel(this);
    photo->setFixedSize(80, 80);
    photo->setFrameShape(QFrame::Box);

    QPixmap pix("photo.png");

    if (pix.isNull()) {
        qDebug() << "Не загружается из файловой системы!";
    } else {
        photo->setPixmap(pix.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    // ТЕКСТ
    QVBoxLayout *textLayout = new QVBoxLayout();

    QLabel *line1 = new QLabel(this);
    QLabel *line2 = new QLabel(this);
    QLabel *line3 = new QLabel(this);
    QLabel *dateLabel = new QLabel(this);

    Pupil *pu = dynamic_cast<Pupil*>(person);
    AmP *am = dynamic_cast<AmP*>(person);

    if (pu) {
        line1->setText(pu->getLastName());
        line2->setText(pu->getFirstName());

        QString middle = pu->getMiddleName();
        if (middle == "-" || middle.isEmpty()) {
            line3->setText("");
            line3->hide();
        } else {
            line3->setText(middle);
        }

        dateLabel->setText(pu->getFormattedDate());
    }
    else if (am) {
        line1->setText(am->getFirstName());

        QString second = am->getSecondName();
        if (second == "-" || second.isEmpty()) {
            line2->setText("");
            line2->hide();
        } else {
            line2->setText(second);
        }

        line3->setText(am->getLastName());
        dateLabel->setText(am->getFormattedDate());
    }

    textLayout->addWidget(line1);
    textLayout->addWidget(line2);
    textLayout->addWidget(line3);
    textLayout->addWidget(dateLabel);

    frameLayout->addWidget(photo);
    frameLayout->addLayout(textLayout);

    // КНОПКИ
    QPushButton *printBtn = new QPushButton("Печать");
    QPushButton *cancelBtn = new QPushButton("Отмена");

    printBtn->setStyleSheet("background-color: blue; color: white;");
    cancelBtn->setStyleSheet("background-color: red; color: white;");

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(printBtn);
    btnLayout->addWidget(cancelBtn);

    mainLayout->addWidget(frame);
    mainLayout->addLayout(btnLayout);

    connect(printBtn, &QPushButton::clicked, this, &PassForm::printClicked);
    connect(cancelBtn, &QPushButton::clicked, this, &PassForm::cancelClicked);
}

void PassForm::printClicked()
{
    emit personPrinted(rowIndex);
    close();
}

void PassForm::cancelClicked()
{
    close();
}
