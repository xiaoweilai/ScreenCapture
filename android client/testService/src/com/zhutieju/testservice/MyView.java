package com.zhutieju.testservice;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
/**
 * 自定义View用于显示接受到的画面
 * @author Administrator
 *
 */
public class MyView extends SurfaceView implements SurfaceHolder.Callback {
	private String TAG = "服务端日志";
	SurfaceHolder holder;// 控制SurfaceView的Holder对象
	private MyThread t;
	private Bitmap ivBitmap;//需要被画的BMP图片
	private boolean running = true;//控制MyThread的run方法
	private Paint paint = new Paint();
	
	public MyView(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
		this.holder = this.getHolder();
		holder.addCallback(this);
		t = new MyThread();
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		// TODO Auto-generated method stub
		t.start();
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width,
			int height) {
		// TODO Auto-generated method stub

	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		// TODO Auto-generated method stub
		if (t.isAlive()) {
			running = false;
		}
	}
	/**
	 * 控制更新MyView的画图线程
	 * @author Administrator
	 *
	 */
	private class MyThread extends Thread {
		Canvas canvas = null;
		@SuppressLint("WrongCall")
		@Override
		public void run() {
			while (running) {			
				if (Util.list.size() > 0) {
					canvas = holder.lockCanvas();
					if (canvas != null) {
						onDraw(canvas);
						Log.i(TAG,"------一帧图像被绘画-------");
					}
					holder.unlockCanvasAndPost(canvas);
				}	
				try {
					sleep(20);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}

	}
	/**
	 * 重写View的onDraw方法
	 */
	@Override
	protected void onDraw(Canvas canvas) {
		// TODO Auto-generated method stub
		super.onDraw(canvas);
		Matrix m = new Matrix();
		ivBitmap = Util.setOrgetBitmap(1, null);
		canvas.drawBitmap(ivBitmap, m, null);
		canvas.drawText("2013年5月2号", 0, 10, paint);
	}

}
