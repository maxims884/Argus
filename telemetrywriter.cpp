#include "telemetrywriter.h"
#include "qdatetime.h"
#include <controller.h>

TelemetryWriter::TelemetryWriter()
{
    telemetryFile.setFileName("telemetry.txt");
    if(!telemetryFile.open(QIODevice::Append)) {
       LOGE() << "Error opening telemetry file.";
       return;
    }
}

void TelemetryWriter::setCamera1DetectedMarkers(vector<int> ids){
    for(int markerID : ids){
        for(int i = 0; i < markers.size(); i++){
            int ID = markers.at(i).toInt();
            if(ID == markerID){
                countCamera1[ID]++;
            }
        }
    }
}

void TelemetryWriter::increaseCamera1ProcessedFrameCount(){
    camera1ProcessedFrameCount++;
}

void TelemetryWriter::setCamera2DetectedMarkers(vector<int> ids){
    for(int markerID : ids){
        for(int i = 0; i < markers.size(); i++){
            int ID = markers.at(i).toInt();
            if(ID == markerID){
                countCamera2[ID]++;
            }
        }
    }
}

void TelemetryWriter::increaseCamera2ProcessedFrameCount(){
    camera2ProcessedFrameCount++;
}

void TelemetryWriter::appendCoordsCollectedData(QString outputData){
    coordsCollectedData.append(outputData);
}

void TelemetryWriter::writeToFile(){
    QTime ct = QTime::currentTime();
    QDate cd = QDate::currentDate();

    QTextStream stream(&telemetryFile);

    if(markers.size() % 2 == 0){
        stream << "Camera1 " << ct.toString() << " " << cd.toString();
        for(int i = 0; i < markers.size(); i++){
            stream << " | " << markers.at(i) << ": " << countCamera1.value(markers.at(i).toInt());
        }
        stream <<  "| FrameCount: " << camera1ProcessedFrameCount << "\n";

        stream << "Camera2 " << ct.toString() << " " << cd.toString();
        for(int i = 0; i < markers.size(); i++){
            stream << " | " << markers.at(i) << ": " << countCamera2.value(markers.at(i).toInt());
        }
        stream <<  "| FrameCount: " << camera2ProcessedFrameCount << "\n";
    } else {
        LOGE() << "Not all markers are paired in config";
    }

    stream << coordsCollectedData;
    camera1ProcessedFrameCount = 0;
    camera2ProcessedFrameCount = 0;

    for(int i = 0; i < markers.size(); i++){
        countCamera1[markers.at(i).toInt()] = 0;
        countCamera2[markers.at(i).toInt()] = 0;
    }

    coordsCollectedData = "";
}

void TelemetryWriter::init(){
    markers = Controller::getInstance()->getPairDetectionMarkers();
    for(int i = 0; i < markers.size(); i++){
        int markerID = markers.at(i).toInt();
        countCamera1.insert(markerID,0);
        countCamera2.insert(markerID,0);
    }
}

TelemetryWriter::~TelemetryWriter()
{
//    TRACELOG;
    QTextStream stream(&telemetryFile);
    stream << "Program closed\n";
    telemetryFile.close();
}
