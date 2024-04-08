#include "mainwindow.h"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"
#include "qdir.h"
#include "qjsonobject.h"
//#include "ui_dialog.h"
#include <QTimer>
#include <cameracalibration.h>
#include <qthread.h>
#include <math.h>
#include <ui_mainwindow.h>
#include <QProcess>
#include <ffmpegdecoder.h>
#include <processrtspframes.h>
#include <QtCore/qmath.h>
#include <cmath>
MainWindow::MainWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    controller = Controller::getInstance();
    init();
    spot = new UdpSpot(10089);

    telemetryWriter.init();

}

void MainWindow::init(){
    controller->readConfig();
    if(!controller->readCalibrationParams()) return;
    if(controller->getUseCustomMarkers()) {
        controller->readCustomMarkerConfig();
    }
    controller->readGeometryConfig();

    setupSettingsUi();
    //-------------------------------------------------------Попытки распаралеливания-------------------------------------------------------
    QString proto = controller->getProtocol();

    if(proto == "rtsp"){
        bool oldMethod = controller->getOldRtspMethod();
        if(oldMethod){
            getFrameAndProcessRTSP  = new GetFrameThreadRTSP();
//            getFrameAndProcessRTSP2 = new GetFrameThreadRTSP();

            getFrameAndProcessRTSP->initialize(controller->getUrl1(),controller->getMarkerSize(), controller->getCameraMatrix(), controller->getDistCoeffs());
//            getFrameAndProcessRTSP2->initialize(controller->getUrl2(),controller->getMarkerSize(), controller->getCameraMatrix(), controller->getDistCoeffs());

            getFrameAndProcessRTSP->moveToThread(&cam1Thread);
//            getFrameAndProcessRTSP2->moveToThread(&cam2Thread);


    //         Запуск выполнения метода run будет осуществляться по сигналу запуска от соответствующего потока
            connect(&cam1Thread, &QThread::started, getFrameAndProcessRTSP, &GetFrameThreadRTSP::run);
    //         Остановка потока же будет выполняться по сигналу finished от соответствующего объекта в потоке
            connect(getFrameAndProcessRTSP, &GetFrameThreadRTSP::finished, &cam1Thread, &QThread::terminate, Qt::DirectConnection);
    //         коннект для передачи данных из первого объекта в первом потоке, ко второму объекту во втором потоке
            connect(getFrameAndProcessRTSP, &GetFrameThreadRTSP::sendImage, this, &MainWindow::updateOutputParameters, Qt::DirectConnection);

//    //        -------------------------------camera2=------------------
//    //            Запуск выполнения метода run будет осуществляться по сигналу запуска от соответствующего потока
//            connect(&cam2Thread, &QThread::started, getFrameAndProcessRTSP2, &GetFrameThreadRTSP::run);
//    //         Остановка потока же будет выполняться по сигналу finished от соответствующего объекта в потоке
//            connect(getFrameAndProcessRTSP2, &GetFrameThreadRTSP::finished, &cam2Thread, &QThread::terminate, Qt::DirectConnection);
//    //         коннект для передачи данных из первого объекта в первом потоке, ко второму объекту во втором потоке
//            connect(getFrameAndProcessRTSP2, &GetFrameThreadRTSP::sendImage, this, &MainWindow::updateOutputParameters2, Qt::DirectConnection);
        } else {
            //----------------test decoder
            //camera1
            FFmpegDecoder *decoderCam1 = new FFmpegDecoder(controller->getUrl1().toStdString());
            decoderCam1->connect();
            if(decoderCam1->isConncected())
            {
                ffmpegThreadCam1 = new QThread();
                decoderCam1->moveToThread(ffmpegThreadCam1);

                ProcessRtspFrames * imgProc = new ProcessRtspFrames();
                imgProc->initialize(*decoderCam1,controller->getMarkerSize(), controller->getCameraMatrix(), controller->getDistCoeffs());
                ffmpegProcessThreadCam1 = new QThread();
                imgProc->moveToThread(ffmpegProcessThreadCam1);

                connect(ffmpegProcessThreadCam1, &QThread::started, imgProc, &ProcessRtspFrames::process);
                connect(ffmpegThreadCam1, &QThread::started, decoderCam1, &FFmpegDecoder::decode);
                connect(ffmpegProcessThreadCam1, &QThread::started, imgProc, &ProcessRtspFrames::process);

                connect(imgProc, &ProcessRtspFrames::sendImage, this, &MainWindow::updateOutputParameters, Qt::DirectConnection);
                //connect(&decoder, &FFmpegDecoder::finished, &ffmpegThread, &QThread::terminate, Qt::DirectConnection);
                ffmpegThreadCam1->start();
                ffmpegProcessThreadCam1->start();
                // коннект для передачи данных из первого объекта в первом потоке, ко второму объекту во втором потоке
                //connect(&imgProc, &ProcessRtspFrames::sendImage, this, &MainWindow::updateOutputParameters, Qt::DirectConnection);
            }
            //camera2

            FFmpegDecoder *decoderCam2 = new FFmpegDecoder(controller->getUrl2().toStdString());
            decoderCam2->connect();
            if(decoderCam2->isConncected())
            {
                ffmpegThreadCam2 = new QThread();
                decoderCam2->moveToThread(ffmpegThreadCam2);

                ProcessRtspFrames * imgProc = new ProcessRtspFrames();
                imgProc->initialize(*decoderCam2,controller->getMarkerSize(), controller->getCameraMatrix(), controller->getDistCoeffs());
                ffmpegProcessThreadCam2 = new QThread();
                imgProc->moveToThread(ffmpegProcessThreadCam2);

                connect(ffmpegProcessThreadCam2, &QThread::started, imgProc, &ProcessRtspFrames::process);
                connect(ffmpegThreadCam2, &QThread::started, decoderCam2, &FFmpegDecoder::decode);
                connect(ffmpegProcessThreadCam2, &QThread::started, imgProc, &ProcessRtspFrames::process);

                connect(imgProc, &ProcessRtspFrames::sendImage, this, &MainWindow::updateOutputParameters2, Qt::DirectConnection);
                //connect(&decoder, &FFmpegDecoder::finished, &ffmpegThread, &QThread::terminate, Qt::DirectConnection);
                ffmpegThreadCam2->start();
                ffmpegProcessThreadCam2->start();
                // коннект для передачи данных из первого объекта в первом потоке, ко второму объекту во втором потоке
                //connect(&imgProc, &ProcessRtspFrames::sendImage, this, &MainWindow::updateOutputParameters, Qt::DirectConnection);
            }

            //----------------end test decoder
        }
    }
    if(proto == "http") {
        getFrameAndProcessHTTP = new GetFrameThreadHTTP();
        getFrameAndProcessHTTP2 = new GetFrameThreadHTTP();

        getFrameAndProcessHTTP->initialize(controller->getMarkerSize(), controller->getCameraMatrix(), controller->getDistCoeffs());
        getFrameAndProcessHTTP2->initialize(controller->getMarkerSize(), controller->getCameraMatrix(), controller->getDistCoeffs());

        getFrameAndProcessHTTP->moveToThread(&cam1Thread);
        getFrameAndProcessHTTP2->moveToThread(&cam2Thread);
        //менеджер загрузки изображений http
        nam = new QNetworkAccessManager();
        connect(nam, &QNetworkAccessManager::finished, this, &MainWindow::downloadImageFinished);
        QUrl url1 = QUrl(controller->getUrl1());
        request.setUrl(url1);

        nam2 = new QNetworkAccessManager();
        connect(nam2, &QNetworkAccessManager::finished, this, &MainWindow::downloadImage2Finished);
        QUrl qUrl2 = QUrl(controller->getUrl2());
        request2.setUrl(qUrl2);
        //-------------------------------camera1------------------
        // Запуск выполнения метода run будет осуществляться по сигналу запуска от соответствующего потока
        connect(&cam1Thread, &QThread::started, getFrameAndProcessHTTP, &GetFrameThreadHTTP::run);
        // Остановка потока же будет выполняться по сигналу finished от соответствующего объекта в потоке
        connect(getFrameAndProcessHTTP, &GetFrameThreadHTTP::finished, &cam1Thread, &QThread::terminate, Qt::DirectConnection);
        // коннект для передачи данных из первого объекта в первом потоке, ко второму объекту во втором потоке
        connect(getFrameAndProcessHTTP, &GetFrameThreadHTTP::sendImage, this, &MainWindow::updateOutputParameters, Qt::DirectConnection);
        //-------------------------------camera2=------------------
           // Запуск выполнения метода run будет осуществляться по сигналу запуска от соответствующего потока
        connect(&cam2Thread, &QThread::started, getFrameAndProcessHTTP2, &GetFrameThreadHTTP::run);
        // Остановка потока же будет выполняться по сигналу finished от соответствующего объекта в потоке
        connect(getFrameAndProcessHTTP2, &GetFrameThreadHTTP::finished, &cam2Thread, &QThread::terminate, Qt::DirectConnection);
        // коннект для передачи данных из первого объекта в первом потоке, ко второму объекту во втором потоке
        connect(getFrameAndProcessHTTP2, &GetFrameThreadHTTP::sendImage, this, &MainWindow::updateOutputParameters2, Qt::DirectConnection);
    }
    //----------------------------------------------------------------------------------------------------------------------------------------
    // Калибровка камеры
    if(controller->getCameraCalibrationOnStatus()){
        CameraCalibration cameraCalibration;
        cameraCalibration.calcParams(controller->getScreenshotPath() + "/camera1/");
    }

    QString buildInfo = QString::fromStdString(getBuildInformation());
    ui->plainTextEdit->appendPlainText(buildInfo);

    //Чтение с камеры
    tmrTimerReadCam = new QTimer(this);
    connect(tmrTimerReadCam,SIGNAL(timeout()),this, SLOT(processFrame()));
    tmrTimerReadCam->start(1000/controller->getCameraFps());

    // Обновление вьюшки
    tmrTimerFindMarker = new QTimer(this);
    connect(tmrTimerFindMarker,SIGNAL(timeout()),this, SLOT(updateUI()));
    tmrTimerFindMarker->start(controller->getUpdateUiMs());

    //запись в файл каждые n минут
    tmrTimerWriteTelemetry = new QTimer(this);
    connect(tmrTimerWriteTelemetry,SIGNAL(timeout()),this, SLOT(writeTelemetry()));
    tmrTimerWriteTelemetry->start(controller->getWriteTelemetryMs());

    //таймер деланья скриншотов
    if(controller->getScreenShootsOnStatus()){
        tmrTimerMakeScreenShoots = new QTimer(this);
        connect(tmrTimerMakeScreenShoots,SIGNAL(timeout()),this, SLOT(makeScreenShootsForCalibration()));
        tmrTimerMakeScreenShoots->start(1000);
        LOGI() << "timer start";
    }

    if(!controller->getConsoleMode()) show();
}

void MainWindow::downloadImageFinished(QNetworkReply *reply){
    getFrameAndProcessHTTP->setImageReply(reply);
    cam1Thread.start();
}

void MainWindow::downloadImage2Finished(QNetworkReply *reply){
    getFrameAndProcessHTTP2->setImageReply(reply);
    cam2Thread.start();
}

void MainWindow::processFrame()
{
    QString proto = controller->getProtocol();
    bool oldMethod = controller->getOldRtspMethod();
    if(proto == "rtsp" && oldMethod){
        cam1Thread.start();
        cam2Thread.start();
    }

    if(proto == "http"){
        nam->get(request);
        nam2->get(request2);
    }
}

void MainWindow::makeScreenShootsForCalibration(){
    Mat mat1 = outputImage;
    Mat mat2 = outputImage2;
    QString path(controller->getScreenshotPath());
    QString name = "test" + QString(screenShootCounter);
    QString format(".jpg");
    QString dest1 = "/camera1/";
    QString dest2 = "/camera2/";

    QString res1 = QString(path + dest1 + name + format);
    QString res2 = QString(path + dest2 + name + format);
    LOGI() << QString(path + dest1 + name + format);
    if(!mat1.empty()) imwrite(res1.toStdString(), mat1);
    if(!mat2.empty()) imwrite(res2.toStdString(), mat2);

    screenShootCounter++;
}

void MainWindow::writeTelemetry()
{
    telemetryWriter.writeToFile();
}

MainWindow::~MainWindow()
{
//    TRACELOG;
    delete ui;
}

std::mutex mu;
std::mutex mu2;

void MainWindow::updateUI(){
    mu.lock();
    showOutputData(outputImage,rvecs,tvecs,markerIds,"CAMERA1",true);
    mu.unlock();

    mu2.lock();
    showOutputData(outputImage2,rvecs2,tvecs2,markerIds2,"CAMERA2",false);
    mu2.unlock();
}

void MainWindow::showOutputData(Mat image,vector<Vec3d> rvectors,vector<Vec3d> tvectors,vector<int> ids, std::string cameraNum, bool isLabel1){
    //sendToKobus("90","45","90,100,110","1");
    QTime ct = QTime::currentTime();
    if(!image.empty())
    {
//        вывод координат позиции маркеров
        if(!rvectors.empty()&&!tvectors.empty()&&!ids.empty()){
            QHash<int,QString> pairHsh = Controller::getInstance()->getIsDetectPairHash();
            QStringList markersForDetect = Controller::getInstance()->getPairDetectionMarkers();
            LOGI() << "---------------------------------------------------------------------";
            ui->plainTextEdit->appendPlainText("---------------------------------------------------------------------");
            for (size_t i = 0; i < rvectors.size(); ++i) {
                auto rvec = rvectors[i];
                auto tvec = tvectors[i];
                auto markerId = ids[i];
                auto x = tvec[0] * 100;
                auto y = tvec[1] * 100;
                auto z = tvec[2] * 100;

                string xText = format("%4.f",x);
                string yText = format("%4.f",y);
                string zText = format("%4.f",z);
                QString markerXyz = QString::number(x) + " " + QString::number(y) + " " + QString::number(z);

                QString outputData(QString::fromStdString(cameraNum + " Time:" + ct.toString().toStdString() + " ID:" + std::to_string(markerId) + " X:" + xText + "   Y:"+ yText + "   Z:"+ zText) + "\n");
                LOGD () << outputData;
                telemetryWriter.appendCoordsCollectedData(outputData + "\n");
                ui->plainTextEdit->appendPlainText(outputData);
                pairHsh.insert(markerId,markerXyz);

//углы Эйлера
//                Mat matRodrigues;
//                Rodrigues(rvec,matRodrigues);
//                Mat pose_mat;
//                Mat a,s,d,f,g,h,euler;
//                hconcat(matRodrigues,tvec,pose_mat);

//                decomposeProjectionMatrix(pose_mat,a,s,d,f,g,h,euler);

//                LOGI() << "EULER: OZY & AB" << euler.at<double>(0,0);
//                LOGI() << "EULER: OXY & AB" << euler.at<double>(0,1);
//                LOGI() << "EULER: OZX & AB" << euler.at<double>(0,2);
            }
            for(int i = 0; i<markersForDetect.size() - 1; i+=2){

                    auto iElem = markersForDetect.at(i);
                    auto iElem2 = markersForDetect.at(i + 1);
                    auto valueIElem = pairHsh.value(iElem.toInt());
                    auto valueIElem2 = pairHsh.value(iElem2.toInt());

                    if(!valueIElem.isEmpty() && !valueIElem2.isEmpty()){
                         pairMarkersFound(iElem,valueIElem,iElem2,valueIElem2,cameraNum);
                    }
            }
            Controller::getInstance()->clearDetectPairHash();
        }


        QImage qimgOriginal(image.data,image.cols,image.rows,image.step,QImage::Format_RGB888);

        //Эти размеры только для того чтобы изображение помещалось в окно программы
        auto inputWidth = 600;
        auto inputHeight = 323;

        QImage small = qimgOriginal.scaled(inputWidth, inputHeight,Qt::KeepAspectRatio);

        if(isLabel1){
            ui->label->setAlignment(Qt::AlignLeft);
            QPixmap pix = QPixmap::fromImage(small);
            ui->label->setPixmap(pix);
        } else
        {
            ui->label_2->setAlignment(Qt::AlignLeft);
            QPixmap pix = QPixmap::fromImage(small);
            ui->label_2->setPixmap(pix);
        }
    }
}

void MainWindow::updateOutputParameters(cv::Mat image, vector<int> ids, vector<cv::Vec3d> rvectors,vector<cv::Vec3d> tvectors,cv::Mat matrix,cv::Mat coeffs,vector<vector<cv::Point2f>> corners){
    mu.lock();
    if(controller->getProtocol() == "rtsp") cvtColor(image, image,cv::COLOR_BGR2RGB);
    this->outputImage = image;
    this->markerIds = ids;
    this->rvecs = rvectors;
    this->tvecs = tvectors;
    this->cameraMatrix = matrix;
    this->distCoeffs = coeffs;
    this->markerCorners = corners;
    telemetryWriter.increaseCamera1ProcessedFrameCount();
    telemetryWriter.setCamera1DetectedMarkers(markerIds);
    mu.unlock();
}

void MainWindow::updateOutputParameters2(cv::Mat image, vector<int> ids, vector<cv::Vec3d> rvectors,vector<cv::Vec3d> tvectors,cv::Mat matrix,cv::Mat coeffs,vector<vector<cv::Point2f>> corners){
    mu2.lock();
    if(controller->getProtocol() == "rtsp") cvtColor(image, image,cv::COLOR_BGR2RGB);
    this->outputImage2 = image;
    this->markerIds2 = ids;
    this->rvecs2 = rvectors;
    this->tvecs2 = tvectors;
    this->cameraMatrix2 = matrix;
    this->distCoeffs2 = coeffs;
    this->markerCorners2 = corners;
    telemetryWriter.increaseCamera2ProcessedFrameCount();
    telemetryWriter.setCamera2DetectedMarkers(markerIds);
    mu2.unlock();
}

void MainWindow::pairMarkersFound(QString markerId1,QString marker1Xyz, QString markerId2, QString marker2Xyz,std::string cameraNum){
    LOGD()<< "PAIR MARKERS FOUND: " << markerId1 << markerId2;
    auto markersIds = markerId1 + " " + markerId2;

    ui->plainTextEdit->appendPlainText("PAIR MARKERS FOUND: " + markerId1 + " " + markerId2);
    QStringList listValues1 = marker1Xyz.split(' ');
    QStringList listValues2 = marker2Xyz.split(' ');

    auto Ax = listValues1[0].toFloat()/1000;
    auto Ay = listValues1[1].toFloat()/1000;
    auto Az = listValues1[2].toFloat()/1000;

    auto Bx = listValues2[0].toFloat()/1000;
    auto By = listValues2[1].toFloat()/1000;
    auto Bz = listValues2[2].toFloat()/1000;

    int roundVal = 10000;
    Ax = round(Ax*roundVal)/roundVal;
    Ay = round(Ay*roundVal)/roundVal;
    Az = round(Az*roundVal)/roundVal;

    Bx = round(Bx*roundVal)/roundVal;
    By = round(By*roundVal)/roundVal;
    Bz = round(Bz*roundVal)/roundVal;


    LOGD () << "Ax " << Ax;

    auto AngleOxy = abs((atan2(By-Ay,Bx-Ax) * 180 / 3.14) - 180);
    auto AngleOzy = abs(atan2(By-Ay,Bz-Az) * 180 / 3.14);
    auto AngleOzx = abs(atan2(Bx-Ax,Bz-Az) * 180 / 3.14);

    LOGD () << "ANGLE BETWEEN OXY & AB " << AngleOxy;
    ui->plainTextEdit->appendPlainText("ANGLE BETWEEN OXY & AB " + QString::number(AngleOxy));
    LOGD () << "ANGLE BETWEEN OZY & AB " << AngleOzy;
    ui->plainTextEdit->appendPlainText("ANGLE BETWEEN OZY & AB " + QString::number(AngleOzy));
    LOGD () << "ANGLE BETWEEN OZX & AB " << AngleOzx;
    ui->plainTextEdit->appendPlainText("ANGLE BETWEEN OZX & AB " + QString::number(AngleOzx));

    //-----------------------------------------------------------------------------FOR DELETE 2 markers
//    QString dest(QString::number(Ax) + " " + QString::number(Ay) + " " + QString::number(Az) + " " + QString::number(Bx) + " " + QString::number(By) + " " + QString::number(Bz));
    //---------------------------------------------------------------------------------------------------------
    QString dest = drawDestinationPath(markerId1,markerId2,cameraNum);
    sendToKobus(QString::number(AngleOxy),QString::number(AngleOzy),dest,cameraNum,markersIds);
}

QString MainWindow::drawDestinationPath(QString markerId1, QString markerId2,std::string cameraNum)
{
    vector< Point2f > imagePoints1;
    vector< Point2f > imagePoints2;
    QString dest = "";
    if(!rvecs.empty()&&!tvecs.empty()){
        Vec3f rvec1;
        Vec3f rvec2;
        Vec3f tvec1;
        Vec3f tvec2;
        for (size_t i = 0; i < rvecs.size(); ++i) {
            auto rvec = rvecs[i];
            auto tvec = tvecs[i];
            auto markerId = markerIds[i];

            if(markerId1.toInt() == markerId){
                rvec1 = rvec;
                tvec1 = tvec;
            }

            if(markerId2.toInt() == markerId){
                rvec2 = rvec;
                tvec2 = tvec;
            }
        }

        QList<Vec3f> tvecStandartList;
        QStringList standartMarkerlist = controller->getStandartMarkers();
        QStringList standartMarkerCoordslist = controller->getStandartMarkersCoordinates();

        for(int j = 0; j < standartMarkerlist.size();j++){
            for (size_t i = 0; i < rvecs.size(); ++i) {
                auto rvec = rvecs[i];
                auto tvec = tvecs[i];
                auto markerId = markerIds[i];

                if(standartMarkerlist.at(j).toInt() == markerId){
                    QStringList coords = standartMarkerCoordslist.at(j).split(" ");
                    Vec3f coordsVecStandart = Vec3f(coords.at(0).toFloat(),coords.at(1).toFloat(),coords.at(2).toFloat());

                    Vec3f diff = static_cast<Vec3f>(tvec) - coordsVecStandart;
                    tvecStandartList.append(diff);
                }
            }
        }

        float distBetweenMarkers = abs(sqrt((tvec2[0] - tvec1[0]) * (tvec2[0] - tvec1[0]) + (tvec2[1] - tvec1[1]) * (tvec2[1] - tvec1[1]) + (tvec2[2] - tvec1[2]) * (tvec2[2] - tvec1[2])));

        QString geometry = controller->getGeometry();

        QStringList listPoints = geometry.split(",");

        vector< Point3f > points1;
        vector< Point3f > points2;

        for(int i =0; i<listPoints.size(); i++){
            Point3f point;
            QStringList pointCoords = listPoints[i].split(" ");
            point.x = pointCoords[0].toFloat();
            point.y = pointCoords[1].toFloat();
            point.z = pointCoords[2].toFloat();
            points2.push_back(point);

            point.y = pointCoords[1].toFloat() - distBetweenMarkers;
            points1.push_back(point);
        }

        projectPoints(points1, rvec1, tvec1, cameraMatrix, distCoeffs, imagePoints1);
        projectPoints(points2, rvec2, tvec2, cameraMatrix, distCoeffs, imagePoints2);

        Point3f marker1Point = projectToDest(points1,rvec1,tvec1,tvecStandartList);
        Point3f marker2Point = projectToDest(points2,rvec2,tvec2,tvecStandartList);

//        LOGD () << "dist betwweeen : " << distBetweenMarkers;
//        LOGD () << "marker1Point Dest   X: " << marker1Point.x << " Y: " << marker1Point.y << " Z: " << marker1Point.z;
//        LOGD () << "marker2Point Dest   X: " << marker2Point.x << " Y: " << marker2Point.y << " Z: " << marker2Point.z;


        Point3f destPoint = Point3f((marker1Point.x + marker2Point.x)/2,(marker1Point.y + marker2Point.y)/2,(marker1Point.z + marker2Point.z)/2);

        dest = QString::number(destPoint.x/10) + " " + QString::number(destPoint.y/10) + " " + QString::number(destPoint.z/10);
        LOGD () << "DEST  " << dest;
         ui->plainTextEdit->appendPlainText("DEST  " + dest);

        if(controller->getDrawDestPath()){
            if(cameraNum == "CAMERA1"){
                for(int i =0; i<listPoints.size() - 1; i++){
                    line(outputImage, imagePoints1[i], imagePoints1[i+1], Scalar(0, 0, 255), 5);

                    line(outputImage, imagePoints2[i], imagePoints2[i+1], Scalar(0, 0, 255), 5);
                }
            }
            else {
                for(int i =0; i<listPoints.size() - 1; i++){
                    line(outputImage2, imagePoints1[i], imagePoints1[i+1], Scalar(0, 0, 255), 5);

                    line(outputImage2, imagePoints2[i], imagePoints2[i+1], Scalar(0, 0, 255), 5);
                }
            }
        }
    }
    return dest;
}

Point3f MainWindow::projectToDest(vector<Point3f> points,Vec3f rvec, Vec3f tvec,QList<Vec3f> tvecStandartList){
    Mat rot_mat;
    Rodrigues(rvec, rot_mat);

    Point3f lastPointInGeometry = points.back();

//    LOGD () << "last point in geometry " << lastPointInGeometry.x<< " y "<< lastPointInGeometry.y  << " z " << lastPointInGeometry.z;

    Point3f tvec_3f(tvec[0], tvec[1], tvec[2]);
//    LOGD () << "tvec_3f " << tvec_3f.x<< " y "<< tvec_3f.y  << " z " << tvec_3f.z;

    Point3f rvec_3fMat(rot_mat.at<float>(0,0) * lastPointInGeometry.x + rot_mat.at<float>(0,1) * lastPointInGeometry.y + rot_mat.at<float>(0,2) * lastPointInGeometry.z,
                       rot_mat.at<float>(1,0) * lastPointInGeometry.x + rot_mat.at<float>(1,1) * lastPointInGeometry.y + rot_mat.at<float>(1,2) * lastPointInGeometry.z,
                       rot_mat.at<float>(2,0) * lastPointInGeometry.x + rot_mat.at<float>(2,1) * lastPointInGeometry.y + rot_mat.at<float>(2,2) * lastPointInGeometry.z);

//    LOGD () << "rvec_3fMat " << rvec_3fMat.x<< " y "<< rvec_3fMat.y  << " z " << rvec_3fMat.z;

    if(!controller->getUseStandartMarkers()){
        Point3f res = tvec_3f + rvec_3fMat;
//        LOGD () << "projectToDest x " << res.x<< " y "<< res.y  << " z " << res.z;
        return res;
    }else {
        Point3f diffRes = Point3f(0,0,0);
        Point3f diffResSum = Point3f(0,0,0);
        if(tvecStandartList.size() > 0) {
            for(int i = 0; i < tvecStandartList.size(); i++) {
    //            LOGD () << "tvecStandartList x" << tvecStandartList.at(i)[0];
    //            LOGD () << "tvecStandartList y" << tvecStandartList.at(i)[1];
    //            LOGD () << "tvecStandartList z" << tvecStandartList.at(i)[2];
                diffRes = tvec_3f - static_cast<Point3f>(tvecStandartList.at(i));
    //            LOGD () << "tvec_3f x" << tvec_3f.x;
    //            LOGD () << "tvec_3f y" << tvec_3f.y;
    //            LOGD () << "tvec_3f z" << tvec_3f.z;
                diffResSum = diffResSum + diffRes ;
            }
            diffResSum = diffResSum / tvecStandartList.size();
            diffResSum = diffResSum + rvec_3fMat;
        }
//        LOGD () << "projectToDest x with standart" << diffResSum.x<< " y "<< diffResSum.y  << " z " << diffResSum.z;
        return diffResSum;
    }
}

void MainWindow::sendToKobus(QString pitch, QString roll, QString dest, std::string cameraNum,QString ids){
   QJsonObject json;
   QTime ct = QTime::currentTime();
   json.insert("camera",QString::fromStdString(cameraNum));
   json.insert("ids",ids);
   json.insert("time",ct.toString());
   json.insert("pitch",pitch);
   json.insert("roll",roll);
   json.insert("dest",dest);
   spot->sendJson(json);
}

void MainWindow::on_pushButtonGoToMain_clicked()
{
    if(ui->radioButtonRTSP->isChecked())controller->setProtocol("rtsp");
            else controller->setProtocol("http");

    controller->setUrl1(ui->plainTextEditCamera1->document()->toPlainText());
    controller->setUrl2(ui->plainTextEditCamera2->document()->toPlainText());
    controller->setCameraFps(ui->spinBoxFPS->value());
    controller->setConsoleMode(ui->checkBoxConsole->isChecked());
    controller->setKobusIp(ui->plainTextEditIp->document()->toPlainText());
    controller->setMarkerSize(ui->doubleSpinBoxMarkerSize->value());
    controller->setPairDetectionMarkers(ui->plainTextEditPairs->document()->toPlainText());
    controller->setUpdateUiMs(ui->spinBoxUiMs->value());
    controller->setUseCustomMarkers(ui->checkBoxCustomMarkers->isChecked());
    controller->setWriteTelemetryMs(ui->spinBoxWriteTelemetry->value());
    controller->setDrawDestPath(ui->checkBoxDestPath->isChecked());
    controller->setOldRtspMethod(ui->checkBoxOldRtsp->isChecked());

    controller->setScreenShootsOnStatus(ui->checkBoxMakeScrennshots->isChecked());
    controller->setScreenshotPath(ui->plainTextEditMakeScreenshotsPath->document()->toPlainText());

    controller->setUseStandartMarkers(ui->checkBoxUseStandart->isChecked());
//    controller->setStandartMarkers(ui->plainTextEditListStandart->document()->toPlainText());
//    controller->setStandartMarkersCoordinates(ui->plainTextEditStandartCoord->document()->toPlainText());
    if(ui->plainTextEditListStandart->document()->toPlainText().split(" ").size() == ui->plainTextEditStandartCoord->document()->toPlainText().split(",").size()){
        controller->setStandartMarkers(ui->plainTextEditListStandart->document()->toPlainText());
        controller->setStandartMarkersCoordinates(ui->plainTextEditStandartCoord->document()->toPlainText());
    }
    else {
        LOGE() << "ERROR!!! standart_markers_coordinates_size != standart_markers_coordinates_size";
        QStringList markers = controller->getStandartMarkers();
        QString markersRes = "";
        for(int i=0; i<markers.size() - 1; i++) {
            markersRes += markers.at(i);
            markersRes += " ";
        }
        if(markers.size() > 0) markersRes += markers.at(markers.size()-1);

        QStringList coords = controller->getStandartMarkersCoordinates();
        QString coordsRes = "";
        for(int i=0; i<coords.size() - 1; i++) {
            coordsRes += coords.at(i);
            coordsRes += ",";
        }
        if(coords.size() > 0) coordsRes += coords.at(coords.size()-1);

        controller->setStandartMarkers(markersRes);
        controller->setStandartMarkersCoordinates(coordsRes);
    }

    controller->setCameraCalibrationOnStatus(ui->checkBoxCameraCalib->isChecked());
    controller->setCalibrationParamPath(ui->plainTextEditCalibParamsPath->document()->toPlainText());

    controller->setCustomMarkerSize(ui->spinBoxCustomMarkerSize->value());
    controller->setCustomMarkerSizePixel(ui->spinBoxCustomMarkerPixelCount->value());
    controller->setCustomMarker(ui->plainTextEditCustomMarkers->document()->toPlainText());

    controller->setGeometry(ui->plainTextEditMainGeometry->document()->toPlainText());

    controller->updateAllCongigs();
//    ui->stackedWidget->setCurrentIndex(0);
    // restart app:
    qApp->quit();
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}

void MainWindow::on_pushButtonSettings_clicked()
{
     setupSettingsUi();
     ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButtonCancel_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::setupSettingsUi(){
    QString proto = controller->getProtocol();
    if(proto == "rtsp"){
         ui->radioButtonRTSP->setChecked(true);
    } else {
         ui->radioButtonHTTP->setChecked(true);
    }
    ui->plainTextEditCamera1->document()->setPlainText(controller->getUrl1());
    ui->plainTextEditCamera2->document()->setPlainText(controller->getUrl2());
    ui->spinBoxFPS->setValue(controller->getCameraFps());
    ui->checkBoxConsole->setChecked(controller->getConsoleMode());
    ui->plainTextEditIp->document()->setPlainText(controller->getKobusIp());
    ui->doubleSpinBoxMarkerSize->setValue(controller->getMarkerSize());

    QString pairDetectionMarkers = "";
    for(int i = 0; i < controller->getPairDetectionMarkers().size(); i++){
        pairDetectionMarkers.append(controller->getPairDetectionMarkers().at(i));
        pairDetectionMarkers.append(" ");
    }
    if(pairDetectionMarkers.at(pairDetectionMarkers.size()-1) == " ")
        pairDetectionMarkers.remove(pairDetectionMarkers.size() - 1,1);
    ui->plainTextEditPairs->document()->setPlainText(pairDetectionMarkers);

    ui->spinBoxUiMs->setValue(controller->getUpdateUiMs());
    ui->checkBoxCustomMarkers->setChecked(controller->getUseCustomMarkers());
    ui->spinBoxWriteTelemetry->setValue(controller->getWriteTelemetryMs());
    ui->checkBoxDestPath->setChecked(controller->getDrawDestPath());
    ui->checkBoxOldRtsp->setChecked(controller->getOldRtspMethod());

    ui->checkBoxMakeScrennshots->setChecked(controller->getScreenShootsOnStatus());
    ui->plainTextEditMakeScreenshotsPath->document()->setPlainText(controller->getScreenshotPath());

    ui->checkBoxUseStandart->setChecked(controller->getUseStandartMarkers());
    QString standartMarkers = "";
    for(int i = 0; i < controller->getStandartMarkers().size(); i++){
        standartMarkers.append(controller->getStandartMarkers().at(i));
        standartMarkers.append(" ");
    }
    if(standartMarkers.at(standartMarkers.size()-1) == " ")
        standartMarkers.remove(standartMarkers.size() - 1,1);
    ui->plainTextEditListStandart->document()->setPlainText(standartMarkers);

    QString standartMarkersCoordinates = "";
    for(int i = 0; i < controller->getStandartMarkersCoordinates().size(); i++){
        standartMarkersCoordinates.append(controller->getStandartMarkersCoordinates().at(i));
        standartMarkersCoordinates.append(",");
    }
    if(standartMarkersCoordinates.at(standartMarkersCoordinates.size()-1) == ",")
        standartMarkersCoordinates.remove(standartMarkersCoordinates.size() - 1,1);
    ui->plainTextEditStandartCoord->document()->setPlainText(standartMarkersCoordinates);

    ui->checkBoxCameraCalib->setChecked(controller->getCameraCalibrationOnStatus());
    ui->plainTextEditCalibParamsPath->document()->setPlainText(controller->getCalibrationParamPath());

    ui->spinBoxCustomMarkerSize->setValue(controller->getCustomMarkerSize());
    ui->spinBoxCustomMarkerPixelCount->setValue(controller->getCustomMarkerSizePixel());
    ui->plainTextEditCustomMarkers->document()->setPlainText(controller->getCustomMarker());

    ui->plainTextEditMainGeometry->document()->setPlainText(controller->getGeometry());
}

void MainWindow::on_radioButtonRTSP_toggled(bool checked)
{

}

void MainWindow::on_radioButton_toggled(bool checked)
{

}

double MainWindow::roundTo(double inpValue, int inpCount)
{
    double outpValue;
    double tempVal;
    tempVal=inpValue*pow(10,inpCount);
    if (double(int(tempVal))+0.5==tempVal)
    {
        if (int(tempVal)%2==0)
            {outpValue=double(qFloor(tempVal))/pow(10,inpCount);}
        else
            {outpValue=double(qCeil(tempVal))/pow(10,inpCount);}
    }
    else
    {
        if (double(int(tempVal))+0.5>tempVal)
            {outpValue=double(qCeil(tempVal))/pow(10,inpCount);}
        else
            {outpValue=double(qFloor(tempVal))/pow(10,inpCount);}
    }
    return(outpValue);
}
