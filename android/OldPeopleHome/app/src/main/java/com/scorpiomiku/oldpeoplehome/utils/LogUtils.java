package com.scorpiomiku.oldpeoplehome.utils;

import android.util.Log;
import android.widget.Toast;

import com.scorpiomiku.oldpeoplehome.OldPeopleHome;

import java.util.List;

/**
 * Created by ScorpioMiku on 2019/8/18.
 */

public class LogUtils {
    private static final String TAG = "mylogger";

    public static void logd(String message) {
        Log.d(TAG, message);
    }

    public static void shortToast(String message) {
        Toast.makeText(OldPeopleHome.mInstance, message, Toast.LENGTH_SHORT).show();
    }

    public static void loge(String message) {
        Log.e(TAG, message);
    }

    public static void logList(List list) {
        String log = "[\n";
        for (Object item :
                list) {
            log += item.toString() + ";\n";
        }
        log += "]";
        Log.d(TAG, log);
    }
}
