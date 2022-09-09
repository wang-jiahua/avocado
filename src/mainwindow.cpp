#include "mainwindow.h"
#include "configuration.h"
#include "thread/threadCalculateTimeGeoCoordinateCount.h"
#include "thread/threadConstructKDTree.h"
#include "thread/threadConstructTimeTree.h"
#include "thread/threadLoadData.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow), filter(new Filter(parent)),
          progressBar(new QProgressBar(parent)), progressText(new QLabel(parent)), timer(new QTimer(parent)) {
    ui->setupUi(this);
    progressBar->setRange(0, 1000);
    ui->statusbar->addPermanentWidget(progressText, 0);
    ui->statusbar->addPermanentWidget(progressBar, 1);

    connect(ui->actionLoadData, &QAction::triggered, filter, &QDialog::open);
    ui->actionLoadData->setShortcuts(QKeySequence::Open);

    connect(ui->actionExit, &QAction::triggered, this, &QApplication::quit);
    ui->actionExit->setShortcuts(QKeySequence::Quit);

    connect(ui->actionAbout, &QAction::triggered, this, [=]() {
        QMessageBox::about(this, tr("About"),
                           tr(" The final project of CS2309 Problem Solving And Practice "
                              "<p> Analysis and Visualization of POI Checking-In Data "));
    });

    connect(filter, &QDialog::accepted, this, &MainWindow::on_filter_accepted);

    initTabPlot();
    initTabTrajectory();
    initTabHeatmap();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_filter_accepted() {
    bool GPSLoaded = Configuration::get(QString("GPS")).toBool();
    ui->plotLatitudeBegin->setEnabled(GPSLoaded);
    ui->plotLatitudeEnd->setEnabled(GPSLoaded);
    ui->plotLongitudeBegin->setEnabled(GPSLoaded);
    ui->plotLongitudeEnd->setEnabled(GPSLoaded);
    ui->trajectoryApply->setEnabled(GPSLoaded);
    ui->trajectoryPlay->setEnabled(GPSLoaded);
    ui->trajectoryTimeLine->setEnabled(GPSLoaded);
    ui->heatmapTimeLine->setEnabled(GPSLoaded);

    auto *threadLoadData = new ThreadLoadData(this);

    connect(threadLoadData, &ThreadLoadData::message, this, [=](const QString &msg) {
        QMessageBox::warning(this, "Warning", msg, QMessageBox::Ok, QMessageBox::Ok);
    });

    connect(threadLoadData, &ThreadLoadData::started, this, [=]() {
        progressText->setText(" Loading data... ");
    });

    connect(threadLoadData, &ThreadLoadData::changed, progressBar, &QProgressBar::setValue);

    connect(threadLoadData, &ThreadLoadData::ready, this, [=]() {
        progressText->setText(" ready ");

        // latitude and longitude selected
        if (Configuration::get(QString("GPS")).toBool()) {
            auto threadCalculateTimeGeoCoordinateCount = new ThreadCalculateTimeGeoCoordinateCount(this);
            auto threadConstructKDTree = new ThreadConstructKDTree(this);

            connect(threadConstructKDTree, &ThreadConstructKDTree::started, this, [=]() {
                progressText->setText(" Constructing KD tree... ");
            });

            connect(threadConstructKDTree, &ThreadConstructKDTree::changed, progressBar, &QProgressBar::setValue);

            connect(threadConstructKDTree, &ThreadConstructKDTree::ready, this, [=]() {
                progressText->setText(" ready ");
            });

            threadCalculateTimeGeoCoordinateCount->start();
            threadConstructKDTree->start();
        } else {
            auto threadConstructTimeTree = new ThreadConstructTimeTree(this);

            connect(threadConstructTimeTree, &ThreadConstructTimeTree::started, this, [=]() {
                progressText->setText(" Constructing Time tree... ");
            });

            connect(threadConstructTimeTree, &ThreadConstructTimeTree::changed, progressBar, &QProgressBar::setValue);

            connect(threadConstructTimeTree, &ThreadConstructTimeTree::ready, this, [=]() {
                progressText->setText(" ready ");
            });

            threadConstructTimeTree->start();
        }
    });

    threadLoadData->start();
}
