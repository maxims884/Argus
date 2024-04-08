QT       += core gui testlib multimedia network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cameracalibration.cpp \
    controller.cpp \
    customarucodictionary.cpp \
    ffmpegdecoder.cpp \
    getframethreadhttp.cpp \
    getframethreadrtsp.cpp \
    main.cpp \
    mainwindow.cpp \
    processrtspframes.cpp \
    telemetrywriter.cpp \
    udpspot.cpp

HEADERS += \
    cameracalibration.h \
    controller.h \
    customarucodictionary.h \
    ffmpegdecoder.h \
    getframethreadhttp.h \
    getframethreadrtsp.h \
    mainwindow.h \
    processrtspframes.h \
    telemetrywriter.h \
    udpspot.h

FORMS += \
    mainwindow.ui

#либы под Малинку
CONFIG("Raspberrypi"){
    INCLUDEPATH += /home/max/rpi-qt/sysroot/usr/include
    DEPENDPATH += /home/max/rpi-qt/sysroot/usr/include

    LIBS += -L/home/max/rpi-qt/sysroot/usr/local/lib/arm-linux-gnueabihf/ -lopencv_aruco -lopencv_calib3d -lopencv_features2d -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect -lopencv_imgcodecs -lopencv_videoio

    INCLUDEPATH += /home/max/rpi-qt/sysroot/usr/local/include/opencv4
    DEPENDPATH += /home/max/rpi-qt/sysroot/usr/local/include/opencv4

    INCLUDEPATH += /home/max/rpi-qt/sysroot/usr/local/include
    DEPENDPATH += /home/max/rpi-qt/sysroot/usr/local/include

    #av ffmpeg

    INCLUDEPATH += /home/max/rpi-qt/sysroot/usr/include/arm-linux-gnueabihf
    DEPENDPATH += /home/max/rpi-qt/sysroot/usr/include/arm-linux-gnueabihf

    LIBS += -L/home/max/rpi-qt/sysroot/lib/arm-linux-gnueabihf/ -lpthread -lavcodec -lavformat -lavutil -lswscale -lswresample -lavdevice -lavfilter -llapack
    LIBS += -L/home/max/rpi-qt/sysroot/lib/arm-linux-gnueabihf/lapack/ -llapack
    LIBS += -L/home/max/rpi-qt/sysroot/lib/arm-linux-gnueabihf/blas/ -lblas

}

#либы под КОМП ЛИНУКС
CONFIG("Desktop") {
    INCLUDEPATH += /usr/local/include
    LIBS += -IL/usr/local/lib/

    INCLUDEPATH += /usr/local/include/opencv4
    LIBS += -L/home/max/opencv/opencv-4.5.5-debug/lib/ -lopencv_aruco -lopencv_calib3d -lopencv_features2d -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect -lopencv_imgcodecs -lopencv_videoio

    #av ffmpeg
    LIBS += -L/usr/lib/aarch64-linux-gnu -lpthread -lavcodec  -lavformat  -lavutil  -lswscale -lswresample -lavdevice -lavfilter -llapack
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

с += \
    config.ini

DISTFILES +=
