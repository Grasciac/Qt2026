#include "mainwindow.h"
#include "program1window.h"
#include "program2window.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Лабораторная работа №6: Работа с JSON");
    resize(500, 300);
    setupUI();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // Заголовок
    QLabel* titleLabel = new QLabel("Выберите программу:");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 20px;");
    mainLayout->addWidget(titleLabel);

    // Группа кнопок
    QGroupBox* buttonGroup = new QGroupBox("Программы");
    QVBoxLayout* groupLayout = new QVBoxLayout(buttonGroup);

    // Кнопка программы 1
    QPushButton* btnProgram1 = new QPushButton("Программа 1: Создание объекта\n(Добавление в JSON / импорт из TXT)");
    btnProgram1->setMinimumHeight(60);
    btnProgram1->setStyleSheet("QPushButton { font-size: 14px; padding: 10px; }");
    connect(btnProgram1, &QPushButton::clicked, this, &MainWindow::openProgram1);
    groupLayout->addWidget(btnProgram1);

    // Кнопка программы 2
    QPushButton* btnProgram2 = new QPushButton("Программа 2: Просмотр объектов\n(Валидация / сортировка / таблицы)");
    btnProgram2->setMinimumHeight(60);
    btnProgram2->setStyleSheet("QPushButton { font-size: 14px; padding: 10px; }");
    connect(btnProgram2, &QPushButton::clicked, this, &MainWindow::openProgram2);
    groupLayout->addWidget(btnProgram2);

    mainLayout->addWidget(buttonGroup);
    mainLayout->addStretch();

    // Информация
    QLabel* infoLabel = new QLabel("Вариант 6: Точка интереса\nПоля: название, описание, сложность, X, Y");
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setStyleSheet("color: gray; font-size: 12px;");
    mainLayout->addWidget(infoLabel);
}

void MainWindow::openProgram1()
{
    Program1Window* prog1 = new Program1Window();
    prog1->setAttribute(Qt::WA_DeleteOnClose);
    prog1->show();
}

void MainWindow::openProgram2()
{
    Program2Window* prog2 = new Program2Window();
    prog2->setAttribute(Qt::WA_DeleteOnClose);
    prog2->show();
}
