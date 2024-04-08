#ifndef CAMERACALIBRATION_H
#define CAMERACALIBRATION_H
#include <opencv2/core/core.hpp>
#include <opencv2/aruco.hpp>
#include "qglobal.h"
QT_BEGIN_NAMESPACE
using namespace cv;
using namespace std;
using namespace aruco;
QT_END_NAMESPACE

class CameraCalibration
{
public:
    CameraCalibration();
    void calcParams(QString dirPath);
private:
    bool saveCameraParams(const String &filename, const Mat &cameraMatrix, const Mat &distCoeffs);

    int cbWidth = 9; //Количество углов шахматной доски в ширину
    int cbHeight = 6; //Количество углов шахматной доски в высоту
    double cbSquareSize = 26; // размер стороны каждого квадрата в мм

};

#endif // CAMERACALIBRATION_H
