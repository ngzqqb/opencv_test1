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

void MainWindow::on_callButton_clicked() {
    auto varFileName = ui->imageFileName->text();
    qDebug() << sstd::evalAngle(varFileName);
}

