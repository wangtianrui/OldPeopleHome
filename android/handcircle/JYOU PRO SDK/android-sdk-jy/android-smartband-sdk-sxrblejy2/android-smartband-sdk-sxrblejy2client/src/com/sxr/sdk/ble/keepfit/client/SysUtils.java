package com.sxr.sdk.ble.keepfit.client;


import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Build;
import android.os.Environment;
import android.util.Log;

import org.apache.http.util.EncodingUtils;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;
import java.security.MessageDigest;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Calendar;
import java.util.Date;

public class SysUtils {

	// 将字符串写入到文本文件中
	public static void writeTxtToFile(String strcontent, String filePath, String fileName) {
	     //生成文件夹之后，再生成文件，不然会出错
		File sdcardDir = Environment.getExternalStorageDirectory();
		String path = sdcardDir.getPath() + filePath;
		Calendar today = Calendar.getInstance();
		fileName = today.get(Calendar.YEAR) + "-" + (today.get(Calendar.MONTH) + 1) + "-" + today.get(Calendar.DAY_OF_MONTH) + "-" + fileName;
	    makeFilePath(path, fileName);
	     
	     String strFilePath = path + fileName;
	     // 每次写入时，都换行写
	     SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
	     String now = sdf.format(new Date());
	    String strContent = now + ":" + strcontent + "\r\n";
	     try {
	         File file = new File(strFilePath);
	         if (!file.exists()) {
	             Log.d("TestFile", "Create the file:" + strFilePath);
	             file.getParentFile().mkdirs();
	             file.createNewFile();
	         }
	         RandomAccessFile raf = new RandomAccessFile(file, "rwd");
	         raf.seek(file.length());
	         raf.write(strContent.getBytes());
	         raf.close();
	     } catch (Exception e) {
	         Log.e("TestFile", "Error on write File:" + e);
	     }
	 }
	  
	 // 生成文件
	public static File makeFilePath(String filePath, String fileName) {
	     File file = null;
	     makeRootDirectory(filePath);
	     try {
	         file = new File(filePath + fileName);
	         if (!file.exists()) {
	             file.createNewFile();
	         }
	     } catch (Exception e) {
	         e.printStackTrace();
	     }
	     return file;
	 }

	 // 生成文件夹
	public static void makeRootDirectory(String filePath) {
	     File file = null;
	     try {
	         file = new File(filePath);
	         if (!file.exists()) {
	             file.mkdir();
	         }
	     } catch (Exception e) {
	         Log.i("error:", e+"");
	     }
	}

	public static String printHexString( byte[] b) {
		if(b == null) {
			return "";
		}
		
		String hexString = "";
	   for (int i = 0; i < b.length; i++) { 
	     String hex = Integer.toHexString(b[i] & 0xFF); 
	     if (hex.length() == 1) { 
	       hex = '0' + hex; 
	     } 
	     hexString += hex.toUpperCase() + " "; 
	   }

	   return hexString;
	}
	



}

