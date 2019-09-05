package com.scorpiomiku.oldpeoplehome.base;

import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;


import com.scorpiomiku.oldpeoplehome.bean.OldPeople;
import com.scorpiomiku.oldpeoplehome.bean.User;
import com.scorpiomiku.oldpeoplehome.utils.LogUtils;
import com.scorpiomiku.oldpeoplehome.utils.WebUtils;

import java.io.IOException;
import java.util.HashMap;

/**
 * Created by ScorpioMiku on 2019/6/22.
 */

public abstract class BaseFragment extends Fragment {
    private View myView;
    protected Handler handler;
    protected HashMap<String, String> data = new HashMap<>();
    private WebUtils webUtils;
//    private User user;
//
//    public User getUser() {
//        return user;
//    }
//
//    public void setUser(User user) {
//        this.user = user;
//    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        this.myView = inflater.inflate(getLayoutId(), container, false);
        return myView;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        webUtils = WebUtils.getInstance();
        initView();
        handler = initHandle();
//        refreshData();
    }

    protected WebUtils getWebUtils() {
        return webUtils;
    }

    protected abstract Handler initHandle();


    protected abstract int getLayoutId();

    protected abstract void refreshData();

    protected abstract void initView();

    public View getMyView() {
        return myView;
    }

    public void refreshUi(String step, String cal, String distance, String sportTime, String[] heartRate, String sleepType) {
        LogUtils.logd("step:" + step + ";cal:" + cal + ";dis:" + distance + ";sportTime:" + sportTime + ";heartRate:" + heartRate + ";sleepType" + sleepType);
    }

    public void refreshUi(OldPeople oldPeople) {
        LogUtils.logd(oldPeople.toString());
    }


}
