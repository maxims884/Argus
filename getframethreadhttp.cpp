#include "getframethreadhttp.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "qdatetime.h"
#include <QImage>
#include <QUrl>
#include <iostream>

GetFrameThreadHTTP::GetFrameThreadHTTP(QObject *parent):
    QObject(parent)
{

}

void GetFrameThreadHTTP::initialize(float size, cv::Mat matrix, cv::Mat coeffs)
{
    this->markerSize = size;
    this->cameraMatrix = matrix;
    this->distCoeffs = coeffs;
    if(Controller::getInstance()->getUseCustomMarkers()){
        dictionary = customDictionary;
    } else {
        dictionary = predefinedDictionary;
    }
}

// Самый важный метод, в котором будет выполняться "полезная" работа объекта
void GetFrameThreadHTTP::run()
{
    QImage image;
    image.loadFromData(reply->readAll());
    matOriginal = cv::Mat(image.height(),image.width(),CV_8UC4, image.bits(), image.bytesPerLine());
    if(!matOriginal.empty())
    {
        cvtColor(matOriginal,outputImage,cv::COLOR_RGB2BGR);
        cvtColor(matOriginal,matOriginal,cv::COLOR_BGR2GRAY);
        cv::aruco::detectMarkers(matOriginal, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
        cv::aruco::drawDetectedMarkers(outputImage, markerCorners, markerIds);
        //число указывает размер стороны маркера в метрах
        cv::aruco::estimatePoseSingleMarkers(markerCorners, markerSize, cameraMatrix, distCoeffs, rvecs, tvecs);
    }
    emit sendImage(outputImage,markerIds,rvecs,tvecs,cameraMatrix,distCoeffs,markerCorners); // Высылаем данные, которые будут передаваться в другой поток
    emit finished();
}

void GetFrameThreadHTTP::setImageReply(QNetworkReply *networkReply){
    this->reply = networkReply;
}

cv::Ptr<cv::aruco::Dictionary> GetFrameThreadHTTP::generateCustomDict(){
    QString marker = Controller::getInstance()->getCustomMarker();
    int cMarkerSize = Controller::getInstance()->getCustomMarkerSize();
//    int cMarkerSizePixel = Controller::getInstance()->getCustomMarkerSizePixel();

    QStringList markerList = marker.split(";");
    cv::Ptr<cv::aruco::Dictionary> dict = cv::aruco::generateCustomDictionary(0,cMarkerSize,1)/*getPredefinedDictionary(cv::aruco::DICT_6X6_1000)*/;
    for(QString element:markerList)
    {
         bool bits[cMarkerSize][cMarkerSize];
         QStringList markerElementsList = element.split(",");
         for(int i = 0; i < markerElementsList.size(); ++i){
             for (int j = 0; j < markerElementsList.at(i).size(); ++j) {
                 bits[i][j] = markerElementsList.at(i).at(j).toLatin1() - '0';
             }
         }
         cv::Mat dictImage = cv::aruco::Dictionary::getByteListFromBits(cv::Mat(cMarkerSize,cMarkerSize,CV_8U,bits));
         dict->bytesList.push_back(dictImage);
    }

//    for(int i = 0; i < markerList.size(); i++)
//    {
//        cv::Mat arucoMarker;
//        drawMarker(customDictionary,i,cMarkerSizePixel,arucoMarker);
//        std::string str = "marker" + std::to_string(i);
//        imshow(str,arucoMarker);
//        cv::imwrite("/home/max/Рабочий стол/custom_marker/" + str + ".jpg", arucoMarker);
//    }
    return dict;
}
