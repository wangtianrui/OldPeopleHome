package com.scorpiomiku.oldpeoplehome;

import android.app.Application;

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
    private LocationClient mLocationClient = null;

    @Override
    public void onCreate() {
        super.onCreate();
        mInstance = this;
        mLocationClient = new LocationClient(getApplicationContext());
        mLocationClient.registerLocationListener(new BDAbstractLocationListener() {
            @Override
            public void onReceiveLocation(BDLocation bdLocation) {
                double latitude = bdLocation.getLatitude();    //获取纬度信息
                double longitude = bdLocation.getLongitude();    //获取经度信息
                float radius = bdLocation.getRadius();    //获取定位精度，默认值为0.0f
                //获取经纬度坐标类型，以LocationClientOption中设置过的坐标类型为准
                String coorType = bdLocation.getCoorType();
                //获取定位类型、定位错误返回码，具体信息可参照类参考中BDLocation类中的说明
                int errorCode = bdLocation.getLocType();
//                LogUtils.logd(latitude + ";" + longitude + ";" + radius + ";" + coorType + ";" + errorCode);
            }
        });
        LocationClientOption option = new LocationClientOption();
        option.setLocationMode(LocationClientOption.LocationMode.Hight_Accuracy);
        option.setCoorType("bd0911");
        option.setScanSpan(10000);
        option.setOpenGps(true);
        option.setLocationNotify(true);
        option.setIgnoreKillProcess(false);
        option.SetIgnoreCacheException(false);
        option.setWifiCacheTimeOut(5 * 60 * 1000);
        mLocationClient.setLocOption(option);
        mLocationClient.start();
    }
}
