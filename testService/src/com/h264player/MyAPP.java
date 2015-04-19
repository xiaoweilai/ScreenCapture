package com.h264player;

import com.h264player.MainActivity.MyHandler;

import android.app.Application;
import android.os.Handler;

public class MyAPP extends Application {  
    // 共享变量  
    private MyHandler handler = null;  
      
    // set方法  
    public void setHandler(MyHandler mHandler) {  
        this.handler = mHandler;  
    }  
      
    // get方法  
    public MyHandler getHandler() {  
        return handler;  
    }  
}  