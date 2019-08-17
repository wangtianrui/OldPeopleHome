package com.scorpiomiku.oldpeoplehome;

import android.app.Application;

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
