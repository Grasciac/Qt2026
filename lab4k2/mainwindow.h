#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QGraphicsTextItem>
#include "graph.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void findRoute();
    void simulateTrafficJam();
    void saveRoute();

private:
    void setupUI();
    void initializeGraph();
    void drawGraph();
    void drawRoute(const QList<int>& path);
    void clearRoute();
    Vertex findVertexById(int id);

    QGraphicsView *graphicsView;
    QGraphicsScene *scene;
    QComboBox *startPointCombo;
    QComboBox *endPointCombo;
    QPushButton *findButton;
    QPushButton *trafficButton;
    QPushButton *saveButton;
    QLabel *routeLabel;

    Graph graph;
    QList<int> currentPath;

    QList<QGraphicsItem*> graphItems;
    QList<QGraphicsItem*> routeItems;
};

#endif
