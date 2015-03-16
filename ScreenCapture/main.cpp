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
}

#define INBUF_SIZE 4096
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096

#include "capthread.h"
#include <stdio.h>
#include <math.h>


#include <QtCore/QCoreApplication>
#include <QApplication>

#include "capthread.h"


static int decode_write_frame(AVCodecContext *avctx, AVFrame *frame, int *frame_count, AVPacket *pkt, int last)
{
    int len, got_frame;
    char buf[1024];
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


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    int w = QString(argv[1]).toInt();
    int h = QString(argv[2]).toInt();
    printf("w:%d h:%d\n", w, h);
    CapThread* th = new CapThread(w, h);
    th->start();
/*
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
        avpkt.size = fread(inbuf, 1, INBUF_SIZE, f);
        if (avpkt.size == 0)
            break;
        avpkt.data = inbuf;
        while (avpkt.size > 0)
            if (decode_write_frame(c, frame, &frame_count, &avpkt, 0) < 0)
                exit(1);
    }
*/

    return a.exec();
}
