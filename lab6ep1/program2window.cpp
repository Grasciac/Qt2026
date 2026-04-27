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
    errorTable->setColumnCount(6);
    errorTable->setHorizontalHeaderLabels({"Название", "Описание", "Сложность", "X", "Y", "Ошибка"});
    errorTable->horizontalHeader()->setStretchLastSection(true);
    errorTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    errorTable->setStyleSheet("QTableWidget { background-color: #ffcccc; }");
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

    for (int i = 0; i < errorPois.size(); ++i) {
        const PointOfInterest& poi = errorPois[i];
        QString error = getValidationError(poi);

        errorTable->setItem(i, 0, new QTableWidgetItem(poi.name));
        errorTable->setItem(i, 1, new QTableWidgetItem(poi.description));
        errorTable->setItem(i, 2, new QTableWidgetItem(QString::number(poi.difficulty)));
        errorTable->setItem(i, 3, new QTableWidgetItem(QString::number(poi.x)));
        errorTable->setItem(i, 4, new QTableWidgetItem(QString::number(poi.y)));
        errorTable->setItem(i, 5, new QTableWidgetItem(error));
    }
}

QString Program2Window::getValidationError(const PointOfInterest& poi)
{
    if (poi.name.isEmpty())
        return "Поле 'name' пустое";
    if (poi.description.isEmpty())
        return "Поле 'description' пустое";
    if (poi.difficulty < 1 || poi.difficulty > 10)
        return "Сложность вне диапазона 1-10";
    return "Неизвестная ошибка";
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
