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

#include <QDebug>
#include <QPixmap>
#include <QPainter>
///*
//*设置一个像素
//*/
//void CapThread::setImagePixel(const QPoint &pos, bool opaque)
//{
//    int i,j;
//    //将窗口部件的左边转换到图像的坐标上面，部件的位置除以缩放因子则得到原始的图像位置。
//    i=pos.x()/this->zoom;
//    j=pos.y()/this->zoom;
//    if(this->image.rect().contains(i,j)){//这个点是否属于图像的正确范围
//        if(opaque){
//            this->image.setPixel(i,j,penColor().rgba());
//        }else{
//            this->image.setPixel(i,j,qRgba(0,0,0,0));
//        }
//        update(pixelRect(i,j));
//    }
//}

void CapThread::capFrame()
{
    QImage image = QPixmap::grabWindow(QApplication::desktop()->winId()).toImage();
//    image = image.scaled(QSize(resize_width, resize_height));

//    printf("resize_width:%d,resize_height:%d\n",
//           resize_width,
//           resize_height);
    QCursor cusr;
////    QPoint curpos = QCursor::pos();
////    QCursor.shape();
//    qDebug() <<"cursor pos:"<<cusr.pos().x()<<","<<cusr.pos().y();
//    QImage cursorimage = cusr.pixmap().toImage();



//    image.setPixel(curpos,QCursor.pixmap().);
    int x = cusr.pos().x();
    int y = cusr.pos().y();
    image.setPixel(x,y,qRgba(255,255,255,255));
    image.setPixel(x+1,y,qRgba(255,255,255,255));
    image.setPixel(x,y+1,qRgba(255,255,255,255));
    image.setPixel(x,y-1,qRgba(255,255,255,255));


    av_init_packet(pkt);
    pkt->data = NULL;    // packet data will be allocated by the encoder
    pkt->size = 0;

    for (int h = 0; h < c->height; h++)
    {
        for (int w = 0; w < c->width; w++)
        {
            QRgb rgb = image.pixel(w, h);

            int r = qRed(rgb);
            int g = qGreen(rgb);
            int b = qBlue(rgb);

            int dy = ((66*r + 129*g + 25*b) >> 8) + 16;
            int du = ((-38*r + -74*g + 112*b) >> 8) + 128;
            int dv = ((112*r + -94*g + -18*b) >> 8) + 128;

            uchar yy = (uchar)dy;
            uchar uu = (uchar)du;
            uchar vv = (uchar)dv;

            frame->data[0][h * frame->linesize[0] + w] = yy;

            if(h % 2 == 0 && w % 2 == 0)
            {
                frame->data[1][h/2 * (frame->linesize[1]) + w/2] = uu;
                frame->data[2][h/2 * (frame->linesize[2]) + w/2] = vv;
            }

        }
    }

    frame->pts = i;

    /* encode the image */
    ret = avcodec_encode_video2(c, pkt, frame, &got_output);

    if (ret < 0)
    {
        printf("Error encoding frame\n");
        exit(1);
    }

    if (got_output)
    {
        printf("Write frame %3d (size=%5d)\n", i, pkt->size);

        fwrite(pkt->data, 1, pkt->size, f);
        fflush(f);
        av_free_packet(pkt);
    }

    i ++;
}


CapThread::CapThread(int width, int height, QObject *parent) : QThread(parent)
{
    resize_width = width;
    resize_height = height;

    avcodec_register_all();
    c= NULL;
    pkt = new AVPacket;
    i = 0;


    codec = avcodec_find_encoder(AV_CODEC_ID_H264);

    if (codec == 0)
    {
        printf("find encoder failed\n");
        exit(1);
    }
/*
    const AVRational* sup = codec->supported_framerates;
    int k = 0;
    while(true)
    {
        AVRational rat = sup[k];
        if(rat.num == 0 && rat.den == 0)
            break;

        printf("%d  %d\n", rat.num, rat.den);
        k++;
    }*/

    c = avcodec_alloc_context3(codec);
    if (!c)
    {
        printf("alloc context failed\n");
        exit(1);
    }

//    c->bit_rate = 400000;
    c->width = width;
    c->height = height;
    c->time_base = (AVRational){1, 25};
    c->gop_size = 20;
    c->max_b_frames = 1;
    c->pix_fmt = AV_PIX_FMT_YUV420P;


    int re = avcodec_open2(c, codec, NULL);
    if (re < 0)
    {
        printf("open codec failed\n");
        exit(1);
    }
    f = fopen("test.mpg", "wb");
    if (!f)
    {
        printf("open output file failed\n");
        exit(1);
    }
    frame = av_frame_alloc();
    if (!frame)
    {
        printf("Could not allocate video frame\n");
        exit(1);
    }
    frame->format = c->pix_fmt;
    frame->width  = c->width;
    frame->height = c->height;
    /* the image can be allocated by any means and av_image_alloc() is
     * just the most convenient way if av_malloc() is to be used */
    ret = av_image_alloc(frame->data, frame->linesize, c->width, c->height, c->pix_fmt, 32);
    if (ret < 0)
    {
        printf("Could not allocate raw picture buffer\n");
        exit(1);
    }
}

void CapThread::run()
{
    while(1){
        capFrame();
    }
}
