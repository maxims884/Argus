#include "processrtspframes.h"
#include <qtimer.h>


ProcessRtspFrames::ProcessRtspFrames(QObject *parent):
    QObject(parent)
{

}

void ProcessRtspFrames::initialize(FFmpegDecoder &d,float size, cv::Mat matrix, cv::Mat coeffs)
{
    this->markerSize = size;
    this->cameraMatrix = matrix;
    this->distCoeffs = coeffs;
    this->decoder = &d;

    if(Controller::getInstance()->getUseCustomMarkers()){
        dictionary = customDictionary;
    } else {
        dictionary = predefinedDictionary;
    }
}

void ProcessRtspFrames::process()
{
    while(decoder->isConncected())
    {
        decoder->mtx.lock();
        if(!decoder->decodedImgBuf.empty())
        {
            matOriginal = decoder->decodedImgBuf.front().clone();
            decoder->decodedImgBuf.pop_front();
           // std::cout <<"img buffer size = "<< decoder.decodedImgBuf.size() << std::endl;
        }
        decoder->mtx.unlock();

        if(!matOriginal.empty())
        {
            outputImage = matOriginal.clone();
            cvtColor(matOriginal,matOriginal,cv::COLOR_BGR2GRAY);
            cv::aruco::detectMarkers(matOriginal, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
            cv::aruco::drawDetectedMarkers(outputImage, markerCorners, markerIds);
            //число указывает размер стороны маркера в метрах
            cv::aruco::estimatePoseSingleMarkers(markerCorners, markerSize, cameraMatrix, distCoeffs, rvecs, tvecs);

            emit sendImage(outputImage,markerIds,rvecs,tvecs,cameraMatrix,distCoeffs,markerCorners); // Высылаем данные, которые будут передаваться в другой поток
            matOriginal.release();
        }

    }
}

cv::Ptr<cv::aruco::Dictionary> ProcessRtspFrames::generateCustomDict(){
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
