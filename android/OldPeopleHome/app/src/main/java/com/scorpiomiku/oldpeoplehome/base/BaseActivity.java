package com.scorpiomiku.oldpeoplehome.base;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;


import com.scorpiomiku.oldpeoplehome.bean.User;
import com.scorpiomiku.oldpeoplehome.modules.loginregister.LoginActivity;
import com.scorpiomiku.oldpeoplehome.utils.LogUtils;
import com.scorpiomiku.oldpeoplehome.utils.WebUtils;

import java.util.HashMap;

import butterknife.ButterKnife;

/**
 * Created by ScorpioMiku on 2019/6/22.
 */

public abstract class BaseActivity extends AppCompatActivity {
    protected Handler handler;
    protected HashMap<String, String> data = new HashMap<>();
    private WebUtils webUtils;
    private User user;

    public User getUser() {
        return user;
    }

    public void setUser(User user) {
        this.user = user;
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(getLayoutId());
        ButterKnife.bind(this);
        webUtils = WebUtils.getInstance();
        iniview();
        handler = initHandle();
    }

    protected WebUtils getWebUtils() {
        return webUtils;
    }

    protected abstract Handler initHandle();

    abstract public void iniview();

    abstract public int getLayoutId();

    abstract public void refreshData();

    /**
     * 权限管理
     */
    protected void checkPermission() {
        //使用兼容库就无需判断系统版本
        int hasWriteStoragePermission = ContextCompat.checkSelfPermission(getApplication(), Manifest.permission.WRITE_EXTERNAL_STORAGE);
        if (hasWriteStoragePermission == PackageManager.PERMISSION_GRANTED) {
            //拥有权限，执行操作
        } else {
            //没有权限，向用户请求权限
            ActivityCompat.requestPermissions(BaseActivity.this,
                    new String[]{
                            Manifest.permission.WRITE_EXTERNAL_STORAGE,
                            Manifest.permission.READ_EXTERNAL_STORAGE,
                            Manifest.permission.CALL_PHONE,
                            Manifest.permission.CAMERA,
                            Manifest.permission.SEND_SMS,
                            Manifest.permission.ACCESS_COARSE_LOCATION
                    },
                    1);
        }
    }

    public void refreshUi(String step, String cal, String distance, String sportTime, String[] heartRate, String sleepType, BaseFragment fragment) {
        fragment.refreshUi(step, cal, distance, sportTime, heartRate, sleepType);
    }

}
