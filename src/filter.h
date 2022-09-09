#ifndef FILTER_H
#define FILTER_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
    class Filter;
}
QT_END_NAMESPACE

class Filter : public QDialog {
Q_OBJECT

public:
    explicit Filter(QWidget *parent = nullptr);

    ~Filter() override;

private slots:

    void on_openButton_clicked();

private:
    Ui::Filter *ui;
};

#endif // FILTER_H
