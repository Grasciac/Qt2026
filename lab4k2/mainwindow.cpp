#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QPen>
#include <QBrush>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
    initializeGraph();
    drawGraph();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    resize(900, 750);
    setWindowTitle("Лабораторная работа №4. Графы и маршруты");

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    QHBoxLayout *controlLayout = new QHBoxLayout();

    startPointCombo = new QComboBox(this);
    endPointCombo = new QComboBox(this);
    findButton = new QPushButton("Вперед!", this);
    trafficButton = new QPushButton("Имитация пробок", this);
    saveButton = new QPushButton("Сохранить", this);
    routeLabel = new QLabel("Маршрут: —", this);

    routeLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #333;");

    controlLayout->addWidget(new QLabel("Точка А:", this));
    controlLayout->addWidget(startPointCombo);
    controlLayout->addWidget(new QLabel("Точка К:", this));
    controlLayout->addWidget(endPointCombo);
    controlLayout->addWidget(findButton);
    controlLayout->addWidget(trafficButton);
    controlLayout->addWidget(saveButton);
    controlLayout->addStretch();

    graphicsView = new QGraphicsView(this);
    scene = new QGraphicsScene(this);
    graphicsView->setScene(scene);
    graphicsView->setRenderHint(QPainter::Antialiasing);

    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(routeLabel);
    mainLayout->addWidget(graphicsView);

    connect(findButton, &QPushButton::clicked, this, &MainWindow::findRoute);
    connect(trafficButton, &QPushButton::clicked, this, &MainWindow::simulateTrafficJam);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveRoute);
}

void MainWindow::initializeGraph() {
    QString appPath = QCoreApplication::applicationDirPath();

    QDir projectDir(appPath);

    bool found = false;
    QString projectPath;

    for (int i = 0; i < 5; ++i) {
        if (projectDir.exists("main.cpp")) {
            projectPath = projectDir.absolutePath();
            found = true;
            break;
        }
        QFileInfoList list = projectDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QFileInfo &dirInfo : list) {
            QDir subDir(dirInfo.absoluteFilePath());
            if (subDir.exists("main.cpp")) {
                projectPath = subDir.absolutePath();
                found = true;
                break;
            }
        }
        if (found) break;
        projectDir.cdUp();
    }

    if (!found) {
        projectPath = appPath;
    }

    QString finalMapPath = projectPath + "/map.png";
    QPixmap mapPixmap(finalMapPath);

    if (mapPixmap.isNull()) {
        scene->setSceneRect(0, 0, 800, 500);
        scene->setBackgroundBrush(QBrush(QColor(245, 245, 245)));
        qWarning() << "Не удалось загрузить карту. Искали по пути:" << finalMapPath;
    } else {
        scene->setSceneRect(0, 0, mapPixmap.width(), mapPixmap.height());
        scene->addPixmap(mapPixmap);
    }

    graph.addVertex(Vertex(1, "A", QPointF(908, 142)));
    graph.addVertex(Vertex(2, "B", QPointF(648, 42)));
    graph.addVertex(Vertex(3, "C", QPointF(125, 65)));
    graph.addVertex(Vertex(4, "D", QPointF(860, 600)));
    graph.addVertex(Vertex(5, "E", QPointF(665, 575)));
    graph.addVertex(Vertex(6, "K", QPointF(93, 186)));
    graph.addVertex(Vertex(7, "G", QPointF(75, 460)));
    graph.addVertex(Vertex(8, "H", QPointF(910, 610)));

    graph.addVertex(Vertex(9,  "M", QPointF(1150, 150)));
    graph.addVertex(Vertex(10, "N", QPointF(400, 45)));
    graph.addVertex(Vertex(11, "P", QPointF(75,  230)));

    graph.addEdge(Edge(1, 2, 2.0));
    graph.addEdge(Edge(2, 3, 2.5));
    graph.addEdge(Edge(1, 4, 2.0));
    graph.addEdge(Edge(4, 5, 2.5));
    graph.addEdge(Edge(2, 5, 2.3));
    graph.addEdge(Edge(3, 6, 1.5));
    graph.addEdge(Edge(5, 7, 3.8));
    graph.addEdge(Edge(6, 7, 3.5));
    graph.addEdge(Edge(4, 8, 3.0));
    graph.addEdge(Edge(7, 8, 5.0));

    for (const auto& v : graph.getVertices()) {
        startPointCombo->addItem(QString("%1 (%2)").arg(v.name).arg(v.id), v.id);
        endPointCombo->addItem(QString("%1 (%2)").arg(v.name).arg(v.id), v.id);
    }
}

void MainWindow::drawGraph() {
    for (auto* item : graphItems) scene->removeItem(item);
    qDeleteAll(graphItems);
    graphItems.clear();

    for (const auto& edge : graph.getEdges()) {
        Vertex vFrom = findVertexById(edge.from);
        Vertex vTo = findVertexById(edge.to);

        QPen pen(edge.color, 3);
        QGraphicsLineItem *line = scene->addLine(QLineF(vFrom.position, vTo.position), pen);
        line->setZValue(1);
        graphItems.append(line);

        double totalWeight = edge.getWeight();
        QString textStr = QString("%1(%2)").arg(edge.length, 0, 'f', 1).arg(totalWeight, 0, 'f', 1);

        QGraphicsTextItem *textItem = scene->addText(textStr);
        textItem->setDefaultTextColor(Qt::black);
        textItem->setFont(QFont("Arial", 8, QFont::Bold));

        QPointF midPoint = (vFrom.position + vTo.position) / 2.0;
        textItem->setPos(midPoint.x() - 12, midPoint.y() - 10);
        textItem->setZValue(3);
        graphItems.append(textItem);
    }

    int radius = 8;
    for (const auto& v : graph.getVertices()) {
        QGraphicsEllipseItem *ellipse = scene->addEllipse(
            v.position.x() - radius, v.position.y() - radius,
            radius * 2, radius * 2,
            QPen(Qt::black, 1.5), QBrush(Qt::lightGray)
            );
        ellipse->setZValue(4);
        graphItems.append(ellipse);

        QGraphicsTextItem *text = scene->addText(v.name);
        text->setFont(QFont("Arial", 8, QFont::Bold));
        text->setPos(v.position.x() - 6, v.position.y() - 9);
        text->setZValue(5);
        graphItems.append(text);
    }
}

void MainWindow::findRoute() {
    clearRoute();

    int startId = startPointCombo->currentData().toInt();
    int endId = endPointCombo->currentData().toInt();

    if (startId == endId) {
        QMessageBox::information(this, "Внимание", "Точка отправления совпадает с точкой прибытия!");
        return;
    }

    currentPath = graph.findShortestPath(startId, endId);

    if (currentPath.isEmpty() || currentPath.size() < 2) {
        routeLabel->setText("Маршрут: Путь не найден!");
        QMessageBox::warning(this, "Ошибка", "Не удалось построить маршрут между выбранными точками.");
        return;
    }

    QStringList pathNames;
    for (int id : currentPath) {
        pathNames.append(findVertexById(id).name);
    }
    routeLabel->setText("Маршрут: " + pathNames.join(" -> "));

    drawRoute(currentPath);
}

void MainWindow::drawRoute(const QList<int>& path) {
    QPen routePen(QColor(0, 0, 255, 180), 7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    for (int i = 0; i < path.size() - 1; ++i) {
        Vertex vFrom = findVertexById(path[i]);
        Vertex vTo = findVertexById(path[i+1]);

        QGraphicsLineItem *routeLine = scene->addLine(QLineF(vFrom.position, vTo.position), routePen);
        routeLine->setZValue(2);
        routeItems.append(routeLine);
    }
}

void MainWindow::clearRoute() {
    for (auto* item : routeItems) scene->removeItem(item);
    qDeleteAll(routeItems);
    routeItems.clear();
    currentPath.clear();
    routeLabel->setText("Маршрут: —");
}

void MainWindow::simulateTrafficJam() {
    graph.simulateTraffic();
    drawGraph();
    if (!currentPath.isEmpty()) {
        findRoute();
    }
}

void MainWindow::saveRoute() {
    if (currentPath.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Нет активного маршрута для сохранения!");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить маршрут", "", "Text Files (*.txt)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "=== КРАТЧАЙШИЙ МАРШРУТ ===\n";

        QStringList pathNames;
        for (int id : currentPath) {
            pathNames.append(findVertexById(id).name);
        }
        out << "Путь: " << pathNames.join(" -> ") << "\n\n";

        out << "Порядок прохождения узлов (ID и Координаты):\n";
        for (int id : currentPath) {
            Vertex v = findVertexById(id);
            out << QString("Узел: %1 (ID: %2) | Координаты на карте: X=%3, Y=%4\n")
                       .arg(v.name).arg(v.id).arg(v.position.x()).arg(v.position.y());
        }

        file.close();
        QMessageBox::information(this, "Успех", "Маршрут успешно сохранен в файл!");
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл для записи.");
    }
}

Vertex MainWindow::findVertexById(int id) {
    for (const auto& v : graph.getVertices()) {
        if (v.id == id) return v;
    }
    return Vertex(-1, "ERR", QPointF(0,0));
}
