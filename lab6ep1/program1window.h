#ifndef PROGRAM1WINDOW_H
#define PROGRAM1WINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QTextEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include "PointOfInterest.h"

class Program1Window : public QMainWindow
{
    Q_OBJECT

public:
    Program1Window(QWidget *parent = nullptr);
    ~Program1Window();

private slots:
    void saveToJson();
    void loadFromTxt();
    void createNewJson();
    void appendToExistingJson();
    void clearForm();

private:
    void setupUI();
    void appendToJsonFile(const QString& filePath, const PointOfInterest& poi);
    bool validateAndCreatePoi(PointOfInterest& poi);  // Вынесем валидацию

    QLineEdit* nameEdit;
    QTextEdit* descriptionEdit;
    QSpinBox* difficultySpin;
    QDoubleSpinBox* xSpin;
    QDoubleSpinBox* ySpin;

    QPushButton* saveButton;
    QPushButton* loadTxtButton;
    QPushButton* newJsonButton;
    QPushButton* appendJsonButton;

    QString currentJsonFile;
};

#endif // PROGRAM1WINDOW_H
