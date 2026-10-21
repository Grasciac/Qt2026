#ifndef BINARYTREE_H
#define BINARYTREE_H

#include <QString>
#include <QList>
#include <QVector>

struct TreeNode {
    int value;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int val) : value(val), left(nullptr), right(nullptr) {}
};

class BinaryTree {
public:
    BinaryTree();
    ~BinaryTree();

    // Построение дерева из файла (послойный обход)
    void loadFromFile(const QString &filename);

    // Сохранение результата обхода в файл
    void saveToFile(const QString &filename, const QString &data);

    // Постфиксный обход (ЛПУ: Left, Right, Root)
    QString postOrderTraversal() const;

    // Подсчёт узлов, значения которых кратны 3
    int countNodesDivisibleBy3() const;

    // Получить корень
    TreeNode* getRoot() const;

    // Очистка дерева
    void clear();

private:
    TreeNode* root;

    void postOrderHelper(TreeNode* node, QList<int> &result) const;
    int countDivisibleBy3Helper(TreeNode* node) const;
    void clearHelper(TreeNode* node);

    // Вспомогательная функция для построения дерева из послойного обхода
    TreeNode* buildTreeFromLevelOrder(const QVector<int> &values);
};

#endif // BINARYTREE_H
