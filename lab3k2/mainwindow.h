#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include "binarytree.h"

class TreeWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoadTree();
    void onPostOrder();
    void onSaveResult();
    void onCountDivisibleBy3();

private:
    void setupUI();
    void drawTree(TreeNode* node, QPainter* painter,
                  int x, int y, int hSpacing, int vSpacing);

    BinaryTree* tree;
    QString postOrderResult;
    int divisibleCount;

    QPushButton* btnLoad;
    QPushButton* btnPostOrder;
    QPushButton* btnSave;
    QPushButton* btnCount;

    QTextEdit* txtPostOrder;
    QTextEdit* txtCount;
    QTextEdit* txtInfo;

    TreeWidget* treeWidget;
};

class TreeWidget : public QWidget {
    Q_OBJECT
public:
    explicit TreeWidget(QWidget *parent = nullptr);
    void setTree(TreeNode* root);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    TreeNode* root;
    void drawNode(TreeNode* node, QPainter* painter,
                  int x, int y, int hSpacing, int vSpacing, int depth);
};

#endif // MAINWINDOW_H
