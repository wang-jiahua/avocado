#include "filter.h"
#include "configuration.h"
#include "ui_filter.h"
#include <QCheckBox>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

Filter::Filter(QWidget *parent)
        : QDialog(parent), ui(new Ui::Filter) {
    ui->setupUi(this);

    ui->checkBoxUserID->setEnabled(false);
    ui->checkBoxLocationID->setEnabled(false);
    ui->checkBoxTime->setEnabled(false);

    connect(ui->checkBoxLatitude, &QCheckBox::toggled, ui->checkBoxLongitude, &QCheckBox::setChecked);
    connect(ui->checkBoxLongitude, &QCheckBox::toggled, ui->checkBoxLatitude, &QCheckBox::setChecked);
    connect(ui->checkBoxLatitude, &QCheckBox::toggled, this, [=](bool checked) {
        if (!checked) {
            QMessageBox::warning(this, "Warning", "Some functionalities are banned <br>"
                                                  "if GPS information not loaded");
        }
    });
}

Filter::~Filter() {
    delete ui;
}

void Filter::on_openButton_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open", "../", "Text Files (*.csv)");

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Warning", "Can not open file");
        return;
    }

    ui->buttonBox->setEnabled(true);

    Configuration::set(QString("path"), fileName);

    ui->lineEdit->setText(fileName);

    Configuration::set("GPS", true);

    connect(ui->checkBoxLatitude, &QCheckBox::toggled, [=](bool checked) {
        Configuration::set("GPS", checked);
    });

    ui->checkBoxUserID->setChecked(true);
    ui->checkBoxLocationID->setChecked(true);
    ui->checkBoxTime->setChecked(true);
    ui->checkBoxLatitude->setChecked(true);
    ui->checkBoxLongitude->setChecked(true);
}
