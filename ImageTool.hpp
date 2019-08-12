#pragma once

#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <opencv2/opencv.hpp>

namespace sstd {

    /*所有opencv图片都是灰度图，所有opencv图片仅用于计算*/

    /*打开图片:打开为灰度图*/
    cv::Mat loadImage(const QString &);

    /*对图片进行直方图均衡化：*/
    cv::Mat histogramImage(const cv::Mat &);

    /*保存图片:只测试jpg格式，只用于测试*/
    bool saveImage(const cv::Mat &,const QString &);

    /*将Opencv Mat 转为QImage:只考虑1维度*/
    QImage toQImage(const cv::Mat &);

}/*namespace sstd*/

