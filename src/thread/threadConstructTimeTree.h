#ifndef _THREAD_CONSTRUCT_TIME_TREE_H
#define _THREAD_CONSTRUCT_TIME_TREE_H

#include "thread.h"

class ThreadConstructTimeTree : public Thread {
Q_OBJECT

public:
    explicit ThreadConstructTimeTree(QObject *parent);

private:
    void run() override;
};

#endif // _THREAD_CONSTRUCT_TIME_TREE_H