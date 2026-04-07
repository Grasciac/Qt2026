#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QTableWidget>
#include <QPushButton>
#include "person.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QVector<Person*> persons;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QTableWidget *table;
    QPushButton *loadBtn;

    void loadFromFile(QString path);
    void saveToFile(QString path);

private slots:
    void loadClicked();
    void cellDoubleClicked(int row, int column);
};

#endif
