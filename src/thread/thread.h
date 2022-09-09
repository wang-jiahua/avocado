#ifndef _THREAD_H
#define _THREAD_H

#include <QThread>

class Thread : public QThread {
Q_OBJECT

signals:

    void changed(int value);

    void ready();

public:
    explicit Thread(QObject *parent);
};

#endif // _THREAD_H