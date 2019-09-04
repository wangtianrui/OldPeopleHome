package com.scorpiomiku.oldpeoplehome.modules.oldpeople.activity;

import android.annotation.SuppressLint;
import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.support.annotation.NonNull;
import android.support.design.widget.BottomNavigationView;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.view.MenuItem;
import android.view.View;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.scorpiomiku.oldpeoplehome.R;
import com.scorpiomiku.oldpeoplehome.base.BaseActivity;
import com.scorpiomiku.oldpeoplehome.base.BaseFragment;
import com.scorpiomiku.oldpeoplehome.bean.BleDeviceItem;
import com.scorpiomiku.oldpeoplehome.bean.SportData;
import com.scorpiomiku.oldpeoplehome.modules.oldpeople.fragmemt.EnvironmentFragment;
import com.scorpiomiku.oldpeoplehome.modules.oldpeople.fragmemt.HeartRateFragment;
import com.scorpiomiku.oldpeoplehome.modules.oldpeople.fragmemt.MyInformationFragment;
import com.scorpiomiku.oldpeoplehome.modules.oldpeople.fragmemt.SleepFragment;
import com.scorpiomiku.oldpeoplehome.modules.oldpeople.fragmemt.StepFragment;
import com.scorpiomiku.oldpeoplehome.service.SampleBleService;
import com.scorpiomiku.oldpeoplehome.utils.ComparatorBleDeviceItem;
import com.scorpiomiku.oldpeoplehome.utils.LogUtils;
import com.scorpiomiku.oldpeoplehome.utils.StatusBarUtils;
import com.scorpiomiku.oldpeoplehome.utils.TimeUtils;
import com.sxr.sdk.ble.keepfit.aidl.IRemoteService;
import com.sxr.sdk.ble.keepfit.aidl.IServiceCallback;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.Iterator;
import java.util.Locale;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

/**
 * Created by ScorpioMiku on 2019/8/18.
 */

public class OldPeopleMainActivity extends BaseActivity {
    @BindView(R.id.container)
    RelativeLayout container;
    @BindView(R.id.navigation)
    BottomNavigationView navigation;
    @BindView(R.id.floating_button)
    FloatingActionButton floatingButton;
    @BindView(R.id.close_button)
    FloatingActionButton closeButton;

    private String step;
    private String cal;
    private String distance;
    private String sportTime;
    private String curHeartRate;
    private String[] heartRates = new String[6];
    private String sleepType;
    private String bloodPressureShrink = "0";
    private String bloodPressureDiastole = "0";
    private String oxygen;

    private Boolean mIsBound = false;

    private BottomNavigationView.OnNavigationItemSelectedListener mOnNavigationItemSelectedListener;
    private FragmentManager fragmentManager;
    private BaseFragment[] fragments = {
            new StepFragment(),
            new SleepFragment(),
            new HeartRateFragment(),
            new EnvironmentFragment(),
            new MyInformationFragment()
    };
    private int[] barColors = {
            R.color.step_bg,
            R.color.sleep_bg,
            R.color.heart_rate_bg,
            R.color.environment_bg,
            R.color.informaiton_bg
    };

    @SuppressLint("HandlerLeak")
    @Override
    protected Handler initHandle() {
        handler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 1:
                        //蓝牙获取信息
                        loadData();
                        break;
                    case 2:
                        ((HeartRateFragment) fragments[2]).changeText(curHeartRate, bloodPressureShrink, bloodPressureDiastole, oxygen);
                        break;
                }
            }
        };
        return handler;
    }

    @Override
    public void iniview() {
        fragmentManager = getSupportFragmentManager();
        mOnNavigationItemSelectedListener = new BottomNavigationView.OnNavigationItemSelectedListener() {

            @Override
            public boolean onNavigationItemSelected(@NonNull MenuItem item) {
                switch (item.getItemId()) {
                    case R.id.navigation_step:
                        changeFragment(0);
                        return true;
                    case R.id.navigation_sleep:
                        changeFragment(1);
                        return true;
                    case R.id.navigation_heart_rate:
                        changeFragment(2);
                        return true;
                    case R.id.navigation_environmental:
                        changeFragment(3);
                        return true;
                    case R.id.navigation_user:
                        changeFragment(4);
                        return true;
                }
                return false;
            }
        };
        navigation.setOnNavigationItemSelectedListener(mOnNavigationItemSelectedListener);
        initFragmentManager();
        initBlueSDK();
    }

    @Override
    public int getLayoutId() {
        return R.layout.activity_old_people_main;
    }

    @Override
    public void refreshData() {

    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // TODO: add setContentView(...) invocation
        ButterKnife.bind(this);
    }

    /**
     * 初始化fragment管理器
     */
    private void initFragmentManager() {
        FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
        fragmentTransaction.add(R.id.container, fragments[0]);
        fragmentTransaction.add(R.id.container, fragments[1]);
        fragmentTransaction.add(R.id.container, fragments[2]);
        fragmentTransaction.add(R.id.container, fragments[3]);
        fragmentTransaction.add(R.id.container, fragments[4]);

        fragmentTransaction.hide(fragments[1]);
        fragmentTransaction.hide(fragments[2]);
        fragmentTransaction.hide(fragments[3]);
        fragmentTransaction.hide(fragments[4]);
        fragmentTransaction.commit();
        StatusBarUtils.setWindowStatusBarColor(this, barColors[0]);
    }

    /**
     * 修改fragment
     *
     * @param index
     */
    private void changeFragment(int index) {
        FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
        if (index == 4) {
            floatingButton.setVisibility(View.INVISIBLE);
        } else {
            floatingButton.setVisibility(View.VISIBLE);
        }
        for (int i = 0; i < fragments.length; i++) {
            if (i == index) {
                fragmentTransaction.show(fragments[i]);
                StatusBarUtils.setWindowStatusBarColor(this, barColors[i]);
            } else {
                fragmentTransaction.hide(fragments[i]);
            }
        }
        fragmentTransaction.commit();
    }


    //-------------blue
    private IRemoteService mService;
    private ArrayList<BleDeviceItem> nearbyItemList = new ArrayList<>();
    private ServiceConnection mServiceConnection;
    private String mac;
    private boolean bStart = false;
    private String pathLog = "/jyClient/log/";
    private boolean bSave = true;
    private IServiceCallback mServiceCallback = new IServiceCallback.Stub() {
        @Override
        public void onAuthSdkResult(int i) throws RemoteException {

        }

        @Override
        public void onScanCallback(String deviceName, String deviceMacAddress, int rssi) throws RemoteException {
//            LogUtils.logd("扫描ing");
            Iterator<BleDeviceItem> iter = nearbyItemList.iterator();
            BleDeviceItem item = null;
            boolean bExist = false;
            while (iter.hasNext()) {
                item = iter.next();
                if (item.getBleDeviceAddress().equalsIgnoreCase(deviceMacAddress)) {
                    bExist = true;
                    item.setRssi(rssi);
                    break;
                }
            }
            if (!bExist) {
                item = new BleDeviceItem(deviceName, deviceMacAddress, "", "", rssi, "");
                nearbyItemList.add(item);
                Collections.sort(nearbyItemList, new ComparatorBleDeviceItem());
                LogUtils.logList(nearbyItemList);
                if (item.getBleDeviceName().equals("Y1-4389")) {
                    callRemoteDisconnect();
                    mac = item.getBleDeviceAddress();
                    callRemoteConnect(item.getBleDeviceName(), item.getBleDeviceAddress());
                }
            }
        }

        @Override
        public void onConnectStateChanged(int state) throws RemoteException {
            if (state == 2) {
                LogUtils.logd("连接手环成功");
                getNewHandData();
            } else {
                LogUtils.loge("断开连接:" + state);
            }
        }

        @Override
        public void onAuthDeviceResult(int i) throws RemoteException {

        }

        @Override
        public void onGetDeviceTime(int i, String s) throws RemoteException {

        }

        @Override
        public void onSetDeviceTime(int i) throws RemoteException {

        }

        @Override
        public void onSetUserInfo(int i) throws RemoteException {

        }

        @Override
        public void onGetCurSportData(int type1, long timestamp1, int step1, int distance1,
                                      int cal1, int cursleeptime1, int totalrunningtime1, int steptime1) throws RemoteException {
            Date date = new Date(timestamp1 * 1000);
            SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.getDefault());
            String time = sdf.format(date);
            SportData sportData =
                    new SportData(type1, time, step1, distance1, cal1, cursleeptime1, totalrunningtime1, steptime1, "123");
            LogUtils.logd("获得新运动数据:" + sportData.toString());
            if (sportData.getType() == 0) {
                step = sportData.getStep() + "";
                distance = sportData.getDistance() + "";
                cal = sportData.getCal() + "";
            } else {
                sportTime = TimeUtils.getSportTime(sportData.getTotalrunningtime());
            }
            handler.sendEmptyMessage(1);
        }

        @Override
        public void onSendVibrationSignal(int i) throws RemoteException {

        }

        @Override
        public void onSetPhontMode(int i) throws RemoteException {

        }

        @Override
        public void onSetIdleTime(int i) throws RemoteException {

        }

        @Override
        public void onSetSleepTime(int i) throws RemoteException {

        }

        @Override
        public void onGetDeviceBatery(int i, int i1) throws RemoteException {

        }

        @Override
        public void onGetDeviceInfo(int i, String s, String s1, String s2, int i1) throws RemoteException {

        }

        @Override
        public void onSetAlarm(int i) throws RemoteException {

        }

        @Override
        public void onSetDeviceMode(int i) throws RemoteException {

        }

        @Override
        public void onSetNotify(int i) throws RemoteException {

        }

        @Override
        public void onGetSenserData(int result, long timestamp, int heartrate, int sleepstatu)
                throws RemoteException {
            Date date = new Date(timestamp * 1000);
            SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.getDefault());
            String time = sdf.format(date);
            curHeartRate = heartrate + "";
            LogUtils.logd("onGetSenserData-" + "result: " + result + ",time:" + time + ",heartrate:" + heartrate + ",sleepstatu:" + sleepstatu);
            handler.sendEmptyMessage(2);
        }

        @Override
        public void setAutoHeartMode(int i) throws RemoteException {

        }

        @Override
        public void onSetDeviceInfo(int i) throws RemoteException {

        }

        @Override
        public void onSetHourFormat(int i) throws RemoteException {

        }

        @Override
        public void onGetDataByDay(int type, long timestamp1, int step1, int heartrate1) throws RemoteException {
            LogUtils.logd(type + ";" + timestamp1 + ";" + step1 + ";" + heartrate1);
            switch (type) {
                case 1:
                    step = step1 + "";
                    break;
                case 2:
                    break;
                case 3:
                    heartRates[5] = heartrate1 + "";
                    break;
                default:
                    break;
            }
        }

        @Override
        public void onGetDataByDayEnd(int i, long l) throws RemoteException {

        }

        @Override
        public void onGetDeviceAction(int i) throws RemoteException {

        }

        @Override
        public void onGetBandFunction(int i, boolean[] booleans) throws RemoteException {

        }

        @Override
        public void onSetLanguage(int i) throws RemoteException {

        }

        @Override
        public void onSendWeather(int i) throws RemoteException {

        }

        @Override
        public void onSetAntiLost(int i) throws RemoteException {

        }

        @Override
        public void onSetBloodPressureMode(int i) throws RemoteException {

        }

        @Override
        public void onReceiveSensorData(int arg0, int arg1, int arg2, int arg3,
                                        int arg4) throws RemoteException {
//            LogUtils.logd("onReceiveSensorData" + "result:" + arg0 + " , " + arg1 + " , " + arg2 + " , " + arg3 + " , " + arg4);
            curHeartRate = arg0 + "";
            bloodPressureShrink = arg1 + "";
            bloodPressureDiastole = arg2 + "";
            oxygen = arg3 + "";
            handler.sendEmptyMessage(2);
        }

        @Override
        public void onGetMultipleSportData(int i, String s, int i1, int i2) throws RemoteException {

        }

        @Override
        public void onSetGoalStep(int i) throws RemoteException {

        }

        @Override
        public void onSetDeviceHeartRateArea(int i) throws RemoteException {

        }

        @Override
        public void onSensorStateChange(int i, int i1) throws RemoteException {

        }

        @Override
        public void onReadCurrentSportData(int i, String s, int i1, int i2) throws RemoteException {

        }

        @Override
        public void onGetOtaInfo(boolean b, String s, String s1) throws RemoteException {

        }

        @Override
        public void onGetOtaUpdate(int i, int i1) throws RemoteException {

        }

        @Override
        public void onSetDeviceCode(int i) throws RemoteException {

        }

        @Override
        public void onGetDeviceCode(byte[] bytes) throws RemoteException {

        }

        @Override
        public void onCharacteristicChanged(String s, byte[] bytes) throws RemoteException {

        }

        @Override
        public void onCharacteristicWrite(String s, byte[] bytes, int i) throws RemoteException {

        }
    };

    /**
     * SDK的绑定、扫描、蓝牙的选择
     */
    private void initBlueSDK() {
        Intent gattServiceIntent = new Intent(this,
                SampleBleService.class);
        gattServiceIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        startService(gattServiceIntent);
        mServiceConnection = new ServiceConnection() {
            @Override
            public void onServiceConnected(ComponentName componentName, IBinder service) {
                LogUtils.shortToast("Service connected");
                mIsBound = true;
                mService = IRemoteService.Stub.asInterface(service);

                try {
                    mService.registerCallback(mServiceCallback);
                    mService.openSDKLog(bSave, pathLog, "blue.log");
                    boolean isConnected = callRemoteIsConnected();

                    callRemoteScanDevice();
                    if (!isConnected) {

                    } else {

                        int authrize = callRemoteIsAuthrize();
                        LogUtils.logd("authrize:" + authrize);
                    }

                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }

            @Override
            public void onServiceDisconnected(ComponentName componentName) {
                LogUtils.loge("Service disconnected");
            }
        };
        /**
         * 自动绑定服务
         */
        Intent intent = new Intent(IRemoteService.class.getName());
        intent.setClassName("com.scorpiomiku.oldpeoplehome",
                "com.scorpiomiku.oldpeoplehome.service.SampleBleService");
        bindService(intent, mServiceConnection, BIND_AUTO_CREATE);
    }

    /**
     * 是否连接SDK成功
     *
     * @return
     */
    private boolean callRemoteIsConnected() {
        boolean isConnected = false;
        if (mService != null) {
            try {
                isConnected = mService.isConnectBt();
            } catch (RemoteException e) {
                e.printStackTrace();
                LogUtils.shortToast("Remote call error!");
            }
        } else {
            LogUtils.shortToast("Service is not available yet!");
        }

        return isConnected;
    }

    /**
     * 确定权限
     *
     * @return
     */
    private int callRemoteIsAuthrize() {
        int isAuthrize = 0;
        if (mService != null) {
            try {
                isAuthrize = mService.isAuthrize();
            } catch (RemoteException e) {
                e.printStackTrace();
                LogUtils.shortToast("Remote call error!");
            }
        } else {
            LogUtils.shortToast("Service is not available yet!");
        }

        return isAuthrize;
    }

    /**
     * 扫描蓝牙设备，自动进入回调  onScanCallback
     */
    private void callRemoteScanDevice() {
//        LogUtils.logd("开始扫描");
        if (nearbyItemList != null)
            nearbyItemList.clear();

        if (mService != null) {
            try {
                bStart = !bStart;
                mService.scanDevice(bStart);
            } catch (RemoteException e) {
                e.printStackTrace();
                LogUtils.shortToast("Remote call error!");
            }
        } else {
            LogUtils.loge("scan:Service is not available yet!");
        }
    }

    /**
     * 删除当前连接的设备
     */
    private void callRemoteDisconnect() {

        if (mService != null) {
            try {
                mService.disconnectBt(true);
            } catch (RemoteException e) {
                e.printStackTrace();
                LogUtils.logd("callRemoteDisconnect: Remote call error!");
            }
        } else {
            LogUtils.logd("callRemoteDisconnect: Service is not available yet!");
        }
    }

    /**
     * 连接设备
     *
     * @param name
     * @param mac
     */
    private void callRemoteConnect(String name, String mac) {
        if (mac == null || mac.length() == 0) {
            LogUtils.logd("callRemoteConnect: ble device mac address is not correctly!");
            return;
        }

        if (mService != null) {
            try {
                mService.connectBt(name, mac);
            } catch (RemoteException e) {
                e.printStackTrace();
                LogUtils.loge("callRemoteConnect: Remote call error!");
            }
        } else {
            LogUtils.loge("callRemoteConnect: Service is not available yet!");
        }
    }

    /**
     * 主动获取数据函数
     */
    private void getNewHandData() {
        try {
            mService.getCurSportData();
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    /**
     * 更新UI调用
     */
    private void loadData() {
        for (int i = 0; i < fragments.length; i++) {
            refreshUi(step, cal, distance, sportTime, heartRates, sleepType, fragments[i]);
        }
    }

    private Boolean heartRateOpen = false;

    /**
     * 测试心率按钮点击事件
     */
    public void setHeartRateMode(TextView begin) {
        if (heartRateOpen) {
            //关闭
            begin.setText("开启");
        } else {
            //开启
            begin.setText("关闭");
        }
        heartRateOpen = !heartRateOpen;
        try {
            mService.setHeartRateMode(heartRateOpen, 120);
            mService.setBloodPressureMode(heartRateOpen);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }


    @OnClick({R.id.floating_button, R.id.close_button})
    public void onViewClicked(View view) {
        switch (view.getId()) {
            case R.id.floating_button:
                getNewHandData();
                break;
            case R.id.close_button:
                callRemoteDisconnect();
                if (mIsBound) {
                    try {
                        mService.unregisterCallback(mServiceCallback);
                    } catch (RemoteException e) {
                        e.printStackTrace();
                    }
                    unbindService(mServiceConnection);
                }
                mIsBound = false;
                break;
        }
    }
}
