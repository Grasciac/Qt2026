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

    void loadFromFile(const QString &filename);

    void saveToFile(const QString &filename, const QString &data);

    QString postOrderTraversal() const;

    int countNodesDivisibleBy3() const;

    TreeNode* getRoot() const;

    void clear();

private:
    TreeNode* root;

    void postOrderHelper(TreeNode* node, QList<int> &result) const;
    int countDivisibleBy3Helper(TreeNode* node) const;
    void clearHelper(TreeNode* node);

    TreeNode* buildTreeFromLevelOrder(const QVector<int> &values);
    TreeNode* buildTreeWithIndex(const QVector<int> &values, int index);

};

#endif // BINARYTREE_H
