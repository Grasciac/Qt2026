#include "program1window.h"

Program1Window::Program1Window(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Программа 1: Создание объекта Point of Interest");
    resize(500, 500);
    setupUI();
}

Program1Window::~Program1Window()
{
}

void Program1Window::setupUI()
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // Группа "Новый объект"
    QGroupBox* groupBox = new QGroupBox("Новый объект");
    QFormLayout* formLayout = new QFormLayout(groupBox);

    nameEdit = new QLineEdit();
    formLayout->addRow("Название:", nameEdit);

    descriptionEdit = new QTextEdit();
    descriptionEdit->setMaximumHeight(80);
    formLayout->addRow("Описание:", descriptionEdit);

    difficultySpin = new QSpinBox();
    difficultySpin->setRange(1, 10);
    difficultySpin->setValue(1);
    formLayout->addRow("Сложность:", difficultySpin);

    xSpin = new QDoubleSpinBox();
    xSpin->setRange(-10000, 10000);
    xSpin->setDecimals(2);
    formLayout->addRow("Координата X:", xSpin);

    ySpin = new QDoubleSpinBox();
    ySpin->setRange(-10000, 10000);
    ySpin->setDecimals(2);
    formLayout->addRow("Координата Y:", ySpin);

    mainLayout->addWidget(groupBox);

    // Кнопки работы с файлами
    QGroupBox* fileGroupBox = new QGroupBox("Работа с JSON файлом");
    QVBoxLayout* fileGroupLayout = new QVBoxLayout(fileGroupBox);

    // Кнопка 1: Новый JSON файл
    newJsonButton = new QPushButton("1. Создать новый JSON файл и сохранить");
    newJsonButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 8px; }");
    connect(newJsonButton, &QPushButton::clicked, this, &Program1Window::createNewJson);
    fileGroupLayout->addWidget(newJsonButton);

    // Кнопка 2: Добавить к существующему
    appendJsonButton = new QPushButton("2. Добавить к существующему JSON файлу");
    appendJsonButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 8px; }");
    connect(appendJsonButton, &QPushButton::clicked, this, &Program1Window::appendToExistingJson);
    fileGroupLayout->addWidget(appendJsonButton);

    // Кнопка 3: Просто сохранить (быстрая)
    saveButton = new QPushButton("3. Сохранить (в последний использованный файл)");
    saveButton->setStyleSheet("QPushButton { background-color: #FF9800; color: white; padding: 8px; }");
    connect(saveButton, &QPushButton::clicked, this, &Program1Window::saveToJson);
    fileGroupLayout->addWidget(saveButton);

    mainLayout->addWidget(fileGroupBox);

    // Кнопка загрузки из TXT
    QWidget* buttonWidget = new QWidget();
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(0, 10, 0, 0);

    loadTxtButton = new QPushButton("📄 Загрузить из .txt");
    loadTxtButton->setFixedWidth(150);
    connect(loadTxtButton, &QPushButton::clicked, this, &Program1Window::loadFromTxt);

    buttonLayout->addWidget(loadTxtButton);
    buttonLayout->addStretch();

    mainLayout->addWidget(buttonWidget);
    mainLayout->addStretch();

    // Информация
    QLabel* infoLabel = new QLabel("💡 Совет: Заполните поля или загрузите из TXT, затем выберите способ сохранения");
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setStyleSheet("color: gray; font-size: 11px; padding: 5px;");
    mainLayout->addWidget(infoLabel);
}

void Program1Window::loadFromTxt()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Выберите TXT файл", "", "Text Files (*.txt)");
    if (filePath.isEmpty())
        return;

    PointOfInterest poi = PointOfInterest::parseFromTxt(filePath);

    nameEdit->setText(poi.name);
    descriptionEdit->setText(poi.description);
    difficultySpin->setValue(poi.difficulty);
    xSpin->setValue(poi.x);
    ySpin->setValue(poi.y);

    QMessageBox::information(this, "Успех", "Данные из TXT загружены в форму. Можете отредактировать их и сохранить.");
}

void Program1Window::createNewJson()
{
    PointOfInterest poi;
    if (!validateAndCreatePoi(poi))
        return;

    QString filePath = QFileDialog::getSaveFileName(this, "Создать новый JSON файл", "", "JSON Files (*.json)");
    if (filePath.isEmpty())
        return;

    currentJsonFile = filePath;

    // Создаем новый файл с одним объектом
    QJsonArray array;
    array.append(poi.toJson());
    QJsonDocument doc(array);

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        QMessageBox::information(this, "Успех", "Новый JSON файл создан:\n" + filePath);

        // Очищаем форму
        clearForm();
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать файл!");
    }
}

void Program1Window::appendToExistingJson()
{
    PointOfInterest poi;
    if (!validateAndCreatePoi(poi))
        return;

    QString filePath = QFileDialog::getOpenFileName(this, "Выберите существующий JSON файл", "", "JSON Files (*.json)");
    if (filePath.isEmpty())
        return;

    // Проверяем, существует ли файл
    QFile file(filePath);
    if (!file.exists()) {
        QMessageBox::warning(this, "Ошибка", "Файл не существует! Используйте 'Создать новый JSON файл'.");
        return;
    }

    currentJsonFile = filePath;
    appendToJsonFile(filePath, poi);

    QMessageBox::information(this, "Успех", "Объект добавлен в файл:\n" + filePath);

    // Очищаем форму
    clearForm();
}

void Program1Window::saveToJson()
{
    PointOfInterest poi;
    if (!validateAndCreatePoi(poi))
        return;

    // Если файл не выбран, предлагаем сохранить
    if (currentJsonFile.isEmpty()) {
        int reply = QMessageBox::question(this, "Файл не выбран",
                                          "Файл не выбран. Что хотите сделать?\n\n"
                                          "Да - Создать новый файл\n"
                                          "Нет - Выбрать существующий файл\n"
                                          "Отмена - Отменить сохранение",
                                          QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (reply == QMessageBox::Yes) {
            createNewJson();
            return;
        } else if (reply == QMessageBox::No) {
            appendToExistingJson();
            return;
        } else {
            return; // Отмена
        }
    }

    // Проверяем существует ли файл
    QFile file(currentJsonFile);
    if (!file.exists()) {
        // Создаем новый файл
        QJsonArray array;
        array.append(poi.toJson());
        QJsonDocument doc(array);

        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson());
            file.close();
            QMessageBox::information(this, "Успех", "Объект сохранен в новый JSON файл!");
        }
    } else {
        // Добавляем в существующий файл
        appendToJsonFile(currentJsonFile, poi);
        QMessageBox::information(this, "Успех", "Объект добавлен в JSON файл!");
    }

    // Очищаем форму
    clearForm();
}

void Program1Window::appendToJsonFile(const QString& filePath, const PointOfInterest& poi)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл для чтения!");
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonArray array = doc.isArray() ? doc.array() : QJsonArray();

    // Добавляем новый объект
    array.append(poi.toJson());

    QJsonDocument newDoc(array);

    if (file.open(QIODevice::WriteOnly)) {
        file.write(newDoc.toJson());
        file.close();
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось записать в файл!");
    }
}

bool Program1Window::validateAndCreatePoi(PointOfInterest& poi)
{
    if (nameEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Поле 'Название' обязательно для заполнения!");
        nameEdit->setFocus();
        return false;
    }

    if (descriptionEdit->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Поле 'Описание' обязательно для заполнения!");
        descriptionEdit->setFocus();
        return false;
    }

    // Создаем объект
    poi = PointOfInterest(
        nameEdit->text(),
        descriptionEdit->toPlainText(),
        difficultySpin->value(),
        xSpin->value(),
        ySpin->value()
        );

    if (!poi.isValid()) {
        QMessageBox::warning(this, "Ошибка", "Объект некорректен! Проверьте данные.");
        return false;
    }

    return true;
}

void Program1Window::clearForm()
{
    nameEdit->clear();
    descriptionEdit->clear();
    difficultySpin->setValue(1);
    xSpin->setValue(0);
    ySpin->setValue(0);
    nameEdit->setFocus();
}
