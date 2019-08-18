#include <cassert>
#include <string>
#include "ImageTool.hpp"

namespace sstd {

    namespace private_eval_angle {

        class LineItem : public QLineF {
        public:
            double thisLineLength/*线段长度*/;
            double thisDx;
            double thisDy;

            inline LineItem(const QLineF & arg) :
                QLineF{ arg },
                thisLineLength{ arg.length() }{
            }

            inline void fullConstruct() {
                auto varRate = 1.0 / thisLineLength;
                thisDx = varRate * this->dx();
                thisDy = varRate * this->dy();
            }

            inline LineItem() :
                QLineF{ {1,0},{0,0} },
                thisLineLength{ 0 },
                thisDx{ 0 },
                thisDy{ 0 }{
            }

            inline LineItem(const LineItem &) = delete;
            inline LineItem(LineItem &&) = delete;
            inline LineItem&operator=(const LineItem &) = delete;
            inline LineItem&operator=(LineItem &&) = delete;
        };

        inline cv::Mat loadImage(const QString & arg) {
            auto const varImageName = arg.toLocal8Bit();
            return cv::imread({ varImageName.data(),
                                static_cast<std::size_t>(varImageName.size()) },
                cv::IMREAD_GRAYSCALE);
        }

        /* https://blog.csdn.net/wsp_1138886114/article/details/83793331 */
        inline cv::Mat histogramImage(const cv::Mat & arg) {
            cv::Mat varAns;
            if constexpr (false) {
                auto varCLAHE = cv::createCLAHE(40.0, { 8,8 });
                varCLAHE->apply(arg, varAns);
            } else {
                cv::equalizeHist(arg, varAns);
            }
            return std::move(varAns);
        }

        inline cv::Mat cannyImage(const cv::Mat & arg) {
            cv::Mat varAns;
            cv::Canny(arg, varAns, 86, 180);
            return std::move(varAns);
        }

        inline bool saveImage(const cv::Mat & arg, const QString & argFileName) {
            const auto varImageName = argFileName.toLocal8Bit();
            const cv::String varCVFileName{ varImageName.data(),
                        static_cast<std::size_t>(varImageName.size()) };
            return cv::imwrite(varCVFileName, arg);
        }

        inline QImage toQImage(const cv::Mat & arg) {
            if (arg.empty()) {
                return{};
            }
            const auto varImageWidth = arg.cols;
            const auto varImageHeight = arg.rows;

            assert(arg.channels() == 1);
            auto varImageWrap = std::make_unique< cv::Mat>();
            arg.convertTo(*varImageWrap, CV_8UC1);
            assert(varImageWidth == varImageWrap->cols);
            assert(varImageHeight == varImageWrap->rows);
            return QImage{
                varImageWrap->data,
                        varImageWidth,
                        varImageHeight,
                        static_cast<int>(varImageWrap->step),
                        QImage::Format_Grayscale8,
                        [](void * arg) { delete reinterpret_cast<cv::Mat *>(arg); },
                varImageWrap.release() };
        }

        inline std::vector< std::shared_ptr<LineItem> > houghLinesP(const cv::Mat & arg) {
            auto varLess = [](const auto & l, const auto & r) {
                return l->thisLineLength > r->thisLineLength;
            };
            std::vector< std::shared_ptr<LineItem> > varAns;
            {/*霍夫曼找直线...*/
                std::vector<cv::Vec4i> varLines;
                cv::HoughLinesP(arg,
                    varLines,
                    1.05/*半径分辨率*/,
                    CV_PI / 180/*角度分辨率*/,
                    30/*直线点数阈值*/,
                    120/*直线长度阈值*/,
                    16/*最大距离*/);
                varAns.reserve(varLines.size());
                for (const auto & varI : varLines) {
                    varAns.push_back(std::make_shared<LineItem>(QLineF{
                        QPoint{ varI[0],varI[1] },
                        QPoint{ varI[2],varI[3] } }));
                }
            }
            if (varAns.size() > 128) {/* 最多保留前128项 */
                std::nth_element(varAns.begin(), varAns.begin() + 128, varAns.end(), varLess);
                varAns.resize(128);
            }
            /* 从大到小排序 */
            std::sort(varAns.begin(), varAns.end(), varLess);
            /* 完整构造 */
            for (auto & varI : varAns) {
                varI->fullConstruct();
            }
            return std::move(varAns);
        }

        inline std::array<double, 2> PCAMain(const std::vector< std::shared_ptr<LineItem> > & arg) {
             
            cv::Mat varMean;
            cv::Mat varEigenVectors;
            std::vector< cv::Point2d > varPoints;
            varPoints.reserve(arg.size());

            for (const auto & varI:arg) {
                varPoints.emplace_back(varI->thisDx,varI->thisDy);
            }

            cv::PCACompute(varPoints, varMean, varEigenVectors);

            return{ varEigenVectors.at<double>(0,0),varEigenVectors.at<double>(0,1) };
        }

    }/**/

    double evalAngle(const QString & arg) {
        namespace ps = private_eval_angle;
        std::vector< std::shared_ptr<ps::LineItem> > varLines;
        {/**/
            cv::Mat varCannyImage;
            {/**/
                cv::Mat varHistogramImage;
                {/**/
                    cv::Mat varSourceImage;
                    varSourceImage = ps::loadImage(arg);
                    varHistogramImage = ps::histogramImage(varSourceImage);
                }
                varCannyImage = ps::cannyImage(varHistogramImage);
            }
            varLines = ps::houghLinesP(varCannyImage);
        }

        if (varLines.empty()) {/*没有找到符合要求的直线 .... */
            return 0;
        }

        if (varLines.size() == 1) {/*只找到一条符合要求的直线 ... */
            return std::fmod(varLines[0]->angle(), 45.);
        }

        /*获得主方向*/
        auto varMainAxis = ps::PCAMain(varLines);
        return std::fmod( std::atan2(varMainAxis[1],varMainAxis[0])/3.141592654*180,45.);

    }

}/*namespace sstd*/

// https://github.com/ngzHappy/oct2
// https://github.com/ngzHappy/oct2/tree/master/opencv_data/opencv_pca

