#include "mainwindow.h"
#include "pupil.h"
#include "amp.h"
#include "passform.h"

#include <QVBoxLayout>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    setWindowTitle("Пропуска");
    resize(600, 400);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    QFrame *frame = new QFrame(this);
    frame->setFrameShape(QFrame::Box);
    frame->setLineWidth(3);

    QVBoxLayout *frameLayout = new QVBoxLayout(frame);

    table = new QTableWidget(this);
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels({"Фамилия", "Имя", "Отчество/Второе имя", "Дата рождения"});
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setWordWrap(true);
    table->resizeRowsToContents();

    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    frameLayout->addWidget(table);

    loadBtn = new QPushButton("Загрузить", this);
    loadBtn->setFixedWidth(120);
    loadBtn->setStyleSheet("background-color: lightblue;");

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(loadBtn);
    btnLayout->addStretch();

    mainLayout->addWidget(frame);
    mainLayout->addLayout(btnLayout);

    connect(loadBtn, &QPushButton::clicked, this, &MainWindow::loadClicked);
    connect(table, &QTableWidget::cellDoubleClicked,
            this, &MainWindow::cellDoubleClicked);
}

MainWindow::~MainWindow() {}

void MainWindow::loadClicked()
{
    QString file = QFileDialog::getOpenFileName(this, "Открыть файл");
    loadFromFile(file);
}

void MainWindow::loadFromFile(QString path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return;

    QTextStream in(&file);

    table->setRowCount(0);
    persons.clear();

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().isEmpty()) continue;

        QString delimiter = line.contains("|") ? "|" : " ";

        QStringList parts = line.split(delimiter, Qt::SkipEmptyParts);
        if (parts.size() < 6) continue;

        int type = parts[0].toInt();

        if (type == 1) {
            if (parts.size() == 7) {
                QString lastName = parts[1];
                QString firstName = parts[2];
                QString middleName = parts[3];
                int day = parts[4].toInt();
                int month = parts[5].toInt();
                int year = parts[6].toInt();
                persons.push_back(new Pupil(lastName, firstName, middleName, day, month, year));
            }
            else if (parts.size() == 6) {
                QString lastName = parts[1];
                QString firstName = parts[2];
                QString middleName = "";
                int day = parts[3].toInt();
                int month = parts[4].toInt();
                int year = parts[5].toInt();
                persons.push_back(new Pupil(lastName, firstName, middleName, day, month, year));
            }
        }
        else if (type == 2) {
            if (parts.size() == 7) {
                QString firstName = parts[1];
                QString secondName = parts[2];
                QString lastName = parts[3];
                int day = parts[4].toInt();
                int month = parts[5].toInt();
                int year = parts[6].toInt();
                persons.push_back(new AmP(firstName, secondName, lastName, day, month, year));
            }
            else if (parts.size() == 6) {
                QString firstName = parts[1];
                QString secondName = "";
                QString lastName = parts[2];
                int day = parts[3].toInt();
                int month = parts[4].toInt();
                int year = parts[5].toInt();
                persons.push_back(new AmP(firstName, secondName, lastName, day, month, year));
            }
        }
    }

    for (int i = 0; i < persons.size(); i++) {
        table->insertRow(i);

        Pupil* pu = dynamic_cast<Pupil*>(persons[i]);
        AmP* am = dynamic_cast<AmP*>(persons[i]);

        if (pu) {
            table->setItem(i, 0, new QTableWidgetItem(pu->getLastName()));
            table->setItem(i, 1, new QTableWidgetItem(pu->getFirstName()));
            QString middle = pu->getMiddleName();
            table->setItem(i, 2, new QTableWidgetItem(middle.isEmpty() ? "" : middle));
        } else if (am) {
            table->setItem(i, 0, new QTableWidgetItem(am->getLastName()));
            table->setItem(i, 1, new QTableWidgetItem(am->getFirstName()));
            QString second = am->getSecondName();
            table->setItem(i, 2, new QTableWidgetItem(second.isEmpty() ? "" : second));
        }

        table->setItem(i, 3, new QTableWidgetItem(persons[i]->getDate()));
    }

    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
}

void MainWindow::cellDoubleClicked(int row, int)
{
    PassForm *form = new PassForm(persons[row], row, nullptr);

    connect(form, &PassForm::personPrinted, this, [=](int index) {
        persons.remove(index);
        table->removeRow(index);
        saveToFile("data.txt");
    });

    form->show();
}

void MainWindow::saveToFile(QString path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) return;

    QTextStream out(&file);

    for (int i = 0; i < persons.size(); i++) {
        Person* p = persons[i];

        Pupil* pu = dynamic_cast<Pupil*>(p);
        AmP* am = dynamic_cast<AmP*>(p);

        if (pu) {
            out << "1|"
                << pu->getLastName() << "|"
                << pu->getFirstName() << "|"
                << pu->getMiddleName() << "|"
                << pu->getDay() << "|"
                << pu->getMonth() << "|"
                << pu->getYear() << "\n";
        }
        else if (am) {
            out << "2|"
                << am->getFirstName() << "|"
                << am->getSecondName() << "|"
                << am->getLastName() << "|"
                << am->getDay() << "|"
                << am->getMonth() << "|"
                << am->getYear() << "\n";
        }
    }
}
