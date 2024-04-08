#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <customarucodictionary.h>
#include <QMap>
#include <QStringList>
#include <qstring.h>
#include <string>
#include <opencv2/core/mat.hpp>
#include <QHash>
#include <telemetrywriter.h>

class Controller
{
public:
    Controller();
    void instance();
    static Controller *getInstance();
    void readConfig();
    void readCustomMarkerConfig();
    void readGeometryConfig();
    bool readCalibrationParams();

    QString getProtocol();
    void setProtocol(QString proto);
    bool getConsoleMode();
    void setConsoleMode(bool mode);
    QString getUrl1();
    void setUrl1(QString url);
    QString getUrl2();
    void setUrl2(QString url);
    float getMarkerSize();
    void setMarkerSize(float size);
    int getCameraFps();
    void setCameraFps(int fps);
    int getWriteTelemetryMs();
    void setWriteTelemetryMs(int ms);
    int getUpdateUiMs();
    void setUpdateUiMs(int ms);
    QStringList getPairDetectionMarkers();
    void setPairDetectionMarkers(QString pairMarkers);
    QHash<int,QString> getIsDetectPairHash();

    bool getUseStandartMarkers();
    void setUseStandartMarkers(bool use);
    QStringList getStandartMarkers();
    void setStandartMarkers(QString markers);
    QStringList getStandartMarkersCoordinates();
    void setStandartMarkersCoordinates(QString coords);

    cv::Mat getCameraMatrix();
    cv::Mat getDistCoeffs();

    bool getScreenShootsOnStatus();
    void setScreenShootsOnStatus(bool status);
    bool getCameraCalibrationOnStatus();
    void setCameraCalibrationOnStatus(bool status);
    QString getScreenshotPath();
    void setScreenshotPath(QString path);
    QString getCalibrationParamPath();
    void setCalibrationParamPath(QString path);
    bool getUseCustomMarkers();
    void setUseCustomMarkers(bool use);
    QString getKobusIp();
    void setKobusIp(QString ip);
    bool getDrawDestPath();
    void setDrawDestPath(bool draw);
    bool getOldRtspMethod();
    void setOldRtspMethod(bool old);

    QString getGeometry();
    void setGeometry(QString geom);
    QString getCustomMarker();
    void setCustomMarker(QString marker);
    int getCustomMarkerSize();
    void setCustomMarkerSize(int value);
    int getCustomMarkerSizePixel();
    void setCustomMarkerSizePixel(int size);
    void clearDetectPairHash();
    void getStandartPoints();
    QHash<QString,QString> getStandartMarkersHash();
    void updateAllCongigs();
private:
    static Controller* controller_;

    bool consoleMode = 0;
    QString protocol = "";
    QString cam1Url = "";
    QString cam2Url = "";
    bool makeScreenShootsOn = false;
    QString screenshootsPath = "";
    bool cameraCalibrationOn = false;
    QString calibrationParamPath = "";
    float markerSize = 0.0;
    int cameraFPS = 0;
    int writeTelemetryMs = 600000;
    int updateUiMs = 0;
    QStringList pairDetectionMarkers;
    QString pairDetectionMarkersString = "";
    bool useCustomMarkers = false;
    QString kobusIp = "";
    bool useStandartMarkers = false;
    bool drawDestPath = 0;
    bool oldRtspMethod = 0;
    QStringList standartMarkers;
    QString standartMarkersString = "";
    QStringList standartMarkersCoordinates;
    QString standartMarkersCoordinatesString = "";

    QString customMarker = "";
    QString geometry = "";
    int customMarkerSize = 6;
    int customMarkerSizePixel =150;

    QHash<int,QString> isDetectPairHash;
    cv::Mat cameraMatrix, distCoeffs;
//    QStringList configLines;
//    QMap <QString, QString> fieldsMap;
    int screenShootCounter = 0;

    QHash<QString,QString> standartMarkersHash;
};

#endif // CONTROLLER_H
