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
    table->setColumnCount(2);
    table->setHorizontalHeaderLabels({"ФИО", "Дата рождения"});
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setWordWrap(true);
    table->resizeRowsToContents();

    // === ДОБАВЛЕНО ===
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

    while (true) {
        int type;

        if (!(in >> type)) break;

        if (type == 1) {
            QString l, f, m;
            int d, mo, y;
            in >> l >> f >> m >> d >> mo >> y;

            persons.push_back(new Pupil(l, f, m, d, mo, y));
        } else {
            QString f, s, l;
            int d, mo, y;
            in >> f >> s >> l >> d >> mo >> y;

            persons.push_back(new AmP(f, s, l, d, mo, y));
        }
    }

    for (int i = 0; i < persons.size(); i++) {
        table->insertRow(i);

        table->setItem(i, 0, new QTableWidgetItem(persons[i]->craft()));
        table->setItem(i, 1, new QTableWidgetItem(persons[i]->getDate()));
    }
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
            out << "1 "
                << pu->getLastName() << " "
                << pu->getFirstName() << " "
                << pu->getMiddleName() << " "
                << pu->getDate().replace(".", " ")
                << "\n";
        }
        else if (am) {
            out << "2 "
                << am->getFirstName() << " "
                << am->getSecondName() << " "
                << am->getLastName() << " "
                << am->getDate().replace(".", " ")
                << "\n";
        }
    }
}

