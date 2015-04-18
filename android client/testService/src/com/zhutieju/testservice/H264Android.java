package com.zhutieju.testservice;

public class H264Android {
	static {
		System.loadLibrary("avcodec-56");
		System.loadLibrary("avdevice-56");
		System.loadLibrary("avfilter-5");
		System.loadLibrary("avformat-56");
		System.loadLibrary("avutil-54");
		System.loadLibrary("swresample-1");
		System.loadLibrary("swscale-3");
		System.loadLibrary("H264Androidffmpeg22");
	}
	
	public native int initDecoder(int width,int heigth);
	public native int dalDecoder(byte[] in,int size,byte[] out);
	public native int releaseDecoder();
}
