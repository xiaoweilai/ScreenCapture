/**
 * 最简单的基于FFmpeg的AVDevice例子（屏幕录制）
 * Simplest FFmpeg Device (Screen Capture)
 *
 * 雷霄骅 Lei Xiaohua
 * leixiaohua1020@126.com
 * 中国传媒大学/数字电视技术
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 *
 * 本程序实现了屏幕录制功能。可以录制并播放桌面数据。是基于FFmpeg
 * 的libavdevice类库最简单的例子。通过该例子，可以学习FFmpeg中
 * libavdevice类库的使用方法。
 * 本程序在Windows下可以使用2种方式录制屏幕：
 *  1.gdigrab: Win32下的基于GDI的屏幕录制设备。
 *             抓取桌面的时候，输入URL为“desktop”。
 *  2.dshow: 使用Directshow。注意需要安装额外的软件screen-capture-recorder
 * 在Linux下则可以使用x11grab录制屏幕。
 *
 * This software capture screen of computer. It's the simplest example
 * about usage of FFmpeg's libavdevice Library.
 * It's suiltable for the beginner of FFmpeg.
 * This software support 2 methods to capture screen in Microsoft Windows:
 *  1.gdigrab: Win32 GDI-based screen capture device.
 *             Input URL in avformat_open_input() is "desktop".
 *  2.dshow: Use Directshow. Need to install screen-capture-recorder.
 * It use x11grab to capture screen in Linux.
 *
 * 二次修改：添加编码功能
 * 魏新建 Wei Xinjian
 * wxjlmr@126.com
 *
 */





//Output YUV420P
#define OUTPUT_YUV420P 0
//'1' Use Dshow
//'0' Use GDIgrab
#define USE_DSHOW 0

//Refresh Event
#define SFM_REFRESH_EVENT  (SDL_USEREVENT + 1)


//#define ORGSRC
#define DESK_WIDTH  (1366)
#define DESK_HEIGHT (768)





#define INBUF_SIZE 4096
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096

#include "capthread.h"
#include <stdio.h>
#include <math.h>

#include <QDebug>
#include <QPixmap>
#include <QPainter>
#include <QCursor>
#include <QtCore>
//#include <windows.h>
//#include <winuser.h>
#include <QWidget>
#include <QGraphicsItem>



int thread_exit=0;

int sfp_refresh_thread(void *opaque)
{
    while (thread_exit==0) {
        SDL_Event event;
        event.type = SFM_REFRESH_EVENT;
        SDL_PushEvent(&event);
        SDL_Delay(40);
    }
    return 0;
}


//Show Dshow Device
void CapThread::show_dshow_device()
{
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    AVDictionary* options = NULL;
    av_dict_set(&options,"list_devices","true",0);
    AVInputFormat *iformat = av_find_input_format("dshow");
    printf("========Device Info=============\n");
    avformat_open_input(&pFormatCtx,"video=dummy",iformat,&options);
    printf("================================\n");
}

//Show AVFoundation Device
void CapThread::show_avfoundation_device()
{
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    AVDictionary* options = NULL;
    av_dict_set(&options,"list_devices","true",0);
    AVInputFormat *iformat = av_find_input_format("avfoundation");
    printf("==AVFoundation Device Info===\n");
    avformat_open_input(&pFormatCtx,"",iformat,&options);
    printf("=============================\n");
}




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
    #if 0
    QImage image = QPixmap::grabWindow(QApplication::desktop()->winId()).toImage();
    //    image = image.scaled(QSize(resize_width, resize_height));

    //    printf("resize_width:%d,resize_height:%d\n",
    //           resize_width,
    //           resize_height);
    QCursor curCsor;
    updateMouseShape(curCsor.pos(),curCsor.shape());
    ////    QPoint curpos = QCursor::pos();
    ////    QCursor.shape();
    //    qDebug() <<"cursor pos:"<<cusr.pos().x()<<","<<cusr.pos().y();
    //    QImage cursorimage = cusr.pixmap().toImage();



    //    image.setPixel(curpos,QCursor.pixmap().);
    //    int x = cusr.pos().x();
    //    int y = cusr.pos().y();
    //    image.setPixel(x,y,qRgba(255,255,255,255));
    //    image.setPixel(x+1,y,qRgba(255,255,255,255));
    //    image.setPixel(x,y+1,qRgba(255,255,255,255));
    //    image.setPixel(x,y+2,qRgba(255,255,255,255));
    //    image.setPixel(x,y+3,qRgba(255,255,255,255));
    //    image.setPixel(x,y+4,qRgba(255,255,255,255));
    //    image.setPixel(x,y+5,qRgba(255,255,255,255));
    //    image.setPixel(x,y+6,qRgba(255,255,255,255));
    //    curCsor.shape();


    ////    image.set;
    //    QPainter painter(&image);
    //    painter.drawImage(cusr.pos(),cursorimage);
    ////    int w = QCursor.pixmap().width();
    ////    int h = QCursor.pixmap().height();
    ////    painter.drawPixmap(cusr.pos().x(),curpos.y,w,h,QCursor.pixmap());

    ////    QImage resultImage = image;
    ////    QPainter painter(&resultImage);
    ////    painter.setCompositionMode(QPainter::CompositionMode_Xor);
    ////    painter.drawImage(curpos, butterflyImage);




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

    /* encode the image 压缩图片 avcodec_encode_video2
参数说明：
1.c :codec content
2.pkt: output AVPacket.
3.frame:AVFrame containing the raw video data to be encoded.
4. got_output: got_packet_ptr This field is set to 1 by libavcodec if the
 * output packet is non-empty, and to 0 if it is empty.
*/
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
#endif
}


CapThread::CapThread(int width, int height, QObject *parent) : QThread(parent)
{
    //save screen rect
    resize_width = width;
    resize_height = height;


    //ffmpeg info
    av_register_all();
    avformat_network_init();
    //Register Device
    avdevice_register_all();
    avcodec_register_all();
    pEc= NULL;
    pDc= NULL;
    pFormatCtx = avformat_alloc_context();
    pkt=(AVPacket *)av_malloc(sizeof(AVPacket));


    printf("sizeof(AVPacket): %d\n",   sizeof(AVPacket));
    //Open File
    //char filepath[]="src01_480x272_22.h265";
    //avformat_open_input(&pFormatCtx,filepath,NULL,NULL)



    pEcodec = avcodec_find_encoder(AV_CODEC_ID_H264);

    if (pEcodec == 0)
    {
        printf("find encoder failed\n");
        exit(1);
    }

    pEc = avcodec_alloc_context3(pEcodec);
    if (!pEc)
    {
        printf("alloc context failed\n");
        exit(1);
    }

    //    c->bit_rate = 400000;
    pEc->width = resize_width;
    pEc->height = resize_height;
    //c->time_base = (AVRational){1, 25};//num,den
    pEc->gop_size = 20;
    pEc->max_b_frames = 1;
    pEc->pix_fmt = AV_PIX_FMT_YUV420P;
    /* frames per second */
    pEc->time_base.num = 1;
    pEc->time_base.den = 15;//(14,15,15最接近实际时间)

    int re = avcodec_open2(pEc, pEcodec, NULL);
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


    pEframe = av_frame_alloc();
    if (!pEframe)
    {
        printf("Could not allocate video frame\n");
        exit(1);
    }

    pEframe->format = pEc->pix_fmt;
    pEframe->width  = pEc->width;
    pEframe->height = pEc->height;
    /* the image can be allocated by any means and av_image_alloc() is
     * just the most convenient way if av_malloc() is to be used */
    ret = av_image_alloc(pEframe->data, pEframe->linesize, pEc->width, pEc->height, pEc->pix_fmt, 32);
    if (ret < 0)
    {
        printf("Could not allocate raw picture buffer\n");
        exit(1);
    }


    //Windows
#ifdef MAINWORKING
#if USE_DSHOW
    //Use dshow
    //
    //Need to Install screen-capture-recorder
    //screen-capture-recorder
    //Website: http://sourceforge.net/projects/screencapturer/
    //
    AVInputFormat *ifmt=av_find_input_format("dshow");
    if(avformat_open_input(&pFormatCtx,"video=screen-capture-recorder",ifmt,NULL)!=0){
        printf("Couldn't open input stream.\n");
        return -1;
    }
#else
    //Use gdigrab
    AVDictionary* options = NULL;
    //Set some options
    //grabbing frame rate
    //av_dict_set(&options,"framerate","5",0);
    //The distance from the left edge of the screen or desktop
    //av_dict_set(&options,"offset_x","20",0);
    //The distance from the top edge of the screen or desktop
    //av_dict_set(&options,"offset_y","40",0);
    //Video frame size. The default is to capture the full screen
    //av_dict_set(&options,"video_size","640x480",0);
    AVInputFormat *ifmt=av_find_input_format("gdigrab");//格式
    /* avformat_open_input：
    func: open an input stream and read the header.The Codecs are not opened.The stream must be closed with avformat_close_input()
    para show:
    1. pFormatCtx : pointer to user-supplied AVFormatContext(allocated by avformat_alloc_context).
    2."desktop"   : Name of the stream to open.输入流
    3.ifmt        : If non-NULL, forces a specific input format.otherwise the format is autodetected-->(format)格式
    */
    if(avformat_open_input(&pFormatCtx,"desktop",ifmt,&options)!=0){
        printf("Couldn't open input stream.\n");
//        return -1;
        exit(1);
    }

#endif
#elif defined linux
    //Linux
    AVDictionary* options = NULL;
    //Set some options
    //grabbing frame rate
    //av_dict_set(&options,"framerate","5",0);
    //Make the grabbed area follow the mouse
    //av_dict_set(&options,"follow_mouse","centered",0);
    //Video frame size. The default is to capture the full screen
    //av_dict_set(&options,"video_size","640x480",0);
    AVInputFormat *ifmt=av_find_input_format("x11grab");
    //Grab at position 10,20
    if(avformat_open_input(&pFormatCtx,":0.0+10,20",ifmt,&options)!=0){
        printf("Couldn't open input stream.\n");
        return -1;
    }
#else
    show_avfoundation_device();
    //Mac
    AVInputFormat *ifmt=av_find_input_format("avfoundation");
    //Avfoundation
    //[video]:[audio]
    if(avformat_open_input(&pFormatCtx,"1",ifmt,NULL)!=0){
        printf("Couldn't open input stream.\n");
        return -1;
    }
#endif


    /*avformat_find_stream_info
    func: read packets of a media file to get stream infomation.
    param:
    1.pFormatCtx : media file handle
    2.options : If non-NULL,an
    */
    if(avformat_find_stream_info(pFormatCtx,NULL)<0)
    {
        printf("Couldn't find stream information.\n");
//        return -1;
        exit(1);
    }
    videoindex=-1;
    for(i=0; i<pFormatCtx->nb_streams; i++)
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            videoindex=i;
            break;
        }
    if(videoindex==-1)
    {
        printf("Didn't find a video stream.\n");
//        return -1;
        exit(1);
    }
    pDc=pFormatCtx->streams[videoindex]->codec;
    pDcodec=avcodec_find_decoder(pDc->codec_id);
    if(pDcodec==NULL)
    {
        printf("Codec not found.\n");
//        return -1;
        exit(1);
    }
    if(avcodec_open2(pDc, pDcodec,NULL)<0)
    {
        printf("Could not open codec.\n");
//        return -1;
        exit(1);
    }

    pDframe=av_frame_alloc();
    pDFrameYUV=av_frame_alloc();
    //uint8_t *out_buffer=(uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, c->width, c->height));
    //avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_YUV420P, c->width, c->height);
    //SDL----------------------------
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        printf( "Could not initialize SDL - %s\n", SDL_GetError());
//        return -1;
        exit(1);
    }
    int screen_w=resize_width,screen_h=resize_height;
    const SDL_VideoInfo *vi = SDL_GetVideoInfo();
#if 1
    //Half of the Desktop's width and height.
    screen_w = vi->current_w/2;
    screen_h = vi->current_h/2;
#else
    //the Desktop's width and height.
    screen_w = vi->current_w;
    screen_h = vi->current_h;
#endif
    SDL_Surface *screen;
    screen = SDL_SetVideoMode(screen_w, screen_h, 0,0);

    printf("screen_w:%d,screen_h:%d\n",screen_w,screen_h);

    if(!screen) {
        printf("SDL: could not set video mode - exiting:%s\n",SDL_GetError());
//        return -1;
        exit(1);
    }
    SDL_Overlay *bmp;
    bmp = SDL_CreateYUVOverlay(pDc->width, pDc->height,SDL_YV12_OVERLAY, screen);
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = screen_w;
    rect.h = screen_h;
    //SDL End------------------------



#if OUTPUT_YUV420P
    FILE *fp_yuv=fopen("output.yuv","wb+");
#endif

    struct SwsContext *img_convert_ctx;
    img_convert_ctx = sws_getContext(pDc->width, pDc->height, pDc->pix_fmt, pDc->width, pDc->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
    //------------------------------
    SDL_Thread *video_tid = SDL_CreateThread(sfp_refresh_thread,NULL);
    //
    SDL_WM_SetCaption("Simplest FFmpeg Grab Desktop",NULL);
    //Event Loop
    SDL_Event event;
    int execcount = 0;
    int pktnum = 0;



    for (;;) {
        //Wait
        SDL_WaitEvent(&event);
        if(event.type==SFM_REFRESH_EVENT){

            //------------------------------
            printf("->>>>>>>>>count:%d\n",execcount++);

            if(av_read_frame(pFormatCtx, pkt)>=0){
                if(pkt->stream_index==videoindex){

                    printf("->>>>>>>>>pktnum:%d\n",pktnum++);
                    ret = avcodec_decode_video2(pDc, pDframe, &got_picture, pkt);
                    if(ret < 0){
                        printf("Decode Error.\n");
//                        return -1;
                        exit(1);
                    }

                    if(got_picture){
                        SDL_LockYUVOverlay(bmp);
                        pDFrameYUV->data[0]=bmp->pixels[0];
                        pDFrameYUV->data[1]=bmp->pixels[2];
                        pDFrameYUV->data[2]=bmp->pixels[1];
                        pDFrameYUV->linesize[0]=bmp->pitches[0];
                        pDFrameYUV->linesize[1]=bmp->pitches[2];
                        pDFrameYUV->linesize[2]=bmp->pitches[1];
                        sws_scale(img_convert_ctx, (const uint8_t* const*)pDframe->data, pDframe->linesize, 0, pDc->height, pDFrameYUV->data, pDFrameYUV->linesize);
                        //
                        //#if OUTPUT_YUV420P
                        //						int y_size=c->width*c->height;
                        //						fwrite(pFrameYUV->data[0],1,y_size,fp_yuv);    //Y
                        //						fwrite(pFrameYUV->data[1],1,y_size/4,fp_yuv);  //U
                        //						fwrite(pFrameYUV->data[2],1,y_size/4,fp_yuv);  //V
                        //#endif

                        ret = avcodec_encode_video2(pEc, pkt, pDFrameYUV, &got_output);

                        if (ret < 0)
                        {
                            printf("Error encoding frame\n");
                            exit(1);
                        }


                        if (got_output)
                        {
                            printf("Write frame %3d (size=%5d)\n", i++, pkt->size);

                            fwrite(pkt->data, 1, pkt->size, f);
                            fflush(f);
                            //av_free_packet(pkt);
                        }


                        SDL_UnlockYUVOverlay(bmp);

                        SDL_DisplayYUVOverlay(bmp, &rect);

                    }
                }
                av_free_packet(pkt);
            }else{
                //Exit Thread
                thread_exit=1;
                break;
            }
        }else if(event.type==SDL_QUIT){
            thread_exit=1;
            break;
        }

    }


    sws_freeContext(img_convert_ctx);

#if OUTPUT_YUV420P
    fclose(fp_yuv);
#endif

    SDL_Quit();

    av_free(pEframe);
    avcodec_close(pEc);
    //av_free(out_buffer);
    av_free(pDFrameYUV);
    av_free(pDframe);
    avcodec_close(pDc);
    avformat_close_input(&pFormatCtx);
}

void CapThread::run()
{
    while(1){
//        capFrame();
    }
}
