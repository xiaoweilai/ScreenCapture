/************************************************/
/*声 明:  @db.com                                */
/*XXXXX:                                         */
/*XXXXX:                                         */
/*XXXXX:                                         */
/*XXXXX:                                         */
/*功 能:捕屏线程                                   */
/*author :wxj                                    */
/*email  :wxjlmr@126.com                         */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
/*更新记录:                                        */
/*                                                */
/*************************************************/
/**
 * 最简单的基于FFmpeg的AVDevice例子（屏幕录制）
 * Simplest FFmpeg Device (Screen Capture)
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


#include <stdio.h>
#include <math.h>
#include <QDebug>
#include <QPixmap>
#include <QPainter>
#include <QCursor>
#include <QtCore>
#include <QWidget>
#include <QGraphicsItem>
#include <QMessageBox>
#include <QRegExp>
#include <windows.h>
#include "capthread.h"




int thread_exit=0;

/************************************************/
/*函 数:sfp_refresh_thread                       */
/*入 参:opaque-忽略                               */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:向SDL中推送刷新事件，间隔40ms                */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
int sfp_refresh_thread(void *opaque)
{
    while (thread_exit==0) {
        SDL_Event event;
        event.type = SFM_REFRESH_EVENT;
        SDL_PushEvent(&event);
        SDL_Delay(40);
    }
    printf("sfp_refresh_thread quit!!");
    return 0;
}

/************************************************/
/*函 数:sendSDLQuit                              */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:向SDL中推送退出事件                          */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void CapThread::sendSDLQuit()
{
    SDL_Event event;
    event.type = SDL_QUIT;
    SDL_PushEvent(&event);
    SDL_Delay(40);
}

/************************************************/
/*函 数:show_dshow_device                        */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:directShow设备列表                         */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
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

/************************************************/
/*函 数:show_avfoundation_device                 */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:Show AVFoundation Device                  */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
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

/************************************************/
/*函 数:CapThread                                */
/*入 参:width-分辨率宽度，height-分辨度高度，parent-父类*/
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:捕屏线程构造函数                             */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
CapThread::CapThread(int width, int height,QObject *parent)
    : QThread(parent)
{
    arrayNetSize.clear();
    arrayNetData.clear();
    m_threadstate = STAT_THREAD_RUNNING;
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
#ifdef STREAMTOFILE
    f = fopen("test.mpg", "wb");
    if (!f)
    {
        printf("open output file failed\n");
        exit(1);
    }
#endif
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
    {
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            videoindex=i;
            break;
        }
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
#define SHOWSIZEDIV 5  //DIV of the Desktop's width and height.

    //Half of the Desktop's width and height.
    screen_w = vi->current_w/SHOWSIZEDIV;
    screen_h = vi->current_h/SHOWSIZEDIV;


#if 1
    SDL_Surface *screen;
    /* 此处弹出黑屏框 */
    screen = SDL_SetVideoMode(screen_w, screen_h, 0,0);


    printf("screen_w:%d,screen_h:%d\n",screen_w,screen_h);

    if(!screen) {
        printf("SDL: could not set video mode - exiting:%s\n",SDL_GetError());
        //        return -1;
        exit(1);
    }
#endif

#if  1//with screen
    bmp = SDL_CreateYUVOverlay(pDc->width, pDc->height,SDL_YV12_OVERLAY, screen);
#else
    bmp = SDL_CreateYUVOverlay(pDc->width, pDc->height,SDL_YV12_OVERLAY, NULL);
#endif

    rect.x = 0;
    rect.y = 0;
    rect.w = screen_w;
    rect.h = screen_h;
    //SDL End-----------------------
    //    YCbCr 4:2:0
    //img_convert_ctx = sws_getContext(pDc->width, pDc->height, pDc->pix_fmt, pDc->width, pDc->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
    img_convert_ctx = sws_getContext(pDc->width, pDc->height, pDc->pix_fmt, pDc->width, pDc->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
    //------------------------------
    video_tid = SDL_CreateThread(sfp_refresh_thread,NULL);
    //
    SDL_WM_SetCaption("FFmpeg Grab Desktop",NULL);

    execcount = 0;
    pktnum = 0;
}

void CapThread::run()
{
    //Event Loop
    SDL_Event event;

    //    qtime计算时间，耗时
    QTime time;
    time.start(); //开始计时，以ms为单位
    static float time_total = 0.0;



    while(1)
    {
        int time_Diff = time.elapsed(); //返回从上次start()或restart()开始以来的时间差，单位ms
        //以下方法是将ms转为s
        float f = time_Diff / 1000.0;
        time_total += f;

        //    qDebug() << "time elaspe:" <<time_Diff <<"ms";
        qDebug() << "Total time elaspe:" <<time_total <<"s";

        if(STAT_THREAD_STOPED == GetThreadFlag())
        {
            qDebug() << "STAT_THREAD_STOPED!";
            Sleep(1000);
            continue;
        }
        if(STAT_THREAD_QUIT == GetThreadFlag())
        {
            qDebug() << "STAT_THREAD_QUIT!";
            break;
        }
        //Wait
        SDL_WaitEvent(&event);
        if(event.type==SFM_REFRESH_EVENT){

            //------------------------------
#ifdef DEBUG
            qDebug("->>>>>>>>>count:%d\n",execcount++);
#endif

            if(av_read_frame(pFormatCtx, pkt)>=0){
                if(pkt->stream_index==videoindex){
#ifdef DEBUG
                    qDebug("->>>>>>>>>pktnum:%d\n",pktnum++);
#endif
                    ret = avcodec_decode_video2(pDc, pDframe, &got_picture, pkt);
                    if(ret < 0){
                        qDebug("Decode Error.\n");
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

                        ret = avcodec_encode_video2(pEc, pkt, pDFrameYUV, &got_output);

                        if (ret < 0)
                        {
                            qDebug("Error encoding frame\n");
                            exit(1);
                        }


                        if (got_output)
                        {
                            SendPkgData(pkt);
#ifdef DEBUG
                            qDebug("Write frame %3d (size=%5d)\n", i++, pkt->size);
#endif

#ifdef STREAMTOFILE
                            fwrite(pkt->data, 1, pkt->size, f);
                            fflush(f);
#endif
                            av_free_packet(pkt);
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

    qDebug() << "release STAT_THREAD_QUIT!";
    qDebug() << "release resource,free capthread resource and quit!";
    //SDL_KillThread(video_tid);
    thread_exit=1;
    sws_freeContext(img_convert_ctx);
    SDL_Quit();
    av_free(pEframe);
    avcodec_close(pEc);
    //    av_free(out_buffer);
    av_free(pDFrameYUV);
    av_free(pDframe);
    avcodec_close(pDc);
    avformat_close_input(&pFormatCtx);
#ifdef STREAMTOFILE
    fclose(f);
#endif

    qDebug() << "capthread free End";
}

//网络发送数据流
int CapThread::SendPkgData(AVPacket *pkt)
{
    #ifdef DEBUG
    printf("Write pkt addr:%p \n", pkt);
    #endif

    if(pkt)
    {
#if 1
        //*************************写入缓冲区************************//
        //数据源
        QByteArray byteArray;
        //缓冲区绑定数据源
        QBuffer buffer(&byteArray);
        //只写模式打开缓冲区
        buffer.open(QIODevice::WriteOnly);

        //写入缓冲区
        qint64 a = buffer.write((const char*)pkt->data,pkt->size);
        //关闭缓冲区
        buffer.close();

#ifdef DEBUG
        fprintf(stdout,"-->>data size:%d\n",byteArray.count());
        fprintf(stdout,"-->>a size   :%d\n",a);
#endif
        arrayNetSize.append(pkt->size);
        arrayNetData.append(byteArray);
//        buffer.close();
//        bytearry.resize(0);

#else

        outBlock.resize(0);
        QDataStream sendOut(&outBlock, QIODevice::WriteOnly);
        sendOut.resetStatus();
        sendOut.setVersion(QDataStream::Qt_4_0);
        TotalBytes = pkt->size; //大小


        qDebug() << "quint32:" << sizeof(quint32);//4
        qDebug() << "quint64:" << sizeof(quint64);//8
        //数据头 + 8字节
        sendOut <<(qint32)0xFFFEFFFE<< qint64(0);//加四个字节头，占8个字节空位
        qDebug() << "outblock size:" << outBlock.size();
        sendOut << pkt->data;//加数据(n)

        qDebug() << "outblock size:" << outBlock.size();

        TotalBytes = pkt->size; //大小
        qDebug() << "TotalBytes:" << TotalBytes;

        //TotalBytes为总数据长度，即（数据量长度+文件名长度+文件名）
        TotalBytes += outBlock.size(); //加上图片名称长度

        qDebug() << "outblock size:" << outBlock.size();
        qDebug() << "TotalBytes:" << TotalBytes;


        sendOut.device()->seek(4);
        //总字节(文件大小 + 8字节 + 文件名) ，
        sendOut << (qint64)TotalBytes << qint64((outBlock.size() - sizeof(qint64)*2));
        qDebug() << "outblock size:" << outBlock.size();



        printf("Write TotalBytes :%d \n", TotalBytes);
        if(p_tcpClient)
        {
            printf("Write outBlock \n");
            p_tcpClient->write(outBlock);
        }
#endif
    }
}


CapThread::~CapThread()
{
    qDebug() << "~CapThread()~CapThread()~CapThread()";
}

void CapThread::capFrame()
{

}
void CapThread::SetThreadFlag(quint8 flag)
{
    m_threadstate = flag;
}

quint8 CapThread::GetThreadFlag(void)
{
    return m_threadstate;
}

void CapThread::SetStartThread()
{
    SetThreadFlag(STAT_THREAD_RUNNING);
}
void CapThread::SetStopThread()
{
    SetThreadFlag(STAT_THREAD_STOPED);
}
void CapThread::SetQuitThread()
{
//    SetThreadFlag(STAT_THREAD_QUIT);
    sendSDLQuit();
}


