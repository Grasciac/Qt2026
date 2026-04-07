#include "mainwindow.h"
#include <QRegularExpressionValidator>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFileInfo>

// Реализация класса Pair
Pair::Pair(const QString& subject, const QString& teacher, const QString& audience,
           int number, const QString& lessonType, const QStringList& expectations)
    : m_subject(subject)
    , m_teacher(teacher)
    , m_audience(audience)
    , m_number(number)
    , m_lessonType(lessonType)
    , m_expectations(expectations)
{
}

bool Pair::saveToFile(const QString& filename) const
{
    QFile file(filename);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    out << "Предмет: " << m_subject << "\n";
    out << "Преподаватель: " << m_teacher << "\n";
    out << "Аудитория: " << m_audience << "\n";
    out << "Номер в расписании: " << m_number << "\n";
    out << "Тип: " << m_lessonType << "\n";
    out << "Ожидания: " << (m_expectations.isEmpty() ? "Нет" : m_expectations.join(", ")) << "\n";
    out << "------------------------------\n";

    file.close();
    return true;
}

// Реализация MainWindow
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();

    // Инициализация регулярных выражений
    subjectRegex = QRegularExpression("^[А-Я][а-я]*$"); // рус, только первая буква слова капсом
    teacherRegex = QRegularExpression("^[А-Я][а-я]*\\s[А-Я]{1,2}$"); // рус, только первая буква слова капсом потом через пробел 1/2 буквы капсом
    audienceRegex = QRegularExpression("^(10[1-9]|1[1-3][0-9]|140|20[1-9]|2[1-3][0-9]|240|30[1-9]|3[1-3][0-9]|340|40[1-9]|4[1-3][0-9]|440)[А]?$");
    // число из диапазонов 101-140, 201-240, 301-340, 401-440 в конце возможна буква А
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Создаем сетку для полей ввода
    QGridLayout *gridLayout = new QGridLayout();

    // Предмет
    gridLayout->addWidget(new QLabel("Предмет:"), 0, 0);
    subjectEdit = new QLineEdit();
    subjectEdit->setPlaceholderText("Например: Математика");
    gridLayout->addWidget(subjectEdit, 0, 1);

    // Преподаватель
    gridLayout->addWidget(new QLabel("Преподаватель:"), 1, 0);
    teacherEdit = new QLineEdit();
    teacherEdit->setPlaceholderText("Например: Иванов ИИ");
    gridLayout->addWidget(teacherEdit, 1, 1);

    // Аудитория
    gridLayout->addWidget(new QLabel("Аудитория:"), 2, 0);
    audienceEdit = new QLineEdit();
    audienceEdit->setPlaceholderText("101-140, 201-240, 301-340, 401-440 + А");
    gridLayout->addWidget(audienceEdit, 2, 1);

    // Номер в расписании
    gridLayout->addWidget(new QLabel("Номер в расписании:"), 3, 0);
    numberCombo = new QComboBox();
    for (int i = 1; i <= 8; ++i) {
        numberCombo->addItem(QString::number(i));
    }
    gridLayout->addWidget(numberCombo, 3, 1);

    mainLayout->addLayout(gridLayout);

    // Группа для типа пары
    QGroupBox *typeGroupBox = new QGroupBox("Тип");
    QHBoxLayout *typeLayout = new QHBoxLayout();

    lectureRadio = new QRadioButton("Лекция");
    practiceRadio = new QRadioButton("Практика");

    typeGroup = new QButtonGroup(this);
    typeGroup->addButton(lectureRadio);
    typeGroup->addButton(practiceRadio);

    typeLayout->addWidget(lectureRadio);
    typeLayout->addWidget(practiceRadio);
    typeGroupBox->setLayout(typeLayout);

    mainLayout->addWidget(typeGroupBox);

    // Группа для ожиданий
    QGroupBox *expectGroupBox = new QGroupBox("Ожидание");
    QHBoxLayout *expectLayout = new QHBoxLayout();

    controlCheck = new QCheckBox("Контрольная");
    attendanceCheck = new QCheckBox("Проверка посещений");
    cancelCheck = new QCheckBox("Отмена");

    expectLayout->addWidget(controlCheck);
    expectLayout->addWidget(attendanceCheck);
    expectLayout->addWidget(cancelCheck);
    expectGroupBox->setLayout(expectLayout);

    mainLayout->addWidget(expectGroupBox);

    // Кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    resetButton = new QPushButton("Сброс");
    saveButton = new QPushButton("Сохранить");

    buttonLayout->addWidget(resetButton);
    buttonLayout->addWidget(saveButton);

    mainLayout->addLayout(buttonLayout);

    // Подключаем сигналы кнопок
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::onResetClicked);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::onSaveClicked);
}

void MainWindow::onResetClicked()
{
    // Очищаем все поля
    subjectEdit->clear();
    teacherEdit->clear();
    audienceEdit->clear();
    numberCombo->setCurrentIndex(0);

    // Сбрасываем радио-кнопки
    typeGroup->setExclusive(false);
    lectureRadio->setChecked(false);
    practiceRadio->setChecked(false);
    typeGroup->setExclusive(true);

    // Снимаем все чекбоксы
    controlCheck->setChecked(false);
    attendanceCheck->setChecked(false);
    cancelCheck->setChecked(false);
}

void MainWindow::onSaveClicked()
{
    if (!validateFields()) {
        return;
    }

    // Собираем данные
    QString subject = subjectEdit->text();
    QString teacher = teacherEdit->text();
    QString audience = audienceEdit->text();
    int number = numberCombo->currentText().toInt();

    QString lessonType;
    if (lectureRadio->isChecked()) {
        lessonType = "Лекция";
    } else if (practiceRadio->isChecked()) {
        lessonType = "Практика";
    } else {
        showError("Не выбран тип пары");
        return;
    }

    QStringList expectations;
    if (controlCheck->isChecked()) expectations << "Контрольная";
    if (attendanceCheck->isChecked()) expectations << "Проверка посещений";
    if (cancelCheck->isChecked()) expectations << "Отмена";

    // Создаем объект Pair и сохраняем в файл
    Pair pair(subject, teacher, audience, number, lessonType, expectations);

    if (pair.saveToFile()) {
        // 👇 ВОТ СЮДА ВСТАВЛЯЕМ ОТЛАДОЧНЫЙ ВЫВОД С ПУТЕМ К ФАЙЛУ
        QFileInfo fileInfo("result.txt");
        QString absolutePath = fileInfo.absoluteFilePath();

        QMessageBox::information(this, "Успех",
                                 QString("Данные успешно сохранены в:\n%1").arg(absolutePath));
        // 👆 КОНЕЦ ВСТАВКИ
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось сохранить данные в файл");
    }
}

bool MainWindow::validateFields()
{
    // Проверка предмета
    QString subject = subjectEdit->text();
    if (subject.isEmpty()) {
        showError("Поле 'Предмет' не может быть пустым");
        return false;
    }

    QRegularExpressionMatch match = subjectRegex.match(subject);
    if (!match.hasMatch()) {
        showError("Поле 'Предмет' должно содержать русское слово с заглавной буквы");
        return false;
    }

    // Проверка преподавателя
    QString teacher = teacherEdit->text();
    if (teacher.isEmpty()) {
        showError("Поле 'Преподаватель' не может быть пустым");
        return false;
    }

    match = teacherRegex.match(teacher);
    if (!match.hasMatch()) {
        showError("Поле 'Преподаватель' должно быть в формате: Фамилия ИО (например: Иванов ИИ)");
        return false;
    }

    // Проверка аудитории
    QString audience = audienceEdit->text();
    if (audience.isEmpty()) {
        showError("Поле 'Аудитория' не может быть пустым");
        return false;
    }

    match = audienceRegex.match(audience);
    if (!match.hasMatch()) {
        showError("Поле 'Аудитория' должно быть числом из диапазонов 101-140, 201-240, 301-340, 401-440, в конце возможна буква А");
        return false;
    }

    return true;
}

void MainWindow::showError(const QString& message)
{
    QMessageBox::warning(this, "Ошибка валидации", message);
}
