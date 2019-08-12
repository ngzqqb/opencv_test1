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
                    1.05/*半径分辨率*/,
                    CV_PI / 180/*角度分辨率*/,
                    30/*直线点数阈值*/,
                    120/*直线长度阈值*/,
                    16/*最大距离*/);
   QImage varImage{  varCannyImage.cols,varCannyImage.rows,QImage::Format_RGBA8888 };
   varImage.fill( QColor(255,255,255,255) );
   {
       qDebug() << lines.size();
       QPainter varPainter{&varImage};
       varPainter.drawImage(QPoint{0,0}, QImage{varFileName} );
       varPainter.setPen({QColor{255,0,0},2 });
       for(const auto & varI : lines){
           varPainter.drawLine(QPoint{ varI[0],varI[1] },QPoint{ varI[2],varI[3] });
       }
   }
   varImage.save(varFileName+QStringLiteral(".jpg"));
   //sstd::saveImage(varCannyImage,varFileName+QStringLiteral(".jpg"));
}

