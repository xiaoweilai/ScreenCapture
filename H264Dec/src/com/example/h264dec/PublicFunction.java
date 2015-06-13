package com.example.h264dec;


public class PublicFunction {
	
	private static int mScreenWidth;
	private static int mScreenHeight;

	// 获取屏幕的宽度
	public static int getScreenWidth() {	
		return mScreenHeight;    
	}

	// 获取屏幕的高度
	public static int getScreenHeight() {		
		return mScreenWidth;
	}
	
	public static void setScreenWidth(int width){
		mScreenWidth = width;
	}
	
	public static void setScreenHeight(int height){
		mScreenHeight = height;
	}
}
