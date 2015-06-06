package com.h264sdlplayer;

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
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

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
	Bitmap mImgBitmapShow;
	Message msg;
	private TextView mTextIPAddress;
	private String mIpAddress;
	private String mIpAddressInfo;
	private RelativeLayout mRelMain;
	private MyAPP mAPP = null; 
	private Thread receiveImag;
	
	long decoder;
	private int screenWidth = 1920;
	private int screenHeight = 1080;
	byte[] mPixel = new byte[screenWidth * screenHeight*4];
	ByteBuffer buffer = ByteBuffer.wrap(mPixel);
	private long packageHead = 0x00000000FFFEFFFEL;
	byte bytes = 0x40;
	private static String curStartTimeFileName;
	
	//sdl add
	private Bitmap mBitmap;
	private int mSecs = 0;
	private int ulCount = 0;
	private H264Jni jniInterface;
	
	@SuppressLint("NewApi")
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		mContext = MainActivity.this;
		mRes = mContext.getResources();
		curStartTimeFileName = PublicFunction.getStringDate();
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
		
//		h264 = new H264Androidffmpeg();
//		decoder =  h264.initDecoder(screenWidth,screenHeight);
		// *****************************sdl add **********************************
		jniInterface = new H264Jni();
        mBitmap = Bitmap.createBitmap(720, 480, Bitmap.Config.ARGB_8888);
        jniInterface.openFile();
        
        Button btn = (Button)findViewById(R.id.frame_adv);
        btn.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				jniInterface.initScreen(720,480);
				int getStatus = jniInterface.ProcPacketsDisplay(initByte(),100);
//				drawFrame(mBitmap);
//				saveBitmap(mBitmap);
				Toast.makeText(mContext, getStatus+"", Toast.LENGTH_LONG).show();
//				ImageView i = (ImageView)findViewById(R.id.frame);
//				i.setImageBitmap(mBitmap);
			}
		});
        
        Button btn_fwd = (Button)findViewById(R.id.frame_fwd);
        btn_fwd.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				for(ulCount = 0; ulCount<1;ulCount++)
				{
					mSecs += 5;
					jniInterface.drawFrameAt(mBitmap, mSecs);
					jniInterface.drawFrame(mBitmap);
//					ImageView i = (ImageView)findViewById(R.id.frame);
//					i.setImageBitmap(mBitmap);			
				}
			}
		});
        
        Button btn_back = (Button)findViewById(R.id.frame_back);
        btn_back.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				mSecs -= 5;
				jniInterface.drawFrameAt(mBitmap, mSecs);
				jniInterface.drawFrame(mBitmap);
//				ImageView i = (ImageView)findViewById(R.id.frame);
//				i.setImageBitmap(mBitmap);
			}
		});
    
		setIpAddress();
		try{
			SocketServer();
		}catch(Throwable e){
			e.printStackTrace();
			PublicFunction.writeFile(mContext, getLogFileName(), "SocketServer Th rowable" + Thread.currentThread().getName());
			System.out.println("SocketServer Th rowable");
			mHandler.sendEmptyMessage(6);
			if(receiveImag != null){
				receiveImag.interrupt();
				receiveImag = null;
			}
 			mReceiveSuccessedFlag = true;
		} finally{
			
		}				
	}
	
	@Override
	protected void onResume() {
		setIpAddress();
		super.onResume();
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
		ServerSocket ss = null;
		DataOutputStream mDos = null;
		DataInputStream mDis = null;
		@Override
		public void run() {
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
		         mHandler.sendEmptyMessageDelayed(8, 5000);
		         while(!mReceiveSuccessedFlag){
		        	 receiveMessage(mDis,mSocket,mDos);
	        	 }
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				if(!mReceiveSuccessedFlag){
					PublicFunction.writeFile(mContext, getLogFileName(), "线程连接异常   run " + Thread.currentThread().getName());
					System.out.println("线程连接异常   run " + Thread.currentThread().getName());
					mHandler.sendEmptyMessage(6);
					if(receiveImag != null){
						receiveImag.interrupt();
					}
				}
				mReceiveSuccessedFlag = true;
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

public void receiveMessage(DataInputStream input, Socket s, DataOutputStream output) throws SocketException{
		
		int numRead = 0;
		try {
			int tempBytelength = input.available();
			if(!mReceiveSuccessedBeforeFourByteFlag){
	            if(tempBytelength >= 100){
	            	long tempTotalSize = input.readLong();
	            	long readPackageHead = input.readLong();
	            	long readPackageContent = input.readLong();
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
			PublicFunction.writeFile(mContext, getLogFileName(), "socket input output异常" + Thread.currentThread().getName());
			PublicFunction.writeFile(mContext, getLogFileName(), "线程连接异常" + Thread.currentThread().getName());
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
			mReceiveSuccessedFlag = true;
			if(receiveImag != null){
				receiveImag.interrupt();
				receiveImag = null;
			}
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
	            	//********************需要传的字节数组。****************************
	            	int getStatus = H264Jni.ProcPacketsDisplay(mCopyByteMessage,mCopyByteMessage.length);
//	            	int resout = h264.dalDecoder(mCopyByteMessage, mCopyByteMessage.length, mPixel);
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
				} else if (msg.what == 8){
		        	 try{ 
			        	 mSocket.sendUrgentData(0xFF); 
			        	 mHandler.sendEmptyMessageDelayed(8, 5000);
		        	 }catch(Exception ex){
		        		 mHandler.removeMessages(8);
		        		 PublicFunction.writeFile(mContext, getLogFileName(), "线程连接异常  mReceiveSuccessedFlag" + Thread.currentThread().getName());
		        		 System.out.println("线程连接异常  mReceiveSuccessedFlag" + Thread.currentThread().getName());
//		        		 try {
//		     				if (null != mDis)
//		     					mDis.close();
//		     				if (null != mDos)
//		     					mDos.close();
//		     				if (null != ss){
//		     					ss.close();
//		     				}
//		     			} catch (IOException ee) {
//		     				ee.printStackTrace();
//		     			}
		        		mReceiveSuccessedFlag = true;
		     			mHandler.sendEmptyMessage(6);
		     			if(receiveImag != null){
			     			receiveImag.interrupt();
			     			receiveImag = null;
		     			}
		     			
		        	 }
				}
	        }  
	}  
	
    private byte[] initByte(){
    	byte[] array = new byte[100];
    	for(byte i=0;i<array.length;i++){
    		array[i] = i;
    	}
    	return array;
    }
	    
	public static String getLogFileName(){
		return curStartTimeFileName + ".txt";
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
