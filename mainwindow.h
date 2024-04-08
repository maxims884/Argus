#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include <QThread>
#include <controller.h>
#include <getframethreadhttp.h>
#include <getframethreadrtsp.h>
#include <qelapsedtimer.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <opencv2/core/core.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/videoio.hpp>
#include <qpushbutton.h>
#include <udpspot.h>

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
using namespace cv;
using namespace std;
using namespace aruco;
QT_END_NAMESPACE

class MainWindow : public QDialog
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    //varsc -------------------------------------------------------------------------------------------------------
    Ui::Dialog *ui;

    Mat matOriginal;
    QTimer* tmrTimerReadCam;
    QTimer* tmrTimerFindMarker;
    QTimer* tmrTimerMakeScreenShoots;
    QTimer* tmrTimerWriteTelemetry;

    Mat outputImage;
    Mat outputImage2;

    vector<Vec3d> rvecs; //Вектор поворота
    vector<Vec3d> tvecs; //Вектор Сдвига

    vector<Vec3d> rvecs2; //Вектор поворота
    vector<Vec3d> tvecs2; //Вектор Сдвига

    vector<int> markerIds;
    vector<int> markerIds2;

    cv::Mat cameraMatrix, distCoeffs,cameraMatrix2, distCoeffs2;
    vector<vector<Point2f>> markerCorners, rejectedCandidates;
     vector<vector<Point2f>> markerCorners2, rejectedCandidates2;

    TelemetryWriter telemetryWriter;

    QThread * ffmpegThreadCam1;
    QThread * ffmpegProcessThreadCam1;
    QThread * ffmpegThreadCam2;
    QThread * ffmpegProcessThreadCam2;

    QThread cam1Thread;
    QThread cam2Thread;

    GetFrameThreadHTTP *getFrameAndProcessHTTP;
    GetFrameThreadHTTP *getFrameAndProcessHTTP2;

    GetFrameThreadRTSP *getFrameAndProcessRTSP;
    GetFrameThreadRTSP *getFrameAndProcessRTSP2;

    QNetworkRequest request;
    QNetworkAccessManager *nam;
    QNetworkRequest request2;
    QNetworkAccessManager *nam2;

    Controller *controller;
    int screenShootCounter = 0;

    //methods() ---------------------------------------------------------------------------------------------------
    void init();
    void setupSettingsUi();
    QString drawDestinationPath(QString markerId1,QString markerId2,std::string cameraNum);

    void showOutputData(Mat image,vector<Vec3d> rvectors,vector<Vec3d> tvectors,vector<int> ids, std::string cameraNum, bool isLabel1);

    void updateOutputParameters(cv::Mat image, vector<int> ids, vector<cv::Vec3d> rvectors,vector<cv::Vec3d> tvectors,cv::Mat matrix,cv::Mat coeffs,vector<vector<cv::Point2f>> corners);
    void updateOutputParameters2(cv::Mat image, vector<int> ids, vector<cv::Vec3d> rvectors,vector<cv::Vec3d> tvectors,cv::Mat matrix,cv::Mat coeffs,vector<vector<cv::Point2f>> corners);

    void downloadImageFinished(QNetworkReply *reply);
    void downloadImage2Finished(QNetworkReply *reply);

    void pairMarkersFound(QString markerId1,QString marker1Xyz, QString markerId2, QString marker2Xyz,std::string cameraNum);

    UdpSpot *spot;
    void sendToKobus(QString pitch, QString roll, QString dest, std::string cameraNum,QString ids);

    Point3f projectToDest(vector<Point3f> points,Vec3f rvec, Vec3f tvec,QList<Vec3f> tvecStandartList);

    double roundTo(double inpValue, int inpCount);
public slots:
    void processFrame();
    void updateUI();
    void makeScreenShootsForCalibration();
    void writeTelemetry();

private slots:
    void on_pushButtonGoToMain_clicked();
    void on_pushButtonSettings_clicked();
    void on_pushButtonCancel_clicked();
    void on_radioButton_toggled(bool checked);
    void on_radioButtonRTSP_toggled(bool checked);
};
#endif // MAINWINDOW_H
