package com.zhutieju.testservice;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;
import java.net.Inet4Address;
import java.net.InetSocketAddress;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.view.WindowManager;

public class PublicFunction {
private static int curImageIndex = 0;
	public static int toInt(byte[] bRefArr) {
	    int iOutcome = 0;
	    byte bLoop;

	    for (int i = 0; i < bRefArr.length; i++) {
	        bLoop = bRefArr[i];
	        iOutcome += (bLoop & 0xFF) << (8 * i);
	    }
	    return iOutcome;
	}
	
	public static byte[] toByteArray(int iSource, int iArrayLen) {
	    byte[] bLocalArr = new byte[iArrayLen];
	    for (int i = 0; (i < 4) && (i < iArrayLen); i++) {
	        bLocalArr[i] = (byte) (iSource >> 8 * i & 0xFF);
	    }
	    return bLocalArr;
	}
	
	public static Bitmap bytesToBimap(byte[] b){   
        if(b.length!=0){
            return BitmapFactory.decodeByteArray(b, 0, b.length);   
        } else {
            return null;   
        }   
  } 
	
	public static byte[] long2bytes(long num) {
		byte[] b = new byte[8];
		for (int i=0;i<8;i++) {
			b[i] = (byte)(num>>>(56-(i*8)));
		}
		return b;
	}
	
	public static String getLocalIPAddress() throws SocketException{  
	    for(Enumeration<NetworkInterface> en = NetworkInterface.getNetworkInterfaces();en.hasMoreElements();){  
	        NetworkInterface intf = en.nextElement();  
	        for(Enumeration<InetAddress> enumIpAddr = intf.getInetAddresses(); enumIpAddr.hasMoreElements();){  
	            InetAddress inetAddress = enumIpAddr.nextElement();  
	            if(!inetAddress.isLoopbackAddress() && (inetAddress instanceof Inet4Address)){  
	                return inetAddress.getHostAddress().toString();  
	            }  
	        }  
	    }  
	    return null;  
	}
	
	public static int[] getScreenWithAndHeight(Context context){
		WindowManager wm = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);
		int wh[] = new int[2];
		int width = wm.getDefaultDisplay().getWidth();
		int height = wm.getDefaultDisplay().getHeight();
		wh[0] = width;
		wh[1] = height;
		System.out.println("wh[0] = " + wh[0] + "wh[1] = " + wh[1]);
		return wh;
	}
	
	public static void saveBitmap(Bitmap bm) {
			File f = new File("/sdcard/test");
			if (!f.exists()) {
			f.mkdir();
			}
		  f = new File("/sdcard/test/", "test"+ curImageIndex +".jpg");
		  curImageIndex++;

		  try {
		   FileOutputStream out = new FileOutputStream(f);
		   bm.compress(Bitmap.CompressFormat.JPEG, 100, out);
		   out.flush();
		   out.close();
		  } catch (FileNotFoundException e) {
		   // TODO Auto-generated catch block
		   e.printStackTrace();
		  } catch (IOException e) {
		   // TODO Auto-generated catch block
		   e.printStackTrace();
		  }
		 }
}
