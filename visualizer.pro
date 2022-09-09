QT       += core gui charts quick quickwidgets concurrent
QT += positioning

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

RC_ICONS = img/app.ico

CONFIG += c++11

# CONFIG += release

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/filter.cpp \
    src/configuration.cpp \
    src/canvas/canvasHeatmap.cpp \
    src/canvas/canvasTrajectory.cpp \
    src/storage/dataset.cpp \
    src/storage/kdtree.cpp \
    src/storage/hashUser.cpp \
    src/storage/hashLocation.cpp \
    src/storage/mapTime.cpp \
    src/storage/dataTimeGeoCoordinateCount.cpp \
    src/storage/cacheQueryAll.cpp \
    src/storage/cacheQueryByRange.cpp \
    src/storage/cacheHeatmap.cpp \
    src/tab/tabPlot.cpp \
    src/tab/tabComparison.cpp \
    src/tab/tabTrajectory.cpp \
    src/tab/tabHeatmap.cpp \
    src/thread/thread.cpp \
    src/thread/threadLoadData.cpp \
    src/thread/threadConstructKDTree.cpp \
    src/thread/threadConstructTimeTree.cpp \
    src/thread/threadCalculateTimeGeoCoordinateCount.cpp \
    src/thread/query/threadQueryTrajectory.cpp \
    src/thread/query/threadQueryByUser.cpp \
    src/thread/query/threadQueryByLocation.cpp \
    src/thread/query/threadQueryByRange.cpp \
    src/thread/query/threadQueryAll.cpp \
    src/thread/query/threadQueryByUserLocation.cpp \
    src/thread/query/threadQueryByUserRange.cpp \

HEADERS += \
    src/mainwindow.h \
    src/filter.h \
    src/configuration.h \
    src/utilities.h \
    src/canvas/canvasHeatmap.h \
    src/canvas/canvasTrajectory.h \
    src/storage/dataset.h \
    src/storage/kdtree.h \
    src/storage/hashUser.h \
    src/storage/hashLocation.h \
    src/storage/mapTime.h \
    src/storage/dataTimeGeoCoordinateCount.h \
    src/storage/cacheQueryAll.h \
    src/storage/cacheQueryByRange.h \
    src/storage/cacheHeatmap.h \
    src/storage/dataTimeCount.h \
    src/thread/thread.h \
    src/thread/threadLoadData.h \
    src/thread/threadConstructKDTree.h \
    src/thread/threadConstructTimeTree.h \
    src/thread/threadCalculateTimeGeoCoordinateCount.h \
    src/thread/query/threadQueryTrajectory.h \
    src/thread/query/threadQueryByUser.h \
    src/thread/query/threadQueryByLocation.h \
    src/thread/query/threadQueryByRange.h \
    src/thread/query/threadQueryAll.h \
    src/thread/query/threadQueryByUserLocation.h \
    src/thread/query/threadQueryByUserRange.h \

FORMS += \
    src/filter.ui \
    src/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    visualizer.qrc
