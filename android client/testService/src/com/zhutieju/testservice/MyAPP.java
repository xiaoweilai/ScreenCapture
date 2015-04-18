package com.zhutieju.testservice;

import com.zhutieju.testservice.MainActivity.MyHandler;

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