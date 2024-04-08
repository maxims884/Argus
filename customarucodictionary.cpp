#include "customarucodictionary.h"
#include <controller.h>
#include <iostream>
using namespace std;
//CustomArucoDictionary::CustomArucoDictionary()
//{
//    LOGI() << "CustomArucoDictionary CONSTR";
//}

//void CustomArucoDictionary::generateCustomDict(){
//    QString marker = Controller::getInstance()->getCustomMarker();
//    int customMarkerSize = Controller::getInstance()->getCustomMarkerSize();
//    int customMarkerSizePixel = Controller::getInstance()->getCustomMarkerSizePixel();

//    QStringList markerList = marker.split(";");
//    cv::Ptr<cv::aruco::Dictionary> customDictionary = cv::aruco::generateCustomDictionary(0,customMarkerSize,1);


//    for(QString element:markerList)
//    {
//         bool bits[customMarkerSize][customMarkerSize];
//         QStringList markerElementsList = element.split(",");
//         for(int i = 0; i < markerElementsList.size(); ++i){
//             for (int j = 0; j < markerElementsList.at(i).size(); ++j) {
//                 bits[i][j] = markerElementsList.at(i).at(j).toLatin1() - '0';
//             }
//         }
//         cv::Mat dictImage = cv::aruco::Dictionary::getByteListFromBits(cv::Mat(customMarkerSize,customMarkerSize,CV_8U,bits));
//         customDictionary->bytesList.push_back(dictImage);
//    }


//    for(int i = 0; i < markerList.size(); i++)
//    {
//        LOGI() << "Marker " << i;
//        cv::Mat arucoMarker;
//        drawMarker(customDictionary,i,customMarkerSizePixel,arucoMarker);
//        std::string str = "marker" + std::to_string(i);
////        imshow(str,arucoMarker);
////        cv::imwrite("/home/max/Рабочий стол/custom_marker/" + str + ".jpg", arucoMarker);
//    }

//    customDict = customDictionary;
//    cout << "customDict " << customDict->bytesList;
//}

//cv::Ptr<cv::aruco::Dictionary> CustomArucoDictionary::getCustomDict(){
//    return customDict;
//}
