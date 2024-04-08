#ifndef GETFRAMETHREADRTSP_H
#define GETFRAMETHREADRTSP_H

#include <QObject>
#include <qvector.h>
#include <opencv2/aruco.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <qnetworkreply.h>
#include <telemetrywriter.h>
#include <controller.h>

class GetFrameThreadRTSP : public QObject
{
    Q_OBJECT

public:
    explicit GetFrameThreadRTSP(QObject *parent = 0);
    void initialize(QString url, float markerSize, cv::Mat cameraMatrix, cv::Mat distCoeffs);
private:
    cv::Mat matOriginal;
    cv::Mat outputImage;
    cv::VideoCapture capWebcam;
    cv::Ptr<cv::aruco::DetectorParameters>  parameters = cv::aruco::DetectorParameters::create();
    cv::Ptr<cv::aruco::Dictionary> dictionary;
    cv::Ptr<cv::aruco::Dictionary> customDictionary = generateCustomDict();
    cv::Ptr<cv::aruco::Dictionary> predefinedDictionary  = getPredefinedDictionary(cv::aruco::DICT_6X6_1000);
    vector<vector<cv::Point2f>> markerCorners, rejectedCandidates;
    cv::Mat cameraMatrix, distCoeffs;
    vector<int> markerIds;
    vector<cv::Vec3d> rvecs; //Вектор поворота
    vector<cv::Vec3d> tvecs; //Вектор Сдвига
    float markerSize = 0.0;
    cv::Ptr<cv::aruco::Dictionary> generateCustomDict();
signals:
    void finished();    // Сигнал, по которому будем завершать поток, после завершения метода run
    void sendImage(cv::Mat image,vector<int> markerIds, vector<cv::Vec3d> rvecs,vector<cv::Vec3d> tvecs,cv::Mat cameraMatrix,cv::Mat distCoeffs,vector<vector<cv::Point2f>> markerCorners);

public slots:
    void run(); // Метод с полезной нагрузкой, который может выполняться в цикле
};

#endif // GETFRAMETHREADRTSP_H
