package com.h264sdlplayer;

import android.graphics.Bitmap;

public class H264Jni {
	
    static {
    	System.loadLibrary("ffmpegutils");
    }
    
	public static native void openFile();
	public static native void drawFrame(Bitmap bitmap);
	public static native void drawFrameAt(Bitmap bitmap, int secs);
	public static native void initScreen(int width, int height);
	public static native int ProcPacketsDisplay(byte[] array,int len);
}
