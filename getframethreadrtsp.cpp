#include "getframethreadrtsp.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "qdatetime.h"
#include <QImage>
#include <QUrl>


GetFrameThreadRTSP::GetFrameThreadRTSP(QObject *parent):
    QObject(parent)
{

}

void GetFrameThreadRTSP::initialize(QString url, float size, cv::Mat matrix, cv::Mat coeffs)
{
//    capWebcam.open(url.toStdString(),cv::CAP_FFMPEG);
    capWebcam.open(0);
    if(capWebcam.isOpened() == false)
    {
        LOGE() << "vebCam error";
    }
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
void GetFrameThreadRTSP::run()
{
    capWebcam.read(matOriginal);
//    cv::rotate(matOriginal,matOriginal, cv::ROTATE_180);
//    TRACELOG;
    if(!matOriginal.empty())
    {
        outputImage = matOriginal.clone();
        cvtColor(matOriginal,matOriginal,cv::COLOR_BGR2GRAY);
        cv::aruco::detectMarkers(matOriginal, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
        cv::aruco::drawDetectedMarkers(outputImage, markerCorners, markerIds);
        //число указывает размер стороны маркера в метрах
        cv::aruco::estimatePoseSingleMarkers(markerCorners, markerSize, cameraMatrix, distCoeffs, rvecs, tvecs);
    }
    emit sendImage(outputImage,markerIds,rvecs,tvecs,cameraMatrix,distCoeffs,markerCorners); // Высылаем данные, которые будут передаваться в другой поток
    emit finished();
}

cv::Ptr<cv::aruco::Dictionary> GetFrameThreadRTSP::generateCustomDict(){
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
