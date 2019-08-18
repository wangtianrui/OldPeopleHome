package com.scorpiomiku.oldpeoplehome.base;

import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;


import butterknife.ButterKnife;

/**
 * Created by ScorpioMiku on 2019/6/22.
 */

public abstract class BaseActivity extends AppCompatActivity {
    protected Handler handler;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(getLayoutId());
        ButterKnife.bind(this);
        iniview();
        handler = initHandle();
    }

    protected abstract Handler initHandle();

    abstract public void iniview();

    abstract public int getLayoutId();

    abstract public void refreshData();


}
