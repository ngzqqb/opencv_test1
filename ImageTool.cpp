#include <cassert>
#include <string>
#include "ImageTool.hpp"

namespace sstd{

cv::Mat loadImage(const QString & arg){
    auto const varImageName = arg.toLocal8Bit();
    return cv::imread( {varImageName.data(), static_cast<std::size_t>( varImageName.size() )},
                                 cv::IMREAD_GRAYSCALE );
}

/* https://blog.csdn.net/wsp_1138886114/article/details/83793331 */
cv::Mat histogramImage(const cv::Mat & arg){
    cv::Mat varAns;
    if constexpr(false){
    auto varCLAHE = cv::createCLAHE(40.0,{8,8});
    varCLAHE->apply(arg,varAns);
    }else {
        cv::equalizeHist(arg,varAns);
    }
    return std::move(varAns);
}

bool saveImage(const cv::Mat & arg,const QString & argFileName){
    const auto varImageName = argFileName.toLocal8Bit();
    const cv::String varCVFileName {varImageName.data(), static_cast<std::size_t>( varImageName.size() )};
    return cv::imwrite(varCVFileName,arg);
}

QImage toQImage(const cv::Mat & arg){
    if(arg.empty()){
        return{};
    }
    const auto varImageWidth = arg.cols;
    const auto varImageHeight = arg.rows;

    assert( arg.channels() == 1 );
    auto varImageWrap = std::make_unique< cv::Mat>();
    arg.convertTo( *varImageWrap , CV_8UC1 );
    assert(varImageWidth == varImageWrap->cols );
    assert( varImageHeight == varImageWrap->rows );
    return QImage {
        varImageWrap->data,
        varImageWidth,
                varImageHeight,
                static_cast<int>(varImageWrap->step),
                QImage::Format_Grayscale8,
                [](void * arg){ delete reinterpret_cast<cv::Mat *>(arg); },
        varImageWrap.release()};
}

}/*namespace sstd*/

