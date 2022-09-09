#ifndef _THREAD_CONSTRUCT_KDTREE_H
#define _THREAD_CONSTRUCT_KDTREE_H

#include "thread.h"

class ThreadConstructKDTree : public Thread {
Q_OBJECT

public:
    explicit ThreadConstructKDTree(QObject *parent);

private:
    void run() override;
};

#endif // _THREAD_CONSTRUCT_KDTREE_H