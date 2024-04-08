#ifndef PROCESSRTSPFRAMES_H
#define PROCESSRTSPFRAMES_H

#include <ffmpegdecoder.h>
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


class ProcessRtspFrames : public QObject
{
        Q_OBJECT
public:
    explicit ProcessRtspFrames(QObject *parent = 0);
    ~ProcessRtspFrames(){};
    void initialize(FFmpegDecoder &d, float markerSize, cv::Mat cameraMatrix, cv::Mat distCoeffs);

signals:
    void sendImage(cv::Mat image,vector<int> markerIds, vector<cv::Vec3d> rvecs,vector<cv::Vec3d> tvecs,cv::Mat cameraMatrix,cv::Mat distCoeffs,vector<vector<cv::Point2f>> markerCorners);
public slots:
    void process();

private:
    cv::Mat outputImage;
    cv::Mat matOriginal;
    vector<vector<cv::Point2f>> markerCorners, rejectedCandidates;
    cv::Mat cameraMatrix, distCoeffs;
    vector<int> markerIds;
    vector<cv::Vec3d> rvecs; //Вектор поворота
    vector<cv::Vec3d> tvecs; //Вектор Сдвига
    cv::Ptr<cv::aruco::DetectorParameters>  parameters = cv::aruco::DetectorParameters::create();
    cv::Ptr<cv::aruco::Dictionary> dictionary;
    cv::Ptr<cv::aruco::Dictionary> customDictionary = generateCustomDict();
    cv::Ptr<cv::aruco::Dictionary> predefinedDictionary  = getPredefinedDictionary(cv::aruco::DICT_6X6_1000);
    float markerSize = 0.0;
    cv::Ptr<cv::aruco::Dictionary> generateCustomDict();
    FFmpegDecoder *decoder;


};

#endif // PROCESSRTSPFRAMES_H
