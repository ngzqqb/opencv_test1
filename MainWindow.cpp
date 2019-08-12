#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include <QtWidgets/QtWidgets>
#include "ImageTool.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::on_imageFileNameButton_clicked(){
    auto varAns = QFileDialog::getOpenFileName(nullptr,{},ui->imageFileName->text());
    if(varAns.isEmpty()){
        return;
    }
    ui->imageFileName->setText(varAns);
}

void MainWindow::on_callButton_clicked() {
   auto varFileName = ui->imageFileName->text();
   auto varSourceImage = sstd::loadImage(varFileName);
   auto varHistogramImage = sstd::histogramImage(varSourceImage);
   auto varCannyImage = sstd::cannyImage(varHistogramImage);
   std::vector<cv::Vec4i> lines;
   cv::HoughLinesP( varCannyImage,
                    lines,
                    1/*半径分辨率*/,
                    CV_PI / 72/*角度分辨率*/,
                    300/*线段长度阈值*/ );
   sstd::saveImage(varCannyImage,varFileName+QStringLiteral(".jpg"));
}

