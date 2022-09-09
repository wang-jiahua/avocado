#ifndef _THREAD_LOAD_DATA_H
#define _THREAD_LOAD_DATA_H

#include "thread.h"
#include <QString>

class ThreadLoadData : public Thread {
Q_OBJECT

signals:

    void message(const QString &msg);

public:
    explicit ThreadLoadData(QObject *parent);

private:
    void run() override;
};

#endif // _THREAD_LOAD_DATA_H
