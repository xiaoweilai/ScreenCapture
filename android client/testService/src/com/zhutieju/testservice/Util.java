package com.zhutieju.testservice;

import java.util.ArrayList;

import android.graphics.Bitmap;
/**
 * 存储集合的工具类
 * @author Administrator
 *
 */
public class Util {
	public static ArrayList<Bitmap> list = new ArrayList<Bitmap>();
	/**
	 * 设置或获取位图
	 * @param type	0表示设置,1表示获取
	 * @param mBitmap
	 * @return
	 */
	public static synchronized Bitmap setOrgetBitmap(int type,Bitmap mBitmap) {
		switch (type) {
		case 0:
			list.add(mBitmap);
			return null;
		case 1:
			Bitmap bitmap = list.get(0);
			list.remove(0);
			return bitmap;
		}
		return null;
	}
}
