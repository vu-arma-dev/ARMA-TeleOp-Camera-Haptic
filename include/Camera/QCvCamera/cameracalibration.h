/* Camera Calibration Class
 * Jason Pile
 * Last Updated: 10/29/2014
 *
 * This class is intended to calibrate cameras through opencv. It is
 * a direct implementation of the code found in "Mastering Opencv
 * with Practical Computer Vision Projects" by Baggio (2012 version)
 * Please reference chapter 2 of this text along with the website
 * https://www.packtpub.com/books/content/opencv-estimating-projective-relations-images
 *
 *
 *
 */


#ifndef CAMERACALIBRATION_H
#define CAMERACALIBRATION_H

#include <opencv.hpp>
#include <highgui/highgui.hpp>
#include <calib3d/calib3d.hpp>
#include <imgproc/imgproc.hpp>
#include <stdlib.h>

using namespace std;
using namespace cv;

typedef std::vector<std::string> filelist_vec;

typedef std::vector<cv::Mat> calibrationImg_vec;

class CameraCalibration
{
public:
    CameraCalibration(); // default constructor
    ~CameraCalibration();
    int addChessBoardPoints(filelist_vec& f, Size & boardSize);
    void addPoints(const vector<Point2f>& imageCorners,
                   const vector<Point3f>& objectCorners);
    double calibrate();
    double calibrate(calibrationImg_vec I);
    Mat remap(const Mat &image);
    bool isCalibrated();
    bool loadCalibration();
    bool saveCalibration();

private:
    vector<vector<cv::Point3f>> objectPoints;
    vector<vector<cv::Point2f>> imagePoints;
    calibrationImg_vec loadCalibrationImages();
    Mat cameraMatrix;
    Mat distCoeffs;
    int flag;
    Mat map1,map2;
    bool bMUST_INIT_UNDISTORT;
    bool bIS_CALIBRATED;
    Size calibrationImageSize;
};

#endif // CAMERACALIBRATION_H
