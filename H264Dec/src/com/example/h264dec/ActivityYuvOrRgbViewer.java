package com.example.h264dec;


import com.example.h264dec.display.AndroidVideoWindowImpl;

import android.app.Activity;
import android.content.res.Configuration;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.Window;
import android.view.WindowManager;

public class ActivityYuvOrRgbViewer extends Activity {
	private SurfaceHolder 	 _outputSurfaceHolder = null;
	//private VideoDisplayView _outputSurfaceView = null;
	GLSurfaceView view = null;
	AndroidVideoWindowImpl mVideoWindow;
	private static final String TAG = "ACTIVITY YUV VIEWER";
	private String _fileName = null;
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.e("ActivityYuvOrRgbViewer", "onCreate");
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);

        setContentView(R.layout.yuv_viewer);
        _fileName = getIntent().getStringExtra("h264.fileName");
        view = (GLSurfaceView) findViewById(R.id.surfaceOutputView);
        view.setZOrderOnTop(false);
        
        mVideoWindow = new AndroidVideoWindowImpl(view);
        mVideoWindow
		.setListener(new AndroidVideoWindowImpl.VideoWindowListener() {					
			public void onVideoRenderingSurfaceDestroyed(AndroidVideoWindowImpl vw) {
				 Log.e("ActivityYuvOrRgbViewer", "onVideoRenderingSurfaceDestroyed");
				stopVideo();
			};
			
			public void onVideoRenderingSurfaceReady(AndroidVideoWindowImpl vw, SurfaceView sv) {
				setVideoWindowId(vw,_fileName);
				// set device rotation too
				/*setDeviceRotation(rotationToAngle(getWindowManager().getDefaultDisplay()
						.getRotation()), nativeObj);*/
			}
		}); 

        mVideoWindow.init();
    }
	native void setVideoWindowId(Object wid,String path);
	native void stopVideo();
	protected void onStart(){
		super.onStart();
		Log.e("ActivityYuvOrRgbViewer", "onStart");
	}
	@Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        Log.i("ActivityYuvOrRgbViewer", "onConfigurationChanged");
        if(newConfig.orientation==Configuration.ORIENTATION_LANDSCAPE){
           Log.d("ActivityYuvOrRgbViewer","current screen is landscape");
        }else{
        Log.d("ActivityYuvOrRgbViewer","current screen is portrait");
        }
    }
	protected void onStop(){
		super.onStop();
		Log.e("ActivityYuvOrRgbViewer", "onStop");
	}
    protected void onDestroy() {
    	Log.e("ActivityYuvOrRgbViewer", "onDestroy");
    	super.onDestroy();
    }

}