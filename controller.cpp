#include "controller.h"
#include <QFile>
#include <QSettings>
#include <qstringlist.h>
#include <opencv2/core/persistence.hpp>
#include <iostream>
Controller* Controller::controller_= nullptr;;

Controller::Controller()
{

}

Controller *Controller::getInstance()
{
    if(controller_==nullptr){
        controller_ = new Controller();
    }
    return controller_;
}

QString Controller::getProtocol(){
    return protocol;
}

void Controller::setProtocol(QString proto){
    protocol = proto;
}

bool Controller::getConsoleMode(){
    return consoleMode;
}

void Controller::setConsoleMode(bool mode){
    consoleMode = mode;
}

QString Controller::getUrl1(){
    return cam1Url;
}

void Controller::setUrl1(QString url){
    cam1Url = url;
}

QString Controller::getUrl2(){
    return cam2Url;
}


void Controller::setUrl2(QString url){
    cam2Url = url;
}

float Controller::getMarkerSize(){
    return markerSize;
}

void Controller::setMarkerSize(float size){
    markerSize = size;
}

int Controller::getCameraFps(){
    return cameraFPS;
}

void Controller::setCameraFps(int fps){
    cameraFPS = fps;
}

int Controller::getWriteTelemetryMs(){
    return writeTelemetryMs;
}

void Controller::setWriteTelemetryMs(int ms){
    writeTelemetryMs = ms;
}

int Controller::getUpdateUiMs(){
    return updateUiMs;
}

void Controller::setUpdateUiMs(int ms){
    updateUiMs = ms;
}

QHash<int,QString> Controller::getIsDetectPairHash(){
    return isDetectPairHash;
}

QStringList Controller::getPairDetectionMarkers(){
    return pairDetectionMarkers;
}

void Controller::setPairDetectionMarkers(QString pairMarkers){
    pairDetectionMarkersString = pairMarkers;
}

bool Controller::getUseStandartMarkers(){
    return useStandartMarkers;
}

void Controller::setUseStandartMarkers(bool use){
    useStandartMarkers = use;
}

QStringList Controller::getStandartMarkers(){
    return standartMarkers;
}

void Controller::setStandartMarkers(QString markers){
   standartMarkersString = markers;
}

QStringList Controller::getStandartMarkersCoordinates(){
    return standartMarkersCoordinates;
}

void Controller::setStandartMarkersCoordinates(QString coords){
    standartMarkersCoordinatesString = coords;
}

cv::Mat Controller::getCameraMatrix(){
    return cameraMatrix;
}

cv::Mat Controller::getDistCoeffs(){
    return distCoeffs;
}

bool Controller::getScreenShootsOnStatus(){
    return makeScreenShootsOn;
}

void Controller::setScreenShootsOnStatus(bool status){
    makeScreenShootsOn = status;
}

bool Controller::getCameraCalibrationOnStatus(){
    return cameraCalibrationOn;
}

void Controller::setCameraCalibrationOnStatus(bool status){
    cameraCalibrationOn = status;
}

QString Controller::getScreenshotPath(){
    return screenshootsPath;
}

void Controller::setScreenshotPath(QString path){
     screenshootsPath = path;
}

QString Controller::getCalibrationParamPath(){
    return calibrationParamPath;
}

void Controller::setCalibrationParamPath(QString path){
    calibrationParamPath = path;
}

bool Controller::getUseCustomMarkers(){
    return useCustomMarkers;
}

void Controller::setUseCustomMarkers(bool use){
    useCustomMarkers = use;
}

QString Controller::getCustomMarker(){
    return customMarker;
}

void Controller::setCustomMarker(QString marker){
    customMarker = marker;
}

int Controller::getCustomMarkerSize(){
    return customMarkerSize;
}

void Controller::setCustomMarkerSize(int value){
    customMarkerSize = value;
}

int Controller::getCustomMarkerSizePixel(){
    return customMarkerSizePixel;
}

void Controller::setCustomMarkerSizePixel(int size){
    customMarkerSizePixel = size;
}

QString Controller::getKobusIp(){
    return kobusIp;
}

void Controller::setKobusIp(QString ip){
    kobusIp = ip;
}

bool Controller::getDrawDestPath(){
    return drawDestPath;
}

void Controller::setDrawDestPath(bool draw){
    drawDestPath = draw;
}

bool Controller::getOldRtspMethod(){
    return oldRtspMethod;
}

void Controller::setOldRtspMethod(bool old){
    oldRtspMethod = old;
}

QString Controller::getGeometry(){
    return geometry;
}

void Controller::setGeometry(QString geom){
    geometry = geom;
}

QHash<QString,QString> Controller::getStandartMarkersHash(){
    return standartMarkersHash;
}

void Controller::readConfig(){
    QString fileName = "config.ini";

    if (!QFile::exists(fileName)){
        LOGE() << "FILE" << fileName <<" DON'T EXIST" << endl;
        //создаем тут конфиг файл
        QSettings settings(fileName, QSettings::IniFormat);
        //вариант 2
        LOGD() << "Creating " << fileName <<" with default value ..." << endl;
        settings.beginGroup("gen");
        settings.setValue("console_mode", 0);
        settings.setValue("protocol", "http");
        settings.setValue("cam_1", "http://");
        settings.setValue("cam_2", "http://");
        settings.setValue("marker_size", 0.18);
        settings.setValue("camera_fps", 4);
        settings.setValue("write_telemetry_ms", 600000);
        settings.setValue("update_ui_ms", 250);
        settings.setValue("pair_detection_markers","100 103 107 112 126 127");
        settings.setValue("use_custom_markers",0);
        settings.setValue("kobus_ip","10.11.12.20");
        settings.setValue("draw_destination_path",0);
        settings.setValue("old_rtsp_method",0);
        settings.endGroup();

        settings.beginGroup("screenshots");
        settings.setValue("make_screenshoots_on", 0);
        settings.setValue("screenshots_path", "/");
        settings.endGroup();

        settings.beginGroup("calibration");
        settings.setValue("camera_calibration_on", 0);
        settings.setValue("calibration_params_path", "/home/pi/Desktop/ARGUS/calibration_params.txt");
        settings.endGroup();

        settings.beginGroup("standart");
        settings.setValue("use_standart_markers",0);
        settings.setValue("standart_markers","0 1");
        settings.setValue("standart_markers_coordinates","0 0 0,0 0 0");
        settings.endGroup();

        readConfig();
    }
    else{
        QSettings settings(fileName, QSettings::IniFormat);
        QStringList groups = settings.childGroups();

        settings.beginGroup("gen");
        consoleMode = settings.value("console_mode").toInt();
        protocol = settings.value("protocol").toString();
        cam1Url = settings.value("cam_1").toString();
        cam2Url = settings.value("cam_2").toString();
        markerSize = settings.value("marker_size").toFloat();
        cameraFPS = settings.value("camera_fps").toInt();
        writeTelemetryMs = settings.value("write_telemetry_ms").toInt();
        updateUiMs = settings.value("update_ui_ms").toInt();
        pairDetectionMarkers = settings.value("pair_detection_markers").toString().split(' ');
        useCustomMarkers = settings.value("use_custom_markers").toInt();
        kobusIp = settings.value("kobus_ip").toString();
        LOGI()<<"KOBUS IP TEST" << kobusIp;
        drawDestPath = settings.value("draw_destination_path").toInt();
        oldRtspMethod = settings.value("old_rtsp_method").toInt();
        settings.endGroup();

        settings.beginGroup("screenshots");
        makeScreenShootsOn = settings.value("make_screenshoots_on").toInt();
        screenshootsPath = settings.value("screenshots_path").toString();
        settings.endGroup();

        settings.beginGroup("calibration");
        cameraCalibrationOn = settings.value("camera_calibration_on").toInt();
        calibrationParamPath = settings.value("calibration_params_path").toString();
        settings.endGroup();

        settings.beginGroup("standart");
        useStandartMarkers = settings.value("use_standart_markers").toInt();
        standartMarkers = settings.value("standart_markers").toString().split(' ');
        standartMarkersCoordinates = settings.value("standart_markers_coordinates").toString().split(",");
        settings.endGroup();

        clearDetectPairHash();
        getStandartPoints();
    }
}

void Controller::readCustomMarkerConfig(){
    QString fileName = "customMarkerConfig.ini";


    if (!QFile::exists(fileName)){
        LOGE() << "FILE" << fileName <<" DON'T EXIST" << endl;
        //создаем тут конфиг файл
        QSettings settings(fileName, QSettings::IniFormat);
        //вариант 2
        LOGD() << "Creating " << fileName <<" with default value ..." << endl;
        settings.beginGroup("customMarker");
        settings.setValue("custom_marker_size", 6);
        settings.setValue("custom_marker_size_pixel", 150);
        settings.setValue("custom_marker", "111111,100001,100001,100001,100001,111111;100001,010010,001100,001100,010010,100001");

        settings.endGroup();
        readCustomMarkerConfig();
    }
    else{
        QSettings settings(fileName, QSettings::IniFormat);
        QStringList groups = settings.childGroups();

        for (int i = 0; i < groups.size(); ++i) {
            settings.beginGroup(groups.at(i));
            customMarkerSize = settings.value("custom_marker_size").toInt();
            customMarkerSizePixel = settings.value("custom_marker_size_pixel").toInt();
            customMarker = settings.value("custom_marker").toString();
            settings.endGroup();
        }
        LOGD() << "Read custom marker config done "<< endl;
    }
}

void Controller::readGeometryConfig(){
    QString fileName = "geometryConfig.ini";
    if (!QFile::exists(fileName)){
        LOGE() << "FILE" << fileName <<" DON'T EXIST" << endl;
        //создаем тут конфиг файл
        QSettings settings(fileName, QSettings::IniFormat);
        //вариант 2
        LOGD() << "Creating " << fileName <<" with default value ..." << endl;
        settings.beginGroup("geom");
        settings.setValue("geometry", "0 0 0,0 0 0.2");
        settings.endGroup();
        readGeometryConfig();
    }
    else{
        QSettings settings(fileName, QSettings::IniFormat);
        QStringList groups = settings.childGroups();

        for (int i = 0; i < groups.size(); ++i) {
            settings.beginGroup(groups.at(i));
            geometry = settings.value("geometry").toString();
            settings.endGroup();
        }
    }
}

void Controller::getStandartPoints(){
    QStringList standartMarkerList = getStandartMarkers();
    QStringList standartMarkerListCoordinates = getStandartMarkersCoordinates();

    if(standartMarkerList.size()!=standartMarkerListCoordinates.size()){
        LOGE () << "ERROR! standart_markers.size != standart_markers_coordinates.size";
    } else {
        for(int i =0; i < standartMarkerList.size(); i++){
            standartMarkersHash.insert(standartMarkerList.at(i),standartMarkerListCoordinates.at(i));
        }
    }
}

void Controller::clearDetectPairHash(){
    for(int i = 0; i < pairDetectionMarkers.size(); i++){
        isDetectPairHash.insert(pairDetectionMarkers.at(i).toInt(),"");
    }
}

bool Controller::readCalibrationParams(){
    cv::FileStorage fs;
    fs.open(getCalibrationParamPath().toStdString(), cv::FileStorage::READ);
    if (!fs.isOpened())
    {
        LOGE() << "Error opening calibration file.";
        return false;
    }
    fs["camera_matrix"] >> cameraMatrix;
    fs["distortion_coefficients"] >> distCoeffs;
    return true;
}

void Controller::updateAllCongigs(){
    QString fileName = "config.ini";
    QString customMarkerConfig = "customMarkerConfig.ini";
    QString geometryConfig = "geometryConfig.ini";

    QSettings settings(fileName, QSettings::IniFormat);
    QSettings customMarkerSettings(customMarkerConfig, QSettings::IniFormat);
    QSettings geometrySettings(geometryConfig, QSettings::IniFormat);

    settings.beginGroup("gen");
    settings.setValue("console_mode", int(getConsoleMode()));
    settings.setValue("protocol", getProtocol());
    settings.setValue("cam_1", getUrl1());
    settings.setValue("cam_2", getUrl2());
    settings.setValue("marker_size", double(getMarkerSize()));
    settings.setValue("camera_fps", getCameraFps());
    settings.setValue("write_telemetry_ms", getWriteTelemetryMs());
    settings.setValue("update_ui_ms", getUpdateUiMs());
    settings.setValue("pair_detection_markers",pairDetectionMarkersString);
    settings.setValue("use_custom_markers",int(getUseCustomMarkers()));
    settings.setValue("kobus_ip",getKobusIp());
    settings.setValue("draw_destination_path",int(getDrawDestPath()));
    settings.setValue("old_rtsp_method",int(getOldRtspMethod()));
    settings.endGroup();

    settings.beginGroup("screenshots");
    settings.setValue("make_screenshoots_on", int(getScreenShootsOnStatus()));
    settings.setValue("screenshots_path", getScreenshotPath());
    settings.endGroup();

    settings.beginGroup("calibration");
    settings.setValue("camera_calibration_on", int(getCameraCalibrationOnStatus()));
    settings.setValue("calibration_params_path", getCalibrationParamPath());
    settings.endGroup();

    settings.beginGroup("standart");
    settings.setValue("use_standart_markers",int(getUseStandartMarkers()));
    settings.setValue("standart_markers",standartMarkersString);
    settings.setValue("standart_markers_coordinates",standartMarkersCoordinatesString);
    settings.endGroup();

    customMarkerSettings.beginGroup("customMarker");
    customMarkerSettings.setValue("custom_marker_size", getCustomMarkerSize());
    customMarkerSettings.setValue("custom_marker_size_pixel", getCustomMarkerSizePixel());
    customMarkerSettings.setValue("custom_marker", getCustomMarker());
    customMarkerSettings.endGroup();

    geometrySettings.beginGroup("geom");
    geometrySettings.setValue("geometry", getGeometry());
    geometrySettings.endGroup();
}
