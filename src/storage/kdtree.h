#ifndef _KDTREE_H
#define _KDTREE_H

#include <QMap>
#include <QMutex>
#include <QVariant>
#include <utility>

constexpr int MAXDIM = 3;

class Item {
public:
    QMap<int, QVariant> key; // store 1-timestamp, 2-latitude and 3-longitude in key
    int value; // here "value" is actually the primary key

    explicit Item(QMap<int, QVariant> _key, int _value = 0)
            : key(std::move(_key)), value(_value) {
    }
};

class Node {
public:
    Item item;
    Node *left, *right;

    explicit Node(Item _item)
            : item(std::move(_item)), left(nullptr), right(nullptr) {
    }
};

class KDTree {
public:
    static void insert(Node *&t, Item item, int dim);

    static Node *search(Node *t, QMap<int, QVariant> key, int dim);

    static Node *findMin(Node *t, int dimCmp, int dim);

    static Node *findMax(Node *t, int dimCmp, int dim);

    static QMap<qint64, int> rangeSearchByTime(Node *t, int dim, QMap<int, QVariant> searchRange);

    static QSet<int> rangeSearchByLocation(Node *t, int dim, QMap<int, QVariant> searchRange);

    static Node *root;
    static QMap<int, QVariant> treeRange;
    static QMutex mutex;
    static bool ready;

private:
    static Node *minNode(Node *a, Node *b, int dimCmp);

    static Node *maxNode(Node *a, Node *b, int dimCmp);
};

#endif // _KDTREE_H
