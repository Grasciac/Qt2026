#include "program2window.h"

Program2Window::Program2Window(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Программа 2: Просмотр объектов Point of Interest");
    resize(900, 600);
    setupUI();
}

Program2Window::~Program2Window()
{
}

void Program2Window::setupUI()
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // Кнопка загрузки
    loadButton = new QPushButton("Загрузить .json");
    connect(loadButton, &QPushButton::clicked, this, &Program2Window::loadJsonFile);
    mainLayout->addWidget(loadButton);

    // Группа таблиц
    QWidget* tablesWidget = new QWidget();
    QHBoxLayout* tablesLayout = new QHBoxLayout(tablesWidget);

    // Таблица 1: Корректные объекты
    QGroupBox* correctGroup = new QGroupBox("Корректные объекты (отсортированы по названию Z-A)");
    QVBoxLayout* correctLayout = new QVBoxLayout(correctGroup);

    correctTable = new QTableWidget();
    correctTable->setColumnCount(5);
    correctTable->setHorizontalHeaderLabels({"Название", "Описание", "Сложность", "X", "Y"});
    correctTable->horizontalHeader()->setStretchLastSection(true);
    correctTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    correctLayout->addWidget(correctTable);

    // Таблица 2: Ошибки
    QGroupBox* errorGroup = new QGroupBox("Ошибки (битые объекты)");
    QVBoxLayout* errorLayout = new QVBoxLayout(errorGroup);

    errorTable = new QTableWidget();
    errorTable->setColumnCount(5);
    errorTable->setHorizontalHeaderLabels({"Название", "Описание", "Сложность", "X", "Y"});
    errorTable->horizontalHeader()->setStretchLastSection(true);
    errorTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    errorLayout->addWidget(errorTable);

    tablesLayout->addWidget(correctGroup, 1);
    tablesLayout->addWidget(errorGroup, 1);

    mainLayout->addWidget(tablesWidget);
}

void Program2Window::loadJsonFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Выберите JSON файл", "", "JSON Files (*.json)");
    if (filePath.isEmpty())
        return;

    currentJsonFile = filePath;
    loadAndProcessJson(filePath);
}

void Program2Window::loadAndProcessJson(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл!");
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isArray()) {
        QMessageBox::critical(this, "Ошибка", "JSON должен содержать массив объектов!");
        return;
    }

    QJsonArray array = doc.array();
    QList<PointOfInterest> correctPois;
    QList<PointOfInterest> errorPois;

    for (const QJsonValue& value : array) {
        if (value.isObject()) {
            PointOfInterest poi = PointOfInterest::fromJson(value.toObject());

            if (poi.isValid()) {
                correctPois.append(poi);
            } else {
                errorPois.append(poi);
            }
        }
    }

    // Сортировка по названию в обратном алфавитном порядке
    std::sort(correctPois.begin(), correctPois.end(), [](const PointOfInterest& a, const PointOfInterest& b) {
        return a.name > b.name;
    });

    fillCorrectTable(correctPois);
    fillErrorTable(errorPois);

    if (!errorPois.isEmpty()) {
        saveErrorObjects(errorPois);
    }

    QMessageBox::information(this, "Успех",
                             QString("Загружено объектов: %1\nКорректных: %2\nС ошибками: %3")
                                 .arg(array.size()).arg(correctPois.size()).arg(errorPois.size()));
}

void Program2Window::fillCorrectTable(const QList<PointOfInterest>& correctPois)
{
    correctTable->setRowCount(correctPois.size());

    for (int i = 0; i < correctPois.size(); ++i) {
        const PointOfInterest& poi = correctPois[i];

        correctTable->setItem(i, 0, new QTableWidgetItem(poi.name));
        correctTable->setItem(i, 1, new QTableWidgetItem(poi.description));
        correctTable->setItem(i, 2, new QTableWidgetItem(QString::number(poi.difficulty)));
        correctTable->setItem(i, 3, new QTableWidgetItem(QString::number(poi.x)));
        correctTable->setItem(i, 4, new QTableWidgetItem(QString::number(poi.y)));
    }
}

void Program2Window::fillErrorTable(const QList<PointOfInterest>& errorPois)
{
    errorTable->setRowCount(errorPois.size());

    QColor errorColor(255, 200, 200); // Светло-красный

    for (int i = 0; i < errorPois.size(); ++i) {
        const PointOfInterest& poi = errorPois[i];

        // Создаем ячейки
        QTableWidgetItem *item0 = new QTableWidgetItem(poi.name);
        QTableWidgetItem *item1 = new QTableWidgetItem(poi.description);
        QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(poi.difficulty));
        QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(poi.x));
        QTableWidgetItem *item4 = new QTableWidgetItem(QString::number(poi.y));

        // Проверяем и подсвечиваем только ошибочные ячейки
        if (poi.name.isEmpty()) {
            item0->setBackground(errorColor);
        }

        if (poi.description.isEmpty()) {
            item1->setBackground(errorColor);
        }

        if (poi.difficulty < 0 || poi.difficulty > 10) {
            item2->setBackground(errorColor);
        }

        // Устанавливаем ячейки в таблицу (всего 5 колонок)
        errorTable->setItem(i, 0, item0);
        errorTable->setItem(i, 1, item1);
        errorTable->setItem(i, 2, item2);
        errorTable->setItem(i, 3, item3);
        errorTable->setItem(i, 4, item4);
    }
}

QString Program2Window::getValidationError(const PointOfInterest& poi)
{
    QStringList errors;

    if (poi.name.isEmpty())
        errors << "name пустое";
    if (poi.description.isEmpty())
        errors << "описание пустое";
    else if (poi.difficulty < 1 || poi.difficulty > 10)
        errors << "сложность вне 0-10";

    return errors.isEmpty() ? "Неизвестная ошибка" : errors.join("; ");
}

void Program2Window::saveErrorObjects(const QList<PointOfInterest>& errorPois)
{
    QJsonArray errorArray;
    for (const PointOfInterest& poi : errorPois) {
        errorArray.append(poi.toJson());
    }

    QJsonDocument doc(errorArray);

    QString errorFilePath = currentJsonFile;
    errorFilePath.replace(".json", "_errors.json");

    QFile file(errorFilePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}
