package com.h264player;

import android.app.Application;

public class CrashAplication extends Application{

	@Override  
    public void onCreate() {  
        super.onCreate();  
        CrashHandler crashHandler = CrashHandler.getInstance();  
        crashHandler.init(this);  
    }  
	
}
