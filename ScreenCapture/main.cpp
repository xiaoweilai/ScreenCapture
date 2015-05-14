#define __STDC_CONSTANT_MACROS
extern "C"
{
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#include <libavformat/avformat.h>
}

#include <QtCore/QCoreApplication>
#include <QApplication>
#include <stdio.h>
#include <math.h>
#include <QDebug>
#include "capthread.h"

#define INBUF_SIZE 4096
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096
//#define ORGSRC
#define DESK_WIDTH  (1366)
#define DESK_HEIGHT (768)


static int decode_write_frame(AVCodecContext *avctx, AVFrame *frame, int *frame_count, AVPacket *pkt, int last)
{
    int len, got_frame;

    len = avcodec_decode_video2(avctx, frame, &got_frame, pkt);
    if (len < 0) {
        printf("Error while decoding frame %d\n", *frame_count);
        return len;
    }
    if (got_frame) {
        printf("Saving frame %3d\n", *frame_count);

        QImage image(QSize(avctx->width, avctx->height), QImage::Format_RGB888);

        for(int h = 0; h < avctx->height; h++)
        {
            for(int w = 0; w < avctx->width; w ++)
            {
                int hh = h >> 1;
                int ww = w >> 1;
                int Y = frame->data[0][h * frame->linesize[0] + w];
                int U = frame->data[1][hh * (frame->linesize[1]) + ww];
                int V = frame->data[2][hh * (frame->linesize[2]) + ww];

                int C = Y - 16;
                int D = U - 128;
                int E = V - 128;

                int r = 298 * C           + 409 * E + 128;
                int g = 298 * C - 100 * D - 208 * E + 128;
                int b = 298 * C + 516 * D           + 128;

                r = qBound(0, r >> 8, 255);
                g = qBound(0, g >> 8, 255);
                b = qBound(0, b >> 8, 255);

                r = qBound(0, r, 255);
                g = qBound(0, g, 255);
                b = qBound(0, b, 255);

                QRgb rgb = qRgb(r, g, b);
                image.setPixel(QPoint(w, h), rgb);
            }
        }

        QString fname = QString("img") + QString::number(*frame_count) + ".jpg";
        image.save(fname);


        //pgm_save(frame->data[0], frame->linesize[0], avctx->width, avctx->height, buf);
        (*frame_count)++;
    }
    if (pkt->data) {
        pkt->size -= len;
        pkt->data += len;
    }
    return 0;
}

void decodeUt()
{

    avcodec_register_all();
    AVCodec *codec;
    AVCodecContext *c= NULL;
    int frame_count;
    FILE *f;
    AVFrame *frame;
    uint8_t inbuf[INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
    AVPacket avpkt;
    av_init_packet(&avpkt);

    memset(inbuf + INBUF_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE);


    codec = avcodec_find_decoder(AV_CODEC_ID_MPEG1VIDEO);
    if (!codec)
    {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }
    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }
    if(codec->capabilities&CODEC_CAP_TRUNCATED)
        c->flags|= CODEC_FLAG_TRUNCATED;
    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }
    f = fopen("test.mpg", "rb");
    if (!f) {
        fprintf(stderr, "Could not open input file\n");
        exit(1);
    }
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    frame_count = 0;
    for (;;) {
        fprintf(stdout, "parse video frame\n");
        avpkt.size = fread(inbuf, 1, INBUF_SIZE, f);
        if (avpkt.size == 0)
            break;
        avpkt.data = inbuf;
        while (avpkt.size > 0)
            if (decode_write_frame(c, frame, &frame_count, &avpkt, 0) < 0)
                exit(1);
    }

}



void getVideoInfo()
{
    //    1)如何获取媒体文件的信息(Parser):
    // 参考V3代码: interface IFileDecoder, media/impl/filedecoderimpl.cpp
    av_register_all();
    AVFormatContext * pFormatCtx = NULL;
    int err = 0;
    const char *fileName = "test.mpg";
    err = avformat_open_input(&pFormatCtx, fileName, NULL,  NULL);
    if(err != 0)
    {
        // break ;
    }
    err = avformat_find_stream_info(pFormatCtx,NULL);
    if(err < 0)
    {
        // break ;
    }
    qDebug() << "pFormatCtx->nb_streams:" << pFormatCtx->nb_streams;
    for(uint32_t i = 0; i < pFormatCtx->nb_streams; i ++)
    {
        // stream 结构数据
        AVStream *pStream = pFormatCtx->streams[i];
        // 帧率信息
        AVRational frameRate = pStream->r_frame_rate;
        // 时间单位比率
        AVRational timeBase = pStream->time_base;
        // stream duration
        int64_t duration = pStream->duration;

        // 获取Codec数据结构
        AVCodecContext *pCodecCtx = pStream->codec;
        AVMediaType codecType = pCodecCtx->codec_type;

        AVCodecID codecId = pCodecCtx->codec_id;

        qDebug("codecId:%d\n", codecId);
        qDebug("AV_CODEC_ID_H264:%d\n", AV_CODEC_ID_H264);




        if(codecType == AVMEDIA_TYPE_VIDEO)
        {
            // 获取Video基本信息
            int width = pCodecCtx->width;
            int height = pCodecCtx->height;
            PixelFormat pixelFormat = pCodecCtx->pix_fmt;
        }
        else if(codecType == AVMEDIA_TYPE_AUDIO)
        {
            // 获取Audio基本信息
            int channels = pCodecCtx->channels;
            int sample_rate = pCodecCtx->sample_rate;
            AVSampleFormat sampleFmt = pCodecCtx->sample_fmt;
        }
    }
    // 释放
    if(pFormatCtx != NULL)
    {
        avformat_close_input(&pFormatCtx);
        pFormatCtx = NULL;
    }

    qDebug("App End\n");

}



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    int w = DESK_WIDTH;
    int h = DESK_HEIGHT;

    printf("w:%d h:%d\n", w, h);
    //    CapThread* th = new CapThread(w, h);
    //    th->start();

//    decodeUt();


    getVideoInfo();


    return a.exec();
}
