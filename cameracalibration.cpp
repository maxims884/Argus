#include "cameracalibration.h"
#include "opencv2/calib3d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <QDir>
#include <qstring.h>
#include <opencv2/core/mat.hpp>

CameraCalibration::CameraCalibration()
{

}

//откалибровать и рассчитать параметры калибровки
void CameraCalibration::calcParams(QString dirPath){
    Mat img;
    QDir directory(dirPath);
    QStringList images = directory.entryList(QStringList() << "*.jpg",QDir::Files);
    Size chessSize(cbWidth,cbHeight);
    Size winSize(11,11);
    Size zeroZone(-1,-1);
    TermCriteria criteria(TermCriteria::COUNT + TermCriteria::EPS,30,0.001);

    vector< vector< Point3f > > objectPoints;
    vector< vector< Point2f > > imagePoints;
    vector< Point3f > obj;
        for (int i = 0; i < cbHeight; i++)
          for (int j = 0; j < cbWidth; j++)
            obj.push_back(Point3f(j * cbSquareSize, i * cbSquareSize, 0));
    foreach(QString fileName, images){
        img = imread(directory.filePath(fileName).toStdString());
        Mat gray = img.clone();
        cvtColor(img,gray,COLOR_BGR2GRAY);
        std::vector<Point2f> corners;
        bool ret = findChessboardCorners(gray,chessSize,corners, CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE);
        if(ret){
            try{
            cornerSubPix(gray,corners,winSize,zeroZone,criteria);
            drawChessboardCorners(img,chessSize,corners,ret);
            imshow("img",img);
            imagePoints.push_back(corners);
            objectPoints.push_back(obj);
            waitKey(100);
            } catch(Exception ex){
//                LOGI()<<ex.what();
            }
        }
    }
    Mat cameraMatrix,distCoef;
    vector< Mat > rvecs, tvecs;
    int flag = 0;
    flag |= CALIB_FIX_K4;
    flag |= CALIB_FIX_K5;
//    qDebug("Find chess done. Calibration start");
    calibrateCamera(objectPoints,imagePoints,img.size(),cameraMatrix,distCoef,rvecs,tvecs,flag);
    saveCameraParams(dirPath.toStdString() + "/calibration_params.txt",cameraMatrix,distCoef);
//    qDebug("Calibration done. Parameters saved at %s/calibration_params.txt", dirPath.toLocal8Bit().data());
}

// Сохраняем параметры камеры
bool CameraCalibration::saveCameraParams(const String &filename, const Mat &cameraMatrix, const Mat &distCoeffs)
{
    FileStorage fs;
    fs.open(filename, FileStorage::WRITE);
    if (!fs.isOpened())
        return false;

    fs << "camera_matrix" << cameraMatrix;
    fs << "distortion_coefficients" << distCoeffs;
    fs << "board_width" << cbWidth;
    fs << "board_height" << cbHeight;
    fs << "square_size" << cbSquareSize;


    return true;
}
