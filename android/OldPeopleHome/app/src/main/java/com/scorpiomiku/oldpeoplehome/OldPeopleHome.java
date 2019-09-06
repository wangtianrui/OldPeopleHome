package com.scorpiomiku.oldpeoplehome;

import android.annotation.SuppressLint;
import android.app.Application;
import android.os.Handler;
import android.os.Message;

import com.baidu.location.BDAbstractLocationListener;
import com.baidu.location.BDLocation;
import com.baidu.location.LocationClient;
import com.baidu.location.LocationClientOption;
import com.scorpiomiku.oldpeoplehome.utils.LogUtils;

/**
 * Created by ScorpioMiku on 2019/8/17.
 */

public class OldPeopleHome extends Application {
    public static OldPeopleHome mInstance;

    @Override
    public void onCreate() {
        super.onCreate();
        mInstance = this;
    }
}
