package com.example.h264dec;

import java.io.File;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;
import java.util.Vector;


import android.os.Bundle;
import android.os.Environment;
import android.app.Activity;
import android.content.Intent;
import android.content.res.Configuration;
import android.hardware.Camera;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.AdapterView.OnItemClickListener;

public class MainActivity extends Activity implements OnItemClickListener {
	private TextView _textViewPrompt;
	private ListView _listView;
	private static final String TAG = "MENU SELECTOR";
	static{
		try {
			System.loadLibrary("avutil-linphone-arm");
			System.loadLibrary("swscale-linphone-arm");
			System.loadLibrary("avcodec-linphone-arm");
			System.loadLibrary("ndk_h264dec");
	
		} catch (UnsatisfiedLinkError e) {
			e.printStackTrace();
		}
	}
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);
        setScreenWidthAndHeight();
        _textViewPrompt = (TextView)findViewById(R.id.textViewMenuPrompt);
        _listView = (ListView)findViewById(R.id.listViewMenu);
        
        	_listView.setScrollContainer(true);
        	_listView.setScrollingCacheEnabled(false);
        	_listView.setOnItemClickListener(this);

         
            	// Don't perform any operations if the directory is not writeable.	
        		if (!validateStorageDirectory()) {
        			_textViewPrompt.setText("Cannot read from storage directory");
        	    	return;
        		}

        		_textViewPrompt.setText("Below is the h264 files in " + storagePath);
            	Vector<String> files = new Vector<String>(); 
            	
            	File dataPath = new File(getStorageDirectory());
            	Log.e("getStorageDirectory()==", getStorageDirectory());
            	String[] fileList = dataPath.list();
            	for (int i = 0; i < fileList.length; i++) {
            		String fileName = fileList[i];
            		files.add(fileName);
//            		if (fileName.endsWith("h264") &&
//            			new File(dataPath.getAbsolutePath() + "/" + fileName).isFile()) {
//            	    	String[] parts = fileName.split("\\.");
//            	    	for (int j = 0; j < parts.length; j++) {
//            	    		if (parts[j].contains("x")) {
//            	    			String[] dims = parts[j].split("x");
//            	    			if (dims.length == 2) {
//                        			files.add(fileName);
//                    	    		break;
//            	    			}
//            	    		}
//            	    	}
//            		}
            	}

            	if (files.size() > 0) {
            		Object fileArray[] = files.toArray();
            		Arrays.sort(fileArray);
    	        	ArrayAdapter<Object> aAdapter = new ArrayAdapter<Object>(this, R.layout.menuselector_listitem, fileArray);
    	        	_listView.setAdapter(aAdapter);
    	        	_listView.setOnItemClickListener(this);
            	} else {
    	        	_textViewPrompt.setText("No files in storage directory.");
            	}
	}
	
	private void setScreenWidthAndHeight(){
		DisplayMetrics  dm = new DisplayMetrics();  
	      //取得窗口属性  
	      getWindowManager().getDefaultDisplay().getMetrics(dm);  
	      //窗口的宽度  
	      int screenWidth = dm.widthPixels;  
	      //窗口高度  
	      int screenHeight = dm.heightPixels;  
	      PublicFunction.setScreenHeight(screenHeight);
	      PublicFunction.setScreenWidth(screenWidth);
	}
	private static String storagePath = Environment.getExternalStorageDirectory().getPath() +  "/h264";
	public static String getPathInStorageDirectory(String path) {
		return storagePath + "/" + path;
	}
	
	public static String getStorageDirectory() {
		return storagePath;
	}

	public static boolean validateStorageDirectory() {
		File dir = new File(getStorageDirectory());
		if (dir.exists() || dir.mkdir()) {
        	return true;
		}
        return false;
	}
	int backCnt = 0;
	private Timer exitTimer = null;
	private static final int EXIT_TIMEOUT = 3*1000;//3s
	
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if(keyCode == KeyEvent.KEYCODE_BACK ){
            	if(backCnt++ == 0){
    				Toast.makeText(this, "再点一次退出应用", Toast.LENGTH_SHORT).show();
    				  if(exitTimer!=null){
    					  exitTimer.cancel();
    					  exitTimer = null;
    					}
    				  exitTimer = new Timer();
    				  exitTimer.schedule(new TimerTask() {
    					
    					@Override
    					public void run() {
    						// TODO Auto-generated method stub
    						backCnt = 0;
    					}
    				}, EXIT_TIMEOUT);
    			}else{
    				//LogHelper.logPrint(RegisterActivity.class, "stop services return "+ stopService(new Intent(getApplicationContext(), RegisterServices.class)));
    				android.os.Process.killProcess(android.os.Process.myPid());
    			}
    			return true;
			
		}else{
			return super.onKeyDown(keyCode, event);
		}
	}
	@Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        Log.i("MainActivity", "onConfigurationChanged");
        if(newConfig.orientation==Configuration.ORIENTATION_LANDSCAPE){
           Log.d("MainActivity","current screen is portrait");
        }else{
        Log.d("MainActivity","current screen is landscape");
        }
    }
	public void onItemClick(AdapterView<?> arg0, View view, int arg2, long arg3) {
		
			Intent resultIntent = null;
			resultIntent = new Intent(this, ActivityYuvOrRgbViewer.class);
//			resultIntent = new Intent(this, ActivityH264Decoder.class);

			TextView tView = (TextView)view;
			String fileName = tView.getText().toString();
        	resultIntent.putExtra("h264.fileName", getPathInStorageDirectory(fileName));
			startActivity(resultIntent);
			return;

	}
}
