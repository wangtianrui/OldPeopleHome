package com.scorpiomiku.wifitrick;

import android.Manifest;
import android.annotation.SuppressLint;
import android.content.Context;
import android.content.pm.PackageManager;
import android.net.ConnectivityManager;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.RequiresApi;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.google.gson.Gson;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;

import java.io.IOException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.Response;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "MainActivity";
    private Button testButton;
    private Button testButton2;
    private Button startButton;
    private static TextView textView;
    private static WifiManager wifiManager;
    private List<ScanResult> wifiList = new ArrayList<>();
    private static Handler handler;
    private static Context context;

    @SuppressLint("HandlerLeak")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        handler = new Handler() {
            @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP_MR1)
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 1:
                        //关闭wifi的逻辑
                        Log.d(TAG, "handleMessage: 关网");
                        wifiManager.setWifiEnabled(false);
                        break;
                }
            }
        };
        context = this;
        testButton = findViewById(R.id.test);
        testButton2 = findViewById(R.id.test2);
        startButton = findViewById(R.id.start);
        textView = findViewById(R.id.text);
        wifiManager = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);

//        checkPermission();
        startButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                openWifi();
            }
        });
    }


    /**
     * 获取wifi列表
     */
    private void getWifiList() {
        wifiList = wifiManager.getScanResults();
        if (wifiList.size() == 0) {
            Log.d(TAG, "onClick: empty");
        } else {
            for (ScanResult result :
                    wifiList) {
                Log.d(TAG, "onReceive: " + result.toString());
            }
        }
    }

    /**
     * 延时10s关闭wifi
     */
    public static void delayCloseWifi() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    Thread.sleep(2000);
                    handler.sendEmptyMessage(1);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }

    /**
     * 打开wifi，自动链接
     */
    public static void openWifi() {
        wifiManager.setWifiEnabled(true);
    }

    public static void connectWifi() {
        WifiConnect wifiConnect = new WifiConnect(context, textView);
        wifiConnect.addNetWork("sunplus_009", "");
    }


}
