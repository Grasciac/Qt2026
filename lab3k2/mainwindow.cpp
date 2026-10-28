#include "mainwindow.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFont>

TreeWidget::TreeWidget(QWidget *parent) : QWidget(parent), root(nullptr) {
    setMinimumSize(600, 400);
    setStyleSheet("background-color: white;");
}

void TreeWidget::setTree(TreeNode* r) {
    root = r;
    update();
}

void TreeWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    if (!root) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int width = this->width();
    int vSpacing = 65;

    drawNode(root, &painter, width / 2, 40, width / 2, vSpacing, 0);
}

void TreeWidget::drawNode(TreeNode* node, QPainter* painter,
                          int x, int y, int hSpacing, int vSpacing, int depth) {
    if (!node) return;

    int radius = 20;
    int nextY = y + vSpacing;
    int nextHSpacing = hSpacing / 2;

    QPen linePen(QColor(120, 120, 120), 2);
    painter->setPen(linePen);

    if (node->left) {
        painter->drawLine(x, y, x - nextHSpacing, nextY);
    }
    if (node->right) {
        painter->drawLine(x, y, x + nextHSpacing, nextY);
    }

    QBrush brush(QColor(225, 235, 255));
    painter->setBrush(brush);

    QPen circlePen(QColor(60, 80, 180), 2);
    painter->setPen(circlePen);
    painter->drawEllipse(QPoint(x, y), radius, radius);

    painter->setPen(QColor(0, 0, 0));
    QFont font("Arial", 11, QFont::Bold);
    painter->setFont(font);
    painter->drawText(QRect(x - radius, y - radius, radius * 2, radius * 2),
                      Qt::AlignCenter, QString::number(node->value));

    drawNode(node->left, painter, x - nextHSpacing, nextY, nextHSpacing, vSpacing, depth + 1);
    drawNode(node->right, painter, x + nextHSpacing, nextY, nextHSpacing, vSpacing, depth + 1);
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    tree(new BinaryTree()), divisibleCount(0) {
    setupUI();
    setWindowTitle("Лабораторная работа №3 — Вариант 3: Бинарные деревья");
    resize(900, 700);
}

MainWindow::~MainWindow() {
    delete tree;
}

void MainWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout* btnLayout = new QHBoxLayout();

    btnLoad = new QPushButton("Загрузить дерево из файла");
    btnPostOrder = new QPushButton("Постфиксный обход (ЛПУ)");
    btnSave = new QPushButton("Сохранить результат в файл");
    btnCount = new QPushButton("Подсчёт узлов (кратных 3)");

    btnLoad->setStyleSheet("background-color: #4CAF50; color: white; padding: 8px;");
    btnPostOrder->setStyleSheet("background-color: #2196F3; color: white; padding: 8px;");
    btnSave->setStyleSheet("background-color: #FF9800; color: white; padding: 8px;");
    btnCount->setStyleSheet("background-color: #9C27B0; color: white; padding: 8px;");

    btnLayout->addWidget(btnLoad);
    btnLayout->addWidget(btnPostOrder);
    btnLayout->addWidget(btnSave);
    btnLayout->addWidget(btnCount);

    mainLayout->addLayout(btnLayout);

    treeWidget = new TreeWidget();
    treeWidget->setMinimumHeight(350);
    mainLayout->addWidget(treeWidget);

    QGroupBox* grpPostOrder = new QGroupBox("Постфиксный обход (ЛПУ: Левый — Правый — Узел)");
    QVBoxLayout* postLayout = new QVBoxLayout(grpPostOrder);
    txtPostOrder = new QTextEdit();
    txtPostOrder->setReadOnly(true);
    txtPostOrder->setMaximumHeight(60);
    postLayout->addWidget(txtPostOrder);
    mainLayout->addWidget(grpPostOrder);

    QGroupBox* grpCount = new QGroupBox("Подсчёт узлов, значения которых кратны 3");
    QVBoxLayout* countLayout = new QVBoxLayout(grpCount);
    txtCount = new QTextEdit();
    txtCount->setReadOnly(true);
    txtCount->setMaximumHeight(60);
    countLayout->addWidget(txtCount);
    mainLayout->addWidget(grpCount);

    QGroupBox* grpInfo = new QGroupBox("Информация");
    QVBoxLayout* infoLayout = new QVBoxLayout(grpInfo);
    txtInfo = new QTextEdit();
    txtInfo->setReadOnly(true);
    txtInfo->setMaximumHeight(80);
    txtInfo->setPlainText("Загрузите файл с деревом. Формат: послойный обход.\n"
                          "Пример:\n1\n4 3\n2 - 9 7\n\n"
                          "Вариант 3: Постфиксный обход + подсчёт узлов кратных 3.");
    infoLayout->addWidget(txtInfo);
    mainLayout->addWidget(grpInfo);

    connect(btnLoad, &QPushButton::clicked, this, &MainWindow::onLoadTree);
    connect(btnPostOrder, &QPushButton::clicked, this, &MainWindow::onPostOrder);
    connect(btnSave, &QPushButton::clicked, this, &MainWindow::onSaveResult);
    connect(btnCount, &QPushButton::clicked, this, &MainWindow::onCountDivisibleBy3);
}

void MainWindow::onLoadTree() {
    QString filename = QFileDialog::getOpenFileName(this,
                                                    "Выберите файл с деревом", "", "Text Files (*.txt);;All Files (*)");

    if (filename.isEmpty()) return;

    tree->loadFromFile(filename);

    TreeNode* root = tree->getRoot();
    if (root) {
        treeWidget->setTree(root);
        txtInfo->setPlainText("Дерево загружено из файла: " + filename);
        txtPostOrder->clear();
        txtCount->clear();
        postOrderResult.clear();
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось загрузить дерево. Проверьте формат файла.");
    }
}

void MainWindow::onPostOrder() {
    if (!tree->getRoot()) {
        QMessageBox::warning(this, "Ошибка", "Сначала загрузите дерево!");
        return;
    }

    postOrderResult = tree->postOrderTraversal();
    txtPostOrder->setPlainText("Постфиксный обход (ЛПУ): " + postOrderResult);
    txtInfo->append("Выполнен постфиксный обход.");
}

void MainWindow::onSaveResult() {
    if (postOrderResult.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Сначала выполните постфиксный обход!");
        return;
    }

    QString filename = QFileDialog::getSaveFileName(this,
                                                    "Сохранить результат", "postorder_result.txt", "Text Files (*.txt)");

    if (filename.isEmpty()) return;

    QString content = "Постфиксный обход (ЛПУ): " + postOrderResult + "\n"
                                                                      "Количество узлов, кратных 3: " + QString::number(divisibleCount) + "\n";

    tree->saveToFile(filename, content);
    QMessageBox::information(this, "Успех", "Результат сохранён в файл:\n" + filename);
}

void MainWindow::onCountDivisibleBy3() {
    if (!tree->getRoot()) {
        QMessageBox::warning(this, "Ошибка", "Сначала загрузите дерево!");
        return;
    }

    divisibleCount = tree->countNodesDivisibleBy3();
    txtCount->setPlainText("Количество узлов, значения которых кратны 3: " +
                           QString::number(divisibleCount));

    txtInfo->append("Подсчитаны узлы кратные 3. Результат: " +
                    QString::number(divisibleCount));
}
