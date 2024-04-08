#ifndef TELEMETRYWRITER_H
#define TELEMETRYWRITER_H

#include <QTextStream>
#include <qfile.h>
#include <bits/stl_vector.h>
#include <log.h>

QT_BEGIN_NAMESPACE
using namespace std;
QT_END_NAMESPACE

class TelemetryWriter
{
public:
    TelemetryWriter();
     ~TelemetryWriter();
    void writeToFile();
    void setCamera1DetectedMarkers(vector<int> markerIds);
    void increaseCamera1ProcessedFrameCount();
    void setCamera2DetectedMarkers(vector<int> markerIds);
    void increaseCamera2ProcessedFrameCount();
    void appendCoordsCollectedData(QString outputData);
    void init();
private:
    QFile telemetryFile;
    vector<int> markerIds;
    int camera1ProcessedFrameCount = 0;
    int camera2ProcessedFrameCount = 0;
    QString coordsCollectedData = "";
    QStringList markers;
    QHash<int,int> countCamera1;
    QHash<int,int> countCamera2;

//    int markerID100CountCamera1 = 0;
//    int markerID103CountCamera1 = 0;
//    int markerID107CountCamera1 = 0;
//    int markerID112CountCamera1 = 0;
//    int markerID126CountCamera1 = 0;
//    int markerID127CountCamera1 = 0;

//    int markerID100CountCamera2 = 0;
//    int markerID103CountCamera2 = 0;
//    int markerID107CountCamera2 = 0;
//    int markerID112CountCamera2 = 0;
//    int markerID126CountCamera2 = 0;
//    int markerID127CountCamera2 = 0;
};

#endif // TELEMETRYWRITER_H
