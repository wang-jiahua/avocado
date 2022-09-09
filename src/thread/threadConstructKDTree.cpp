#include "threadConstructKDTree.h"
#include "../storage/dataset.h"
#include "../storage/kdtree.h"
#include <QDebug>
#include <QTime>

ThreadConstructKDTree::ThreadConstructKDTree(QObject *parent)
        : Thread(parent) {
}

void ThreadConstructKDTree::run() {
    int oldTime = QTime::currentTime().msecsSinceStartOfDay();

    int cnt = 0;
    int MAX_CNT = Dataset::dataset.size();

    for (int primaryKey: Dataset::dataset.keys()) {
        const Fields &fields = Dataset::dataset[primaryKey];
        qint64 timestamp = fields.timestamp;
        double latitude = fields.latitude;
        double longitude = fields.longitude;

        QMap<int, QVariant> key = QMap<int, QVariant>();
        key.insert(0, timestamp);
        key.insert(1, latitude);
        key.insert(2, longitude);

        KDTree::insert(KDTree::root, Item(key, primaryKey), 0);

        ++cnt;
        emit changed((int) ((double) cnt / MAX_CNT * 1000));
    }

    for (int i = 0; i < 3; ++i) {
        Node *minNode = KDTree::findMin(KDTree::root, i, 0);
        Node *maxNode = KDTree::findMax(KDTree::root, i, 0);
        KDTree::treeRange.insert(i * 2, minNode->item.key[i]);
        KDTree::treeRange.insert(i * 2 + 1, maxNode->item.key[i]);
    }

    KDTree::mutex.lock();
    KDTree::ready = true;
    KDTree::mutex.unlock();

    emit ready();

    int newTime = QTime::currentTime().msecsSinceStartOfDay();

    qDebug() << "constructing KD tree costs" << (newTime - oldTime) / 1000.0 << "s";
}
