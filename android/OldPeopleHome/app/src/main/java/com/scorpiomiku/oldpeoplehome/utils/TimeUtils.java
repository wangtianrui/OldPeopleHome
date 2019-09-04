package com.scorpiomiku.oldpeoplehome.utils;

import android.annotation.SuppressLint;

import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * Created by ScorpioMiku on 2019/8/18.
 */

public class TimeUtils {
    public static String getTime() {
        @SuppressLint("SimpleDateFormat") SimpleDateFormat simpleDateFormat
                = new SimpleDateFormat("yyyy年MM月dd日 HH:mm:ss");// HH:mm:ss
        //获取当前时间
        Date date = new Date(System.currentTimeMillis());
        return simpleDateFormat.format(date) + "";
    }

    public static String getUpDate() {
        @SuppressLint("SimpleDateFormat") SimpleDateFormat simpleDateFormat
                = new SimpleDateFormat("yyyy-MM-dd");// HH:mm:ss
        //获取当前时间
        Date date = new Date(System.currentTimeMillis());
        return simpleDateFormat.format(date) + "";
    }

    /**
     * 运动时间转换
     *
     * @param time
     * @return
     */
    public static String getSportTime(int time) {
        String t = "";
        int sec = time % 60;
        int min = time / 60 % 60;
        int hour = time / 60 / 60;
        if (hour > 0 && hour < 10) {
            t = t + "0" + hour;
        } else if (hour == 0) {
            t = t + "00";
        }
        if (min < 10) {
            t = t + ":0" + min;
        } else {
            t = t + min;
        }
        if (sec < 10) {
            t = t + ":0" + sec;
        } else {
            t = t + ":" + sec;
        }

        return t;
    }
}
