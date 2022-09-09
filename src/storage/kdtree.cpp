#include "kdtree.h"
#include <QDebug>

Node *KDTree::root = nullptr;
QMap<int, QVariant> KDTree::treeRange = QMap<int, QVariant>();
//int KDTree::cnt = 0;
QMutex KDTree::mutex;
bool KDTree::ready = false;

void KDTree::insert(Node *&t, Item item, int dim) {
    if (!t) {
        //qDebug() << ++cnt;
        t = new Node(item);
        return;
    }

    // return if primary key is the same
    if (t->item.value == item.value) {
        return;
    }

    if (item.key[dim] < t->item.key[dim]) {
        insert(t->left, item, (dim + 1) % MAXDIM);
    } else {
        insert(t->right, item, (dim + 1) % MAXDIM);
    }
}

// If the queried key doesn't exist, return NULL.
Node *KDTree::search(Node *t, QMap<int, QVariant> key, int dim) {
    if (!t) {
        return nullptr;
    }

    bool same = true;
    for (int i = 0; i < MAXDIM; ++i) {
        if (t->item.key[0] == key[0]) {
            same = false;
            break;
        }
    }
    if (same) {
        return t;
    }

    if (key[dim] < t->item.key[dim]) {
        return search(t->left, key, (dim + 1) % MAXDIM);
    }
    return search(t->right, key, (dim + 1) % MAXDIM);
}

Node *KDTree::minNode(Node *a, Node *b, int dimCmp) {
    if (!a) {
        return b;
    }
    if (!b) {
        return a;
    }
    return a->item.key[dimCmp] < b->item.key[dimCmp] ? a : b;
}

Node *KDTree::maxNode(Node *a, Node *b, int dimCmp) {
    if (!a) {
        return b;
    }
    if (!b) {
        return a;
    }
    return a->item.key[dimCmp] > b->item.key[dimCmp] ? a : b;
}

// Find the minimum value in a MAXDIM(dimCmp)
Node *KDTree::findMin(Node *t, int dimCmp, int dim) {
    if (!t) {
        return nullptr;
    }
    Node *rightMin;
    Node *min = findMin(t->left, dimCmp, (dim + 1) % MAXDIM);
    if (dimCmp != dim) {
        rightMin = findMin(t->right, dimCmp, (dim + 1) % MAXDIM);
        min = minNode(min, rightMin, dimCmp);
    }
    return minNode(min, t, dimCmp);
}

// Find the maximum value in a MAXDIM(dimCmp)
Node *KDTree::findMax(Node *t, int dimCmp, int dim) {
    if (!t) {
        return nullptr;
    }
    Node *leftMax;
    Node *max = findMax(t->right, dimCmp, (dim + 1) % MAXDIM);
    if (dimCmp != dim) {
        leftMax = findMax(t->left, dimCmp, (dim + 1) % MAXDIM);
        max = maxNode(max, leftMax, dimCmp);
    }
    return maxNode(max, t, dimCmp);
}

// searchRange contains 6 values [t_min, t_max, la_min, la_max, lo_min, lo_max]
QMap<qint64, int> KDTree::rangeSearchByTime(Node *t, int dim, QMap<int, QVariant> searchRange) {
    QMap<qint64, int> visit = QMap<qint64, int>();

    if (!t) {
        return {};
    }
    if (t->item.key[dim] < searchRange[dim * 2]) {
        return rangeSearchByTime(t->right, (dim + 1) % MAXDIM, searchRange);
    }
    if (t->item.key[dim] > searchRange[dim * 2 + 1]) {
        return rangeSearchByTime(t->left, (dim + 1) % MAXDIM, searchRange);
    }

    bool inRange = true;
    for (int i = 1; i < MAXDIM; ++i) {
        if (t->item.key[(dim + i) % MAXDIM] < searchRange[(dim + i) * 2 % (MAXDIM * 2)]
            || t->item.key[(dim + i) % MAXDIM] > searchRange[((dim + i) * 2 + 1) % (MAXDIM * 2)]) {
            // qDebug() << t->item.key[(dim + i) % MAXDIM] << "\t" << t->item.key[(dim + i) % MAXDIM];
            inRange = false;
            break;
        }
    }
    if (inRange) {
        ++visit[t->item.key[0].toLongLong()];
    }

    visit.unite(rangeSearchByTime(t->left, (dim + 1) % MAXDIM, searchRange));
    visit.unite(rangeSearchByTime(t->right, (dim + 1) % MAXDIM, searchRange));

    for (qint64 timestamp: visit.keys()) {
        int cnt = 0;
        for (int num: visit.values(timestamp)) {
            cnt += num;
        }
        visit[timestamp] = cnt;
    }

    return visit;
}

// return a set of primarykey
QSet<int> KDTree::rangeSearchByLocation(Node *t, int dim, QMap<int, QVariant> searchRange) {
    QSet<int> visit = QSet<int>();

    if (!t) {
        return {};
    }
    if (t->item.key[dim] < searchRange[dim * 2]) {
        return rangeSearchByLocation(t->right, (dim + 1) % MAXDIM, searchRange);
    }
    if (t->item.key[dim] > searchRange[dim * 2 + 1]) {
        return rangeSearchByLocation(t->left, (dim + 1) % MAXDIM, searchRange);
    }

    bool inRange = true;
    for (int i = 1; i < MAXDIM; ++i) {
        if (t->item.key[(dim + i) % MAXDIM] < searchRange[(dim + i) * 2 % (MAXDIM * 2)]
            || t->item.key[(dim + i) % MAXDIM] > searchRange[((dim + i) * 2 + 1) % (MAXDIM * 2)]) {
            // qDebug() << t->item.key[(dim + i) % MAXDIM] << "\t" << t->item.key[(dim + i) % MAXDIM];
            inRange = false;
            break;
        }
    }
    if (inRange) {
        visit.insert(t->item.value);
    }

    visit.unite(rangeSearchByLocation(t->left, (dim + 1) % MAXDIM, searchRange));
    visit.unite(rangeSearchByLocation(t->right, (dim + 1) % MAXDIM, searchRange));

    return visit;
}
