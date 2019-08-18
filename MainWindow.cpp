#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include <QtWidgets/QtWidgets>
#include "ImageTool.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::on_imageFileNameButton_clicked() {
    auto varAns = QFileDialog::getOpenFileName(nullptr, {}, ui->imageFileName->text());
    if (varAns.isEmpty()) {
        return;
    }
    ui->imageFileName->setText(varAns);
}


class LineItem : public QLineF {
public:
    double thisLineLength;
    double thisLineAngle;
    inline LineItem(const QLineF & arg) :
        QLineF{ arg },
        thisLineLength{ arg.length() },
        thisLineAngle{ arg.angle() } {
        thisLineAngle = std::abs(std::fmod(thisLineAngle, 45.));
    }
    inline LineItem() :
        thisLineLength{ 0 },
        thisLineAngle{ 0 }{
    }
    inline LineItem(const LineItem &) = default;
    inline LineItem(LineItem &&) = default;
    inline LineItem&operator=(const LineItem &) = default;
    inline LineItem&operator=(LineItem &&) = default;
};

void MainWindow::on_callButton_clicked() {
    auto varFileName = ui->imageFileName->text();
    sstd::evalAngle(varFileName);
}

