package com.h264player;

import com.h264player.MainActivity.MyHandler;

import android.app.Application;
import android.os.Handler;

public class MyAPP extends Application {  
    // �������  
    private MyHandler handler = null;  
      
    // set����  
    public void setHandler(MyHandler mHandler) {  
        this.handler = mHandler;  
    }  
      
    // get����  
    public MyHandler getHandler() {  
        return handler;  
    }  
}  