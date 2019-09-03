package com.scorpiomiku.wifitrick;

import android.annotation.SuppressLint;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.SupplicantState;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Handler;
import android.os.Message;
import android.os.Parcelable;
import android.util.Log;

import java.io.IOException;
import java.util.HashMap;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.Response;

/**
 * Created by ScorpioMiku on 2019/9/3.
 */

public class NetworkReceiver extends BroadcastReceiver {
    private static final String TAG = "NetworkReceiver";
    public static final int STATE1 = 1;//密码错误
    public static final int STATE2 = 2;//连接成功
    public static final int STATE3 = 3;//连接失败
    public static final int STATE4 = 4;//正在获取ip地址
    public static final int STATE5 = 5;//正在连接
    public static HashMap<String, String> mapData = new HashMap<>();
    @SuppressLint("HandlerLeak")
    private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case 1:
                    upData();
                    break;
            }
        }
    };

    @Override
    public void onReceive(Context context, Intent intent) {
        if (WifiManager.SUPPLICANT_STATE_CHANGED_ACTION.equals(intent.getAction())) {
            //密码错误广播,是不是正在获得IP地址
            int linkWifiResult = intent.getIntExtra(WifiManager.EXTRA_SUPPLICANT_ERROR, -1);
            if (linkWifiResult == WifiManager.ERROR_AUTHENTICATING) {
                //密码错误
                Log.d(TAG, "onReceive: 密码错误");
            }
            SupplicantState supplicantState = intent.getParcelableExtra(WifiManager.EXTRA_NEW_STATE);
            NetworkInfo.DetailedState state = WifiInfo.getDetailedStateOf(supplicantState);
            if (state == NetworkInfo.DetailedState.CONNECTING) {
                //正在连接
                Log.d(TAG, "onReceive: 正在连接");
            } else if (state == NetworkInfo.DetailedState.FAILED
                    || state == NetworkInfo.DetailedState.DISCONNECTING) {
                //连接失败
                Log.d(TAG, "onReceive: 连接失败");
            } else if (state == NetworkInfo.DetailedState.CONNECTED) {
                //连接成功
                Log.d(TAG, "onReceive: 连接成功");
            } else if (state == NetworkInfo.DetailedState.OBTAINING_IPADDR) {
                //正在获取ip地址
                Log.d(TAG, "onReceive: 正在获取ip地址");
            } else if (state == NetworkInfo.DetailedState.IDLE) {
                //闲置的
                Log.d(TAG, "onReceive: 闲置");
            }
        } else if (WifiManager.WIFI_STATE_CHANGED_ACTION.equals(intent.getAction())) {
            // 监听wifi的打开与关闭，与wifi的连接无关
            int wifiState = intent.getIntExtra(WifiManager.EXTRA_WIFI_STATE, 0);
            switch (wifiState) {
                case WifiManager.WIFI_STATE_DISABLING://正在停止0
                    Log.d(TAG, "onReceive: 正在停止");
                    break;
                case WifiManager.WIFI_STATE_DISABLED://已停止1
                    Log.d(TAG, "onReceive: 已停止");
//                    upData();
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            try {
                                Thread.sleep(5000);
                                handler.sendEmptyMessage(1);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }
                    }).start();
                    break;
                case WifiManager.WIFI_STATE_UNKNOWN://未知4
                    Log.d(TAG, "onReceive: 未知");
                    break;
                case WifiManager.WIFI_STATE_ENABLING://正在打开2
                    Log.d(TAG, "onReceive: 正在打开");
                    break;
                case WifiManager.WIFI_STATE_ENABLED://已开启3
                    Log.d(TAG, "onReceive: 已打开");
                    MainActivity.connectWifi();
                    break;
                default:
                    break;
            }
        } else if (WifiManager.NETWORK_STATE_CHANGED_ACTION.equals(intent.getAction())) {
            // 监听wifi的连接状态即是否连上了一个有效无线路由
            Parcelable parcelableExtra = intent
                    .getParcelableExtra(WifiManager.EXTRA_NETWORK_INFO);
            if (null != parcelableExtra) {
                // 获取联网状态的NetWorkInfo对象
                NetworkInfo networkInfo = (NetworkInfo) parcelableExtra;
                //获取的State对象则代表着连接成功与否等状态
                NetworkInfo.State state = networkInfo.getState();
                //判断网络是否已经连接
                boolean isConnected = state == NetworkInfo.State.CONNECTED;
//                Log.v(Constants.HTTP_WZ, "isConnected:" + isConnected);
                if (isConnected) {
//                    ToastUtils.showTipMsg(R.string.linked_success);
                }
            }
        }
    }

    @SuppressWarnings("unused")
    private String getConnectionType(int type) {
        String connType = "";
        if (type == ConnectivityManager.TYPE_MOBILE) {
            connType = "移动网络";
        } else if (type == ConnectivityManager.TYPE_WIFI) {
            connType = "WIFI网络";
        }
        return connType;
    }


    /**
     * 上传数据到服务器
     */
    private void upData() {
        WebUtils.upRoomState(mapData, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                Log.e(TAG, "onFailure: " + e.getMessage());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                Log.d(TAG, "onResponse: upok");
                mapData.clear();
                MainActivity.openWifi();
            }
        });
    }
}