package com.scorpiomiku.wifitrick;

import android.annotation.SuppressLint;
import android.content.Context;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.widget.TextView;

import com.google.gson.Gson;

import java.io.IOException;
import java.util.List;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.Response;

/**
 * Created by ScorpioMiku on 2019/9/3.
 */

public class WifiConnect {
    private WifiManager mWifiManager;
    private List<WifiConfiguration> wifiConfigurationList;
    private Context mContext;
    private TextView textView;

    private String isin;
    private String temporature;
    private String humidity;
    private String time;
    private String roomId;

    private Handler handler;
    private static final String TAG = "WifiConnect";


    @SuppressLint("HandlerLeak")
    public WifiConnect(Context context, TextView view) {
        //  常驻
        mWifiManager = (WifiManager) context.getApplicationContext().getSystemService(Context.WIFI_SERVICE);
        textView = view;
        handler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 1:
                        getRoomData();
                        break;
                }
            }
        };
    }

    public WifiConnect(List<WifiConfiguration> wifiConfigurationList, WifiManager mWifiManager) {
        this.wifiConfigurationList = wifiConfigurationList;
        this.mWifiManager = mWifiManager;
    }


    /**
     * 连接wifi
     */
    public void addNetWork(String SSID, String PASSW) {
        int netId = -1;
        if (removeWifi(SSID)) {
            Log.e("sin", "移除,新建config");
            netId = mWifiManager.addNetwork(createWifiInfo(SSID, PASSW));
        } else {
            if (getExitsWifiConfig(SSID) != null) {
                Log.e("sin", "这个wifi是连接过");
                netId = getExitsWifiConfig(SSID).networkId;
            } else {
                Log.e("sin", "没连接过的，新建一个wifi配置 ");
                netId = mWifiManager.addNetwork(createWifiInfo(SSID, PASSW));
            }
        }

        Log.e("sin", "netId: " + netId);

        boolean b = mWifiManager.enableNetwork(netId, true);  //  无论咋这都会返回true；
        Log.e("sin", "ssss: " + b);
        if (!b) {
            Log.e("SSSSSSSSSSSS", "-0.0-");
            //  如果这里失败，再从新获取manager  重新配置config；
            mWifiManager = (WifiManager) mContext.getApplicationContext().getSystemService(Context.WIFI_SERVICE);
            // 配置config
        } else {
            Log.e("SIN", "success");
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
        }

    }

    private void getRoomData() {
        WebUtils.getTest(new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {

            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                NetworkReceiver.mapData.clear();
                String body = response.body().string();
                Gson gson = new Gson();
                Node[] nodes = gson.fromJson(body, Node[].class);
                for (Node node : nodes) {
                    funcList[] funcLists = node.getFuncList();
                    for (funcList funcList : funcLists) {
                        if (funcList.getType().equals("安防")) {
                            isin = String.valueOf((int) funcList.getData());
                        } else if (funcList.getType().equals("温度")) {
                            temporature = String.valueOf(funcList.getData());
                        } else if (funcList.getType().equals("湿度")) {
                            humidity = String.valueOf(funcList.getData());
                        }
                    }
                }
                RoomData roomData = new RoomData(isin, temporature, humidity, TimeUtils.getTime(), "1");
                NetworkReceiver.mapData.putAll(roomData.getMap());
                Log.d(TAG, "onResponse: " + roomData);
                MainActivity.delayCloseWifi();
            }
        });
    }

    /**
     * 存在过的wifiConfiguration
     */
    public WifiConfiguration getExitsWifiConfig(String SSID) {
        wifiConfigurationList = mWifiManager.getConfiguredNetworks();
        for (WifiConfiguration wifiConfiguration : wifiConfigurationList) {
            if (wifiConfiguration.SSID.equals("\"" + SSID + "\"")) {
                return wifiConfiguration;
            }
        }
        return null;
    }


    public boolean removeWifi(int netId) {
        //
        return mWifiManager.removeNetwork(netId);
    }

    /**
     * config里存在； 在mWifiManager移除；
     */
    public boolean removeWifi(String SSID) {
        if (getExitsWifiConfig(SSID) != null) {
            return removeWifi(getExitsWifiConfig(SSID).networkId);
        } else {
            return false;
        }
    }


    /**
     * CHUANJian
     *
     * @param SSID
     * @param password
     * @return
     */
    public WifiConfiguration createWifiInfo(String SSID, String password) {
        WifiConfiguration config = new WifiConfiguration();

        if (config != null) {
            config.allowedAuthAlgorithms.clear();
            config.allowedGroupCiphers.clear();
            config.allowedKeyManagement.clear();
            config.allowedPairwiseCiphers.clear();
            config.allowedProtocols.clear();
            config.SSID = "\"" + SSID + "\"";

            //如果有相同配置的，就先删除
            WifiConfiguration tempConfig = getExitsWifiConfig(SSID);
            if (tempConfig != null) {
                mWifiManager.removeNetwork(tempConfig.networkId);
                mWifiManager.saveConfiguration();
            }
            config.preSharedKey = "\"" + password + "\"";
            config.hiddenSSID = true;
            config.allowedAuthAlgorithms.set(WifiConfiguration.AuthAlgorithm.OPEN);
            config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.TKIP);
            config.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.WPA_PSK);
            config.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.TKIP);
            //config.allowedProtocols.set(WifiConfiguration.Protocol.WPA);
            config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.CCMP);
            config.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.CCMP);
            config.status = WifiConfiguration.Status.ENABLED;

            Log.e("sin", "config: " + config.SSID + "      config: " + config.toString());
            return config;
        } else {

            Log.e("SSS", "WOCAO   NULL  ！！！");
            return null;
        }

    }
}

