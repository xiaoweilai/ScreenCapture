package com.example.h264dec;


public class PublicFunction {
	
	private static int mScreenWidth;
	private static int mScreenHeight;

	// ��ȡ��Ļ�Ŀ��
	public static int getScreenWidth() {	
		return mScreenHeight;    
	}

	// ��ȡ��Ļ�ĸ߶�
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
