#ifndef QTOPENCVCONVERSIONS_HPP
#define QTOPENCVCONVERSIONS_HPP

#include <core/core.hpp>
#include <imgproc/imgproc.hpp>
#include <imgproc/types_c.h>
#include <QImage>

namespace QtCv{
    inline QImage matToQImage(cv::Mat &image,QImage::Format format)
    {
        return QImage(image.data,image.cols,image.rows,image.step,format);
    }

    inline QImage matToQImageCopy(cv::Mat &image,QImage::Format format)
    {
        return QImage(image.data,image.cols,image.rows,image.step,format).copy();
    }

    inline cv::Mat qImageToMat(QImage &image, int format)
    {
        return cv::Mat(image.height(),image.width(),format,image.bits(),image.bytesPerLine());
    }

    inline cv::Mat qImageToMatCopy(QImage &image, int format)
    {
        return cv::Mat(image.height(),image.width(),format,image.bits(),image.bytesPerLine()).clone();
    }
}

#endif // QTOPENCVCONVERSIONS_HPP
