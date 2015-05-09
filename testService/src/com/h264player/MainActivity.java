package com.h264player;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.nio.ByteBuffer;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.Menu;
import android.view.View;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

public class MainActivity extends Activity implements View.OnClickListener{

	ImageView mImgLcd;
	public static Context mContext;
	Resources mRes;
	private byte[] mCopyByteMessage;
	private byte[] mGetByteMessage;
	private byte[] mGetImageNameByte;
	private int mByteLong;
	private int mCurImageSize = 0;
	private int mCurImageNameLength = 0;
	Socket mSocket;
	private static boolean mReceiveSuccessedFlag = false;
	private static boolean mReceiveSuccessedBeforeFourByteFlag = false;
	DataOutputStream mDos = null;
	DataInputStream mDis = null;
	Bitmap mImgBitmapShow;
	Message msg;
	private TextView mTextIPAddress;
	private String mIpAddress;
	private String mIpAddressInfo;
	private RelativeLayout mRelMain;
	private MyAPP mAPP = null; 
	private Thread receiveImag;
	
	H264Androidffmpeg h264;
	long decoder;
	private int screenWidth = 1920;
	private int screenHeight = 1080;
	byte[] mPixel = new byte[screenWidth * screenHeight*4];
	ByteBuffer buffer = ByteBuffer.wrap(mPixel);
	private long packageHead = 0x00000000FFFEFFFEL;
	byte bytes = 0x40;
	
	@SuppressLint("NewApi")
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		mContext = MainActivity.this;
		mRes = mContext.getResources();
//		mAPP = (MyAPP)getApplication();
//		mAPP.setHandler(mHandler);
		PublicFunction.getScreenWithAndHeight(mContext);
		mImgLcd = (ImageView)findViewById(R.id.img_lcd);
		mImgLcd.setImageResource(R.drawable.angelababy);
		mImgLcd.setClickable(true);
		mTextIPAddress = (TextView)findViewById(R.id.ip_address);
		mRelMain = (RelativeLayout)findViewById(R.id.rel_main);
		mRelMain.setSystemUiVisibility(View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);
		mRelMain.setOnClickListener(this);

		mImgLcd.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
	
				mImgLcd.setImageResource(R.drawable.angelababy);

			}
		});
		
		h264 = new H264Androidffmpeg();
		decoder =  h264.initDecoder(screenWidth,screenHeight);
		int i = mPixel.length;
		for (i = 0; i < mPixel.length; i++) {
			mPixel[i] = (byte) 0x00;
		}
		
//		mCopyByteMessage = new byte[20];
//		
//		for(i = 0;i<20;i++){
//    		mCopyByteMessage[i] = (byte) i;
//    	}
//    	int resout = h264.dalDecoder(mCopyByteMessage, mCopyByteMessage.length, mPixel);
    	
		setIpAddress();
		try{
			SocketServer();
		}catch(Throwable e){
			e.printStackTrace();
			System.out.println("SocketServer Th rowable");
			mHandler.sendEmptyMessage(6);
 			receiveImag.interrupt();
		} finally{
			
		}				
	}
	
	@Override
	protected void onResume() {
		setIpAddress();
		super.onResume();
	}
	
	private Bitmap changeBitmapSize(Bitmap bitmapOrg){
        int width = bitmapOrg.getWidth();
        int height = bitmapOrg.getHeight();
        int newWidth = 560;
        int newHeight = 400;
        float scaleWidth = ((float) newWidth) / width;
        float scaleHeight = ((float) newHeight) / height;
        Matrix matrix = new Matrix();
        matrix.postScale(scaleWidth, scaleHeight);
//        matrix.postRotate(45);     
        bitmapOrg = Bitmap.createBitmap(bitmapOrg, 0, 0,
        width, height, matrix, true);
        return bitmapOrg;
	}
	
	private void SocketServer(){
		mByteLong = 0;
		mCurImageSize = 0;
		mCurImageNameLength = 0;
		mHandler.sendEmptyMessage(5);
		mReceiveSuccessedFlag = false;
		if(mIpAddress != null){
			receiveImag = new Thread(run);
			receiveImag.start();
		}
	}
	
	MyHandler mHandler = new MyHandler();
	
	private void setIpAddress(){
		mIpAddressInfo = mRes.getString(R.string.ipaddress);
		if(mTextIPAddress.getVisibility() != View.VISIBLE){
			mTextIPAddress.setVisibility(View.VISIBLE);
		}
   		 try {
   			mIpAddress = PublicFunction.getLocalIPAddress();
   			mIpAddressInfo = String.format(mIpAddressInfo, mIpAddress);
		} catch (SocketException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
   		 
   		 if(mIpAddress != null){
   			 mHandler.removeMessages(7);
   			 mTextIPAddress.setText(mIpAddressInfo);
		} else {
			mHandler.sendEmptyMessageDelayed(7, 1000);
			mTextIPAddress.setText("未获取正确的IP地址，请检查后在查看。");
		}	
	}
	Runnable run = new Runnable() {
		
		@Override
		public void run() {
			 ServerSocket ss = null;
	         try {
	        	 mHandler.sendEmptyMessage(2);
				 ss = new ServerSocket();
	        	 ss.bind(new InetSocketAddress(mIpAddress,16689)); 

		         //服务器接收到客户端的数据后，创建与此客户端对话的Socket
	        	 mSocket = ss.accept();
	        	 mHandler.sendEmptyMessage(3);
		         //用于向客户端发送数据的输出流
		         mDos = new DataOutputStream(mSocket.getOutputStream());
		         //用于接收客户端发来的数据的输入流
		         mDis = new DataInputStream(mSocket.getInputStream());
		         while(!mReceiveSuccessedFlag){
		        	 try{ 
			        	 mSocket.sendUrgentData(0xFF); 
			        	 }catch(Exception ex){ 
			        		 System.out.println("线程连接异常  mReceiveSuccessedFlag" + Thread.currentThread().getName());
			        		 try {
			     				if (null != mDis)
			     					mDis.close();
			     				if (null != mDos)
			     					mDos.close();
			     				if (null != ss){
			     					ss.close();
			     				}
			     			} catch (IOException ee) {
			     				ee.printStackTrace();
			     			}
			     			mHandler.sendEmptyMessage(6);
			     			receiveImag.interrupt();
			     			receiveImag = null;
			     			mReceiveSuccessedFlag = true;
			     			break;
			        	 }
		        	 receiveMessage(mDis,mSocket,mDos);
	        	 }
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				if(!mReceiveSuccessedFlag){
					System.out.println("线程连接异常   run " + Thread.currentThread().getName());
					mHandler.sendEmptyMessage(6);
	     			receiveImag.interrupt();
				}
				receiveImag = null;
				try {
     				if (null != mDis)
     					mDis.close();
     				if (null != mDos)
     					mDos.close();
     				if (null != ss){
     					ss.close();
     				}
     			} catch (IOException ee) {
     				ee.printStackTrace();
     			}
			}
			
		}
	};
	
	long tempTotalSize = 0;
	long readPackageHead = 0;
	long readPackageContent = 0;
public void receiveMessage(DataInputStream input, Socket s, DataOutputStream output) throws SocketException{
		
		int numRead = 0;
		try {
			int tempBytelength = input.available();
			if(!mReceiveSuccessedBeforeFourByteFlag){
	            if(tempBytelength >= 100){
	            	tempTotalSize = input.readLong();
	            	readPackageHead = input.readLong();
	            	readPackageContent = input.readLong();
	            	if(readPackageHead != 8 || readPackageContent != packageHead){
	            		return;
	            	}
	            	mCurImageSize = (int)tempTotalSize - 24;
	            	mGetByteMessage = new byte[mCurImageSize];
	            	mReceiveSuccessedBeforeFourByteFlag = true;
	            }
	            return;
			}else {
					numRead=input.read(mGetByteMessage, mByteLong, mCurImageSize - mByteLong);
					if(mByteLong < mCurImageSize){
						mByteLong += numRead;
						return;
					} else if(mByteLong >= mCurImageSize){
//						PublicFunction.writeFile(mContext, "savefilelog.txt", "TotalBytes:" + tempTotalSize +"\n");
//						PublicFunction.writeFile(mContext, "savefilelog.txt", "TotalBytes:" + tempTotalSize);
//						PublicFunction.writeFile(mContext, System.currentTimeMillis()+"file.txt", mGetByteMessage);
						mByteLong = 0;
						mCurImageSize = 0;
						mCurImageNameLength = 0;
						tempBytelength = 0;
						msg = new Message();
						msg.what = 1;
						mHandler.sendMessage(msg);
						mReceiveSuccessedBeforeFourByteFlag = false;
						
					}
			}
		} catch (IOException e) {
			Log.writeErroLogToFile("socket input output异常"
					+ Thread.currentThread().getName());
				Log.writeErroLogToFile("线程连接异常" + Thread.currentThread().getName());
				System.out.println("线程连接异常  receiveMessage" + Thread.currentThread().getName());
			
			try {
				if (null != input)
					input.close();
				if (null != output)
					output.close();
				if (null != s)
					s.close();
			} catch (IOException ee) {
				ee.printStackTrace();
			}
			mHandler.sendEmptyMessage(6);
 			receiveImag.interrupt();
		} 
	}
		
		@Override
		public void finish() {
			super.finish();
			android.os.Process.killProcess(android.os.Process.myPid());
		}

		/** 
	     * 自己实现 Handler 处理消息更新UI 
	     *  
	     * @author mark 
	     */  
	    final class MyHandler extends Handler {  
	        @Override  
	        public void handleMessage(Message msg) {  
	            super.handleMessage(msg);  
	            if(msg.what == 1){
	            	synchronized (mGetByteMessage) {
	            		mCopyByteMessage = new byte[mGetByteMessage.length];
	            		mCopyByteMessage = mGetByteMessage.clone();
					}
	            	for(int i = 0;i<20;i++){
	            		mCopyByteMessage[i] = (byte) i;
	            	}
	            	int resout = h264.dalDecoder(mCopyByteMessage, mCopyByteMessage.length, mPixel);
					mImgBitmapShow = PublicFunction.bytesToBimap(mPixel);
					if(mImgBitmapShow != null){
						mImgLcd.setImageBitmap(mImgBitmapShow);
					}
				} else if(msg.what == 2){
					setIpAddress();
				} else if(msg.what == 3){
					mTextIPAddress.setVisibility(View.GONE);
				} else if(msg.what == 5){
					mImgLcd.setImageResource(R.drawable.angelababy);
				} else if(msg.what == 6){
					 SocketServer();
				}else if(msg.what == 7){
					setIpAddress();
					SocketServer();
				}
	        }  
	}  
		@Override
		public void onClick(View v) {
//			int i = mRelMain.getSystemUiVisibility();  
//			  if (i == View.SYSTEM_UI_FLAG_HIDE_NAVIGATION) {  
//				  mRelMain.setSystemUiVisibility(View.SYSTEM_UI_FLAG_VISIBLE);  
//			  } else if (i == View.SYSTEM_UI_FLAG_VISIBLE){  
//				  mRelMain.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LOW_PROFILE);  
//			  } else if (i == View.SYSTEM_UI_FLAG_LOW_PROFILE) {  
//				  mRelMain.setSystemUiVisibility(View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);  
//			  }   
		}

}
