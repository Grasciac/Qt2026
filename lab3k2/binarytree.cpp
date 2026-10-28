#include "binarytree.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QStringList>
#include <QRegularExpression>

BinaryTree::BinaryTree() : root(nullptr) {}

BinaryTree::~BinaryTree() {
    clear();
}

void BinaryTree::clear() {
    clearHelper(root);
    root = nullptr;
}

void BinaryTree::clearHelper(TreeNode* node) {
    if (node) {
        clearHelper(node->left);
        clearHelper(node->right);
        delete node;
    }
}

TreeNode* BinaryTree::getRoot() const {
    return root;
}

void BinaryTree::loadFromFile(const QString &filename) {
    clear();

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Не удалось открыть файл:" << filename;
        return;
    }

    QVector<int> values;
    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList tokens = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        for (const QString &token : tokens) {
            if (token == "-") {
                values.append(-1);
            } else {
                bool ok;
                int val = token.toInt(&ok);
                if (ok) {
                    values.append(val);
                }
            }
        }
    }
    file.close();

    root = buildTreeFromLevelOrder(values);
}

TreeNode* BinaryTree::buildTreeWithIndex(const QVector<int> &values, int index) {
    if (index >= values.size() || values[index] == -1) {
        return nullptr;
    }

    TreeNode* node = new TreeNode(values[index]);

    node->left = buildTreeWithIndex(values, 2 * index + 1);
    node->right = buildTreeWithIndex(values, 2 * index + 2);

    return node;
}

TreeNode* BinaryTree::buildTreeFromLevelOrder(const QVector<int> &values) {
    if (values.isEmpty()) return nullptr;
    return buildTreeWithIndex(values, 0);
}



QString BinaryTree::postOrderTraversal() const {
    QList<int> result;
    postOrderHelper(root, result);

    QStringList strList;
    for (int val : result) {
        strList.append(QString::number(val));
    }
    return strList.join(", ");
}

void BinaryTree::postOrderHelper(TreeNode* node, QList<int> &result) const {
    if (!node) return;
    postOrderHelper(node->left, result);
    postOrderHelper(node->right, result);
    result.append(node->value);
}

int BinaryTree::countNodesDivisibleBy3() const {
    return countDivisibleBy3Helper(root);
}

int BinaryTree::countDivisibleBy3Helper(TreeNode* node) const {
    if (!node) return 0;

    int count = 0;
    if (node->value % 3 == 0 && node->value != 0) {
        count = 1;
    }

    count += countDivisibleBy3Helper(node->left);
    count += countDivisibleBy3Helper(node->right);

    return count;
}

void BinaryTree::saveToFile(const QString &filename, const QString &data) {
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << data;
        file.close();
    }
}
