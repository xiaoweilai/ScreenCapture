package com.h264player;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;
import java.util.concurrent.locks.ReentrantReadWriteLock;

public class Log {
	static String TAG = "Log  ";

	public Log() {

	}

	public static void writeSystemLogToFile(String content) {
//		System.out.println(TAG + content+"  time=" + FormatDate.getCurDateInMillMaohao());
		return;
	}
	public static void writeErroLogToFile(String content) {
		String path = "Log";
		return;
//		File idFile = new File(path);
//		if (!idFile.exists()) {
//			idFile.mkdir();
//		}
//		String logPath = path + "/hualuhealthy.log";
//		idFile = new File(logPath);
//		if (!idFile.exists()) {
//			try {
//				idFile.createNewFile();
//			} catch (IOException e) {
//				// TODO Auto-generated catch block
//				e.printStackTrace();
//			}
//		}
//		LockFile file = new LockFile(logPath);
//		ReentrantReadWriteLock lock = file.getLock();
//		lock.writeLock().lock();
//		try {
//
//			FileWriter fileWriter = new FileWriter(file, true);
//			BufferedWriter bfw = new BufferedWriter(fileWriter);
//			bfw.write(content + "  time=" + FormatDate.getCurDateInMillMaohao());
//			bfw.write("\n");
//			bfw.flush();
//			fileWriter.close();
//			bfw.close();
//		} catch (IOException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		} finally {
//			lock.writeLock().unlock();
//		}

	}

	public synchronized static void writeConnectCountToFile(String count) {
		String path = "Log";
		System.out.println(TAG + count);
		File idFile = new File(path);
		if (!idFile.exists()) {
			idFile.mkdir();
		}
		String logPath = path + "/connectCount.log";
		idFile = new File(logPath);
		if (!idFile.exists()) {
			try {
				idFile.createNewFile();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		while (!idFile.renameTo(idFile)) {

		}
		try {
			FileWriter fileWriter = new FileWriter(logPath, false);
			fileWriter.write("仪器连接总数量是" + count + "个");
			fileWriter.flush();
			fileWriter.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	
}
