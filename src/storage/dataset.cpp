#include "dataset.h"

QHash<int, Fields> Dataset::dataset = QHash<int, Fields>();

QMutex Dataset::mutex;

bool Dataset::ready = false;
