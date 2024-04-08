#ifndef ffmpegdecoder_H
#define ffmpegdecoder_H

#include <iostream>
#include <string>

#include <thread>
#include <mutex>
#include <chrono>
#include <qobject.h>

#include <opencv2/opencv.hpp>
#include <qtimer.h>

extern "C"
{
#include <libavutil/mathematics.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libavutil/pixdesc.h>
#include <libavdevice/avdevice.h>

}

class FFmpegDecoder : public QObject
{
    Q_OBJECT
    public:
        FFmpegDecoder(std::string);
        ~FFmpegDecoder();

        void connect();
        void decode();

        bool isConncected() const {return bConnected;}

        std::deque <cv::Mat> decodedImgBuf;
        std::mutex mtx;

    private:

        void destroy();

        AVFormatContext *pFormatCtx;
        AVCodecContext *pCodecCtx;
        AVCodec *pCodec;
        AVFrame *pFrame, *pFrameBGR;
        AVPacket *packet;
        uint8_t *outBuffer;
        SwsContext *imgConvertCtx;

        int videoStream;
        std::string path;
        bool bConnected;

public slots:
        void decodeSlot();
signals:
    void finished();
};

#endif
