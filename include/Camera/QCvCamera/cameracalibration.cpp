#include "cameracalibration.h"

CameraCalibration::CameraCalibration() : flag(0), bMUST_INIT_UNDISTORT(true)
{
    bIS_CALIBRATED = false;
}

CameraCalibration::~CameraCalibration()
{

}

int CameraCalibration::addChessBoardPoints(filelist_vec &f, Size &boardSize)
{
    int successes = 0;
    // the points on the chessboard
    std::vector<cv::Point2f> imageCorners;
    std::vector<cv::Point3f> objectCorners;
   // 3D Scene Points:
   // Initialize the chessboard corners
   // in the chessboard reference frame
   // The corners are at 3D location (X,Y,Z)= (i,j,0)
   for (int i=0; i<boardSize.height; i++) {
     for (int j=0; j<boardSize.width; j++) {
       objectCorners.push_back(cv::Point3f((float)i, (float)j, 0.0f));
     }
   }

   // 2D Image points:
   cv::Mat image; // to contain chessboard image

   // for all viewpoints
   for (unsigned int i=0; i<f.size(); i++) {
       // Open the image
       image = cv::imread(f[i],0);
       calibrationImageSize = Size(image.rows,image.cols);
       cv::imshow("test",image);
       // Get the chessboard corners, this function can return a boolean for debugging
       bool found = cv::findChessboardCorners(image, boardSize, imageCorners);
       cv::drawChessboardCorners(image,boardSize, imageCorners,found); // corners have been found
       // Get subpixel accuracy on the corners
       cv::cornerSubPix(image, imageCorners,
                 cv::Size(9,5),
                 cv::Size(-1,-1),
       cv::TermCriteria(cv::TermCriteria::MAX_ITER +
                        cv::TermCriteria::EPS,
           30,      // max number of iterations
           0.1));  // min accuracy
      //If we have a good board, add it to our data
      if (imageCorners.size() == boardSize.area()) {
          // Add image and scene points from one view
          addPoints(imageCorners, objectCorners);
          successes++;
       }
    }
   return successes;
}

void CameraCalibration::addPoints(const vector<Point2f> &imageCorners, const vector<Point3f> &objectCorners)
{
    // 2D image points from one view
   imagePoints.push_back(imageCorners);
   // corresponding 3D scene points
   objectPoints.push_back(objectCorners);
}

double CameraCalibration::calibrate()
{
   // undistorter must be reinitialized
   bMUST_INIT_UNDISTORT= true;
   //Output rotations and translations
   vector<Mat> rvecs, tvecs;
   // start calibration
   bIS_CALIBRATED = true;
   return calibrateCamera(objectPoints, // the 3D points
                          imagePoints, // the image points
                          calibrationImageSize,   // image size
                          cameraMatrix,// output camera matrix
                          distCoeffs,  // output distortion matrix
                          rvecs, tvecs,// Rs, Ts
                          flag);       // set options
}

Mat CameraCalibration::remap(const Mat &image)
{
    cv::Mat undistorted;
    if (bIS_CALIBRATED)
    {
        if (bMUST_INIT_UNDISTORT) // called once per calibration
        {
             initUndistortRectifyMap(
               cameraMatrix,  // computed camera matrix
               distCoeffs,    // computed distortion matrix
               cv::Mat(),     // optional rectification (none)
               cv::Mat(),     // camera matrix to generate undistorted
                     image.size(),  // size of undistorted
                     CV_32FC1,      // type of output map
                     map1, map2);   // the x and y mapping functions
             bMUST_INIT_UNDISTORT = false;
        }
        // Apply mapping functions
        cv::remap(image, undistorted, map1, map2,
           cv::INTER_LINEAR); // interpolation type
    }
    else
    {
        undistorted = image;
    }

    return undistorted;
}

bool CameraCalibration::isCalibrated()
{
    return bIS_CALIBRATED;
}

bool CameraCalibration::loadCalibration()
{
    return false;
}

bool CameraCalibration::saveCalibration()
{
    return false;
}
