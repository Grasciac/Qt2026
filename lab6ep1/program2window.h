#ifndef PROGRAM2WINDOW_H
#define PROGRAM2WINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QFileDialog>
#include <QJsonObject>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QMessageBox>
#include "PointOfInterest.h"

class Program2Window : public QMainWindow
{
    Q_OBJECT

public:
    Program2Window(QWidget *parent = nullptr);
    ~Program2Window();

private slots:
    void loadJsonFile();

private:
    void setupUI();
    void loadAndProcessJson(const QString& filePath);
    void fillCorrectTable(const QList<PointOfInterest>& correctPois);
    void fillErrorTable(const QList<PointOfInterest>& errorPois);
    void saveErrorObjects(const QList<PointOfInterest>& errorPois);
    QString getValidationError(const PointOfInterest& poi);

    QTableWidget* correctTable;
    QTableWidget* errorTable;
    QPushButton* loadButton;

    QString currentJsonFile;
};

#endif // PROGRAM2WINDOW_H
