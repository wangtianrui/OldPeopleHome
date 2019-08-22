package com.sxr.sdk.ble.keepfit.client;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.graphics.drawable.AnimationDrawable;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Process;
import android.os.RemoteException;
import android.text.Html;
import android.text.format.DateFormat;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnKeyListener;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.LinearLayout.LayoutParams;
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;

import com.sxr.sdk.ble.keepfit.aidl.AlarmInfoItem;
import com.sxr.sdk.ble.keepfit.aidl.BleClientOption;
import com.sxr.sdk.ble.keepfit.aidl.DeviceProfile;
import com.sxr.sdk.ble.keepfit.aidl.IRemoteService;
import com.sxr.sdk.ble.keepfit.aidl.IServiceCallback;
import com.sxr.sdk.ble.keepfit.aidl.UserProfile;
import com.sxr.sdk.ble.keepfit.aidl.Weather;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.Locale;
import java.util.Random;

public class MainActivity extends Activity implements View.OnClickListener {

    private static final String TAG = MainActivity.class.getSimpleName();

    private IRemoteService mService;
    private boolean mIsBound = false;
    private int countStep = 0;
    private String data = "";
    private LinearLayout llConnect;

    private String pathLog = "/jyClient/log/";
    private boolean bSave = true;

    private ServiceConnection mServiceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            Toast.makeText(MainActivity.this, "Service connected", Toast.LENGTH_SHORT).show();

            mService = IRemoteService.Stub.asInterface(service);
            try {
                mService.registerCallback(mServiceCallback);
                mService.openSDKLog(bSave, pathLog, "blue.log");

                boolean isConnected = callRemoteIsConnected();

                if (isConnected == false) {
                    btBind.setEnabled(false);
                    btUnbind.setEnabled(true);
                    btScan.setEnabled(true);
                    btConnect.setEnabled(false);
                    btDisconnect.setEnabled(false);
                    llConnect.setVisibility(View.GONE);
                } else {
                    int authrize = callRemoteIsAuthrize();
                    if (authrize == 200) {
                        String curMac = callRemoteGetConnectedDevice();

                        btBind.setEnabled(false);
                        btUnbind.setEnabled(true);
                        btScan.setEnabled(true);
                        btConnect.setEnabled(false);
                        btDisconnect.setEnabled(true);
                        llConnect.setVisibility(View.VISIBLE);
                    }
                }

            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            Toast.makeText(MainActivity.this, "Service disconnected", Toast.LENGTH_SHORT).show();

            btBind.setEnabled(true);
            btUnbind.setEnabled(false);
            btScan.setEnabled(false);
            btConnect.setEnabled(false);
            btDisconnect.setEnabled(false);
            llConnect.setVisibility(View.GONE);
            mService = null;
        }
    };

    private IServiceCallback mServiceCallback = new IServiceCallback.Stub() {
        @Override
        public void onConnectStateChanged(int state) throws RemoteException {
            showToast("onConnectStateChanged", curMac + " state " + state);
            updateConnectState(state);
        }


        @Override
        public void onScanCallback(String deviceName, String deviceMacAddress, int rssi)
                throws RemoteException {
            Log.i(TAG, String.format("onScanCallback <%1$s>[%2$s](%3$d)", deviceName, deviceMacAddress, rssi));

            if(nearbyItemList == null)
                return;

            Iterator<BleDeviceItem> iter = nearbyItemList.iterator();
            BleDeviceItem item = null;
            boolean bExist = false;
            while (iter.hasNext()) {
                item = (BleDeviceItem) iter.next();
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
            }

            Message msg = new Message();
            scanDeviceHandler.sendMessage(msg);


//            if(deviceName.equals("S7-0192")){
//                mService.connectBt(deviceName, deviceMacAddress);
//                new Handler().postDelayed(new Runnable() {
//                    @Override
//                    public void run() {
//                        try {
//                            mService.disconnectBt(true);
//                            mService.scanDevice(true);
//                        } catch (RemoteException e) {
//                            e.printStackTrace();
//                        }
//
//                    }
//                }, 5000);
//            }
        }


        @Override
        public void onSetNotify(int result) throws RemoteException {
            showToast("onSetNotify", String.valueOf(result));
        }

        @Override
        public void onSetUserInfo(int result) throws RemoteException {
            showToast("onSetUserInfo", "" + result);
        }

        @Override
        public void onAuthSdkResult(int errorCode) throws RemoteException {
            showToast("onAuthSdkResult", errorCode + "");
        }

        @Override
        public void onGetDeviceTime(int result, String time) throws RemoteException {
            showToast("onGetDeviceTime", String.valueOf(time));
        }

        @Override
        public void onSetDeviceTime(int arg0) throws RemoteException {
            showToast("onSetDeviceTime", arg0 + "");
        }

        @Override
        public void onSetDeviceInfo(int arg0) throws RemoteException {
            showToast("onSetDeviceInfo", arg0 + "");
        }


        @Override
        public void onAuthDeviceResult(int arg0) throws RemoteException {
            showToast("onAuthDeviceResult", arg0 + "");
        }


        @Override
        public void onSetAlarm(int arg0) throws RemoteException {
            showToast("onSetAlarm", arg0 + "");
        }

        @Override
        public void onSendVibrationSignal(int arg0) throws RemoteException {
            showToast("onSendVibrationSignal", "result:" + arg0);
        }

        @Override
        public void onGetDeviceBatery(int arg0, int arg1)
                throws RemoteException {
            showToast("onGetDeviceBatery", "batery:" + arg0 + ", statu " + arg1);
        }


        @Override
        public void onSetDeviceMode(int arg0) throws RemoteException {
            showToast("onSetDeviceMode", "result:" + arg0);
        }

        @Override
        public void onSetHourFormat(int arg0) throws RemoteException {
            showToast("onSetHourFormat ", "result:" + arg0);

        }

        @Override
        public void setAutoHeartMode(int arg0) throws RemoteException {
            showToast("setAutoHeartMode ", "result:" + arg0);
        }


        @Override
        public void onGetCurSportData(int type, long timestamp, int step, int distance,
                                      int cal, int cursleeptime, int totalrunningtime, int steptime) throws RemoteException {
            Date date = new Date(timestamp * 1000);
            SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.getDefault());
            String time = sdf.format(date);
            showToast("onGetCurSportData", "type : " + type + " , time :" + time + " , step: " + step + ", distance :" + distance + ", cal :" + cal + ", cursleeptime :" + cursleeptime + ", totalrunningtime:" + totalrunningtime);


        }

        @Override
        public void onGetSenserData(int result, long timestamp, int heartrate, int sleepstatu)
                throws RemoteException {
            Date date = new Date(timestamp * 1000);
            SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.getDefault());
            String time = sdf.format(date);
            showToast("onGetSenserData", "result: " + result + ",time:" + time + ",heartrate:" + heartrate + ",sleepstatu:" + sleepstatu);

        }


        @Override
        public void onGetDataByDay(int type, long timestamp, int step, int heartrate)
                throws RemoteException {
            Date date = new Date(timestamp * 1000);
            SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.getDefault());
            String recorddate = sdf.format(date);
            showToast("onGetDataByDay", "type:" + type + ",time::" + recorddate + ",step:" + step + ",heartrate:" + heartrate);
            if (type == 2) {
                sleepcount++;
            }
        }

        @Override
        public void onGetDataByDayEnd(int type, long timestamp) throws RemoteException {
            Date date = new Date(timestamp * 1000);
            SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.getDefault());
            String recorddate = sdf.format(date);
            showToast("onGetDataByDayEnd", "time:" + recorddate + ",sleepcount:" + sleepcount);
            sleepcount = 0;
        }


        @Override
        public void onSetPhontMode(int arg0) throws RemoteException {
            showToast("onSetPhontMode", "result:" + arg0);
        }


        @Override
        public void onSetSleepTime(int arg0) throws RemoteException {
            showToast("onSetSleepTime", "result:" + arg0);
        }


        @Override
        public void onSetIdleTime(int arg0) throws RemoteException {
            showToast("onSetIdleTime", "result:" + arg0);
        }


        @Override
        public void onGetDeviceInfo(int version, String macaddress, String vendorCode,
                                    String productCode, int result) throws RemoteException {
            showToast("onGetDeviceInfo", "version :" + version + ",macaddress : " + macaddress + ",vendorCode : " + vendorCode + ",productCode :" + productCode + " , CRCresult :" + result);

        }

        @Override
        public void onGetDeviceAction(int type) throws RemoteException {
            showToast("onGetDeviceAction", "type:" + type);
        }


        @Override
        public void onGetBandFunction(int result, boolean[] results) throws RemoteException {
            showToast("onGetBandFunction", "result : " + result + ", results :" + results.length);

            String function = "";
            for(int i = 0; i < results.length; i ++){
                function += String.valueOf((i+1) + "=" + results[i] + " ");
			}
			showToast("onGetBandFunction", function);
        }

        @Override
        public void onSetLanguage(int arg0) throws RemoteException {
            showToast("onSetLanguage", "result:" + arg0);
        }


        @Override
        public void onSendWeather(int arg0) throws RemoteException {
            showToast("onSendWeather", "result:" + arg0);
        }


        @Override
        public void onSetAntiLost(int arg0) throws RemoteException {
            showToast("onSetAntiLost", "result:" + arg0);

        }


        @Override
        public void onReceiveSensorData(int arg0, int arg1, int arg2, int arg3,
                                        int arg4) throws RemoteException {
            showToast("onReceiveSensorData", "result:" + arg0 + " , " + arg1 + " , " + arg2 + " , " + arg3 + " , " + arg4);
        }


        @Override
        public void onSetBloodPressureMode(int arg0) throws RemoteException {
            showToast("onSetBloodPressureMode", "result:" + arg0);
        }


        @Override
        public void onGetMultipleSportData(int flag, String recorddate, int mode, int value)
                throws RemoteException {
//            Date date = new Date(timestamp * 1000);
//            SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.getDefault());
//            String recorddate = sdf.format(date);
            showToast("onGetMultipleSportData", "flag:" + flag + " , mode :" + mode + " recorddate:" + recorddate + " , value :" + value);
        }


        @Override
        public void onSetGoalStep(int result) throws RemoteException {
            showToast("onSetGoalStep", "result:" + result);
        }


        @Override
        public void onSetDeviceHeartRateArea(int result) throws RemoteException {
            showToast("onSetDeviceHeartRateArea", "result:" + result);
        }


        @Override
        public void onSensorStateChange(int type, int state)
                throws RemoteException {

            showToast("onSensorStateChange", "type:" + type + " , state : " + state);
        }

        @Override
        public void onReadCurrentSportData(int mode, String time, int step,
                                           int cal) throws RemoteException {

            showToast("onReadCurrentSportData", "mode:" + mode + " , time : " + time + " , step : " + step + " cal :" + cal);
        }

        @Override
        public void onGetOtaInfo(boolean isUpdate, String version, String path) throws RemoteException {
            showToast("onGetOtaInfo", "isUpdate " + isUpdate + " version " + version + " path " + path);
        }

        @Override
        public void onGetOtaUpdate(int step, int progress) throws RemoteException {
            showToast("onGetOtaUpdate", "step " + step + " progress " + progress);
        }

        @Override
        public void onSetDeviceCode(int result) throws RemoteException {
            showToast("onSetDeviceCode", "result " + result);
        }

        @Override
        public void onGetDeviceCode(byte[] bytes) throws RemoteException {
            showToast("onGetDeviceCode", "bytes " + SysUtils.printHexString(bytes));
        }

        @Override
        public void onCharacteristicChanged(String uuid, byte[] bytes) throws RemoteException {
            showToast("onCharacteristicChanged", uuid + " " + SysUtils.printHexString(bytes));
        }

        @Override
        public void onCharacteristicWrite(String uuid, byte[] bytes, int status) throws RemoteException {
            showToast("onCharacteristicWrite", status + " " + uuid + " " + SysUtils.printHexString(bytes));
        }
    };
    private int sleepcount = 0;
    private boolean bStart = false;

    private PopupWindow window;

    private ArrayList<BleDeviceItem> nearbyItemList;

    private listDeviceViewAdapter nearbyListAdapter;

    private Button btBind;
    private Button btUnbind;
    private Button btScan;
    private Button btConnect;
    private Button btDisconnect;
    //	private Button btReadCurSteps;
    private Button btReadFw;
    private TextView tvSync;
    private Button btSyncPersonalInfo;
    private Button bNotify;
    private Button set_time, getcursportdata, set_parameters;
    private TextView data_text;
    private Button set_userinfo, set_vir, set_photo, set_idletime, set_sleep, read_batery, read_fw, set_alarm, send_msg, set_autoheart, set_fuzhu, set_showmode, openheart, closeheart, getdata;
    private EditText et_getdata, et_getday;
    private Button setLanguage, send_weather, bt_getmutipleSportData, bt_open_blood, bt_close_blood, bt_setgoalstep, bt_setHeartRateArea;
    private ScrollView svDevice;
    /**
     * set_device_code
     */
    private Button bGetDeviceCode;
    /**
     * get_device_code
     */
    private Button bSetDeviceCode;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initView();
        Intent gattServiceIntent = new Intent(this,
                SampleBleService.class);
        gattServiceIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        startService(gattServiceIntent);
    }

    private ScrollView svLog;

    private void initView() {
        svLog = (ScrollView) findViewById(R.id.svLog);

        llConnect = (LinearLayout) findViewById(R.id.llConnect);
        data_text = (TextView) findViewById(R.id.data_text);
        data_text.setOnClickListener(this);
        btBind = (Button) findViewById(R.id.bind);
        btUnbind = (Button) findViewById(R.id.unbind);
        btScan = (Button) findViewById(R.id.scan);
        btConnect = (Button) findViewById(R.id.connect);
        btDisconnect = (Button) findViewById(R.id.disconnect);
        btReadFw = (Button) findViewById(R.id.read_fw);
        btSyncPersonalInfo = (Button) findViewById(R.id.set_alarm);
        set_time = (Button) findViewById(R.id.set_time);
        getcursportdata = (Button) findViewById(R.id.getcursportdata);
        set_parameters = (Button) findViewById(R.id.set_parameters);
        tvSync = (TextView) findViewById(R.id.tvSync);
        bNotify = (Button) findViewById(R.id.bNotify);

        set_userinfo = (Button) findViewById(R.id.set_userinfo);
        set_userinfo.setOnClickListener(this);
        set_vir = (Button) findViewById(R.id.set_vir);
        set_vir.setOnClickListener(this);
        set_photo = (Button) findViewById(R.id.set_photo);
        set_photo.setOnClickListener(this);
        set_idletime = (Button) findViewById(R.id.set_idletime);
        set_idletime.setOnClickListener(this);
        set_sleep = (Button) findViewById(R.id.set_sleep);
        set_sleep.setOnClickListener(this);
        read_batery = (Button) findViewById(R.id.read_batery);
        read_batery.setOnClickListener(this);
        read_fw = (Button) findViewById(R.id.read_fw);
        read_fw.setOnClickListener(this);
        set_alarm = (Button) findViewById(R.id.set_alarm);
        set_alarm.setOnClickListener(this);
        send_msg = (Button) findViewById(R.id.send_msg);
        send_msg.setOnClickListener(this);
        set_autoheart = (Button) findViewById(R.id.set_autoheart);
        set_autoheart.setOnClickListener(this);
        set_fuzhu = (Button) findViewById(R.id.set_fuzhu);
        set_fuzhu.setOnClickListener(this);
        set_showmode = (Button) findViewById(R.id.set_showmode);
        set_showmode.setOnClickListener(this);
        openheart = (Button) findViewById(R.id.openheart);
        openheart.setOnClickListener(this);
        closeheart = (Button) findViewById(R.id.closeheart);
        closeheart.setOnClickListener(this);
        et_getdata = (EditText) findViewById(R.id.et_getdata);
        et_getday = (EditText) findViewById(R.id.et_getday);
        getdata = (Button) findViewById(R.id.getdata);
        getdata.setOnClickListener(this);
        setLanguage = (Button) findViewById(R.id.setLanguage);
        setLanguage.setOnClickListener(this);
        send_weather = (Button) findViewById(R.id.send_weather);
        send_weather.setOnClickListener(this);
        bt_getmutipleSportData = (Button) findViewById(R.id.bt_getmutipleSportData);
        bt_getmutipleSportData.setOnClickListener(this);
        bt_open_blood = (Button) findViewById(R.id.bt_open_blood);
        bt_open_blood.setOnClickListener(this);
        bt_close_blood = (Button) findViewById(R.id.bt_close_blood);
        bt_close_blood.setOnClickListener(this);
        bt_setgoalstep = (Button) findViewById(R.id.bt_setgoalstep);
        bt_setgoalstep.setOnClickListener(this);
        bt_setHeartRateArea = (Button) findViewById(R.id.bt_setHeartRateArea);
        bt_setHeartRateArea.setOnClickListener(this);

        bNotify.setOnClickListener(this);
        findViewById(R.id.bind).setOnClickListener(this);
        findViewById(R.id.unbind).setOnClickListener(this);
        findViewById(R.id.scan).setOnClickListener(this);
        findViewById(R.id.connect).setOnClickListener(this);
        findViewById(R.id.disconnect).setOnClickListener(this);
        findViewById(R.id.read_fw).setOnClickListener(this);
        findViewById(R.id.set_alarm).setOnClickListener(this);
        getcursportdata.setOnClickListener(this);
        set_time.setOnClickListener(this);
        set_parameters.setOnClickListener(this);

        svDevice = (ScrollView) findViewById(R.id.svDevice);
        bGetDeviceCode = (Button) findViewById(R.id.bGetDeviceCode);
        bGetDeviceCode.setOnClickListener(this);
        bSetDeviceCode = (Button) findViewById(R.id.bSetDeviceCode);
        bSetDeviceCode.setOnClickListener(this);
        findViewById(R.id.bGetBandFunction).setOnClickListener(this);
        findViewById(R.id.bSetUuid).setOnClickListener(this);
    }

    Handler updateConnectStateHandler = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(Message msg) {
            //super.handleMessage(msg);
            Bundle data = msg.getData();
            int state = data.getInt("state");

            if (state == 2) {
                btBind.setEnabled(false);
                btUnbind.setEnabled(true);
                btScan.setEnabled(true);
                btConnect.setEnabled(false);
                btDisconnect.setEnabled(true);
                llConnect.setVisibility(View.VISIBLE);
            } else {
                btBind.setEnabled(false);
                btUnbind.setEnabled(true);
                btScan.setEnabled(true);
                btConnect.setEnabled(false);
                btDisconnect.setEnabled(false);
                llConnect.setVisibility(View.GONE);
            }
            return true;
        }
    });

    protected void updateConnectState(int state) {
        Message msg = new Message();
        Bundle data = new Bundle();
        data.putInt("state", state);
        msg.setData(data);
        updateConnectStateHandler.sendMessage(msg);
    }

    protected void showToast(String title, String content) {
        String file = "demo.log";
        if(bSave)
            SysUtils.writeTxtToFile(title + " -> " + content, pathLog, file);
        Message msg = new Message();
        Bundle data = new Bundle();
        data.putString("title", title);
        data.putString("content", content);
        msg.setData(data);
        messageHandler.sendMessage(msg);
    }

    private boolean bColor = false;
    private SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss", Locale.getDefault());
    private Handler messageHandler = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(Message msg) {
            Bundle data = msg.getData();
            String title = data.getString("title");
            String content = data.getString("content");

            Log.i(TAG, title + ": " + content);
            switch (title) {
                default:
                    String text = "[" + sdf.format(new Date()) + "] " + title + "\n" + content;
                    data_text.setText(text);
                    text = "<font color='" + (bColor ? "#00" : "#82") + "'>" + text.replace("\n", "<br>") + "</font><br>";
                    tvSync.append(Html.fromHtml(text));
                    svLog.fullScroll(View.FOCUS_DOWN);
                    bColor = !bColor;
                    break;
            }
            return true;
        }
    });

    Handler scanDeviceHandler = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(Message msg) {
            //super.handleMessage(msg);
            Bundle data = msg.getData();
            String result = data.getString("result");
            nearbyListAdapter.notifyDataSetChanged();
            return true;
        }
    });

    protected String curMac;

    private void callRemoteScanDevice() {
        if (nearbyItemList != null)
            nearbyItemList.clear();

        if (mService != null) {
            try {
                popWindow(findViewById(R.id.scan), R.layout.popwindow_devicelist);
                bStart = !bStart;
                mService.scanDevice(bStart);
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private boolean callRemoteIsConnected() {
        boolean isConnected = false;
        if (mService != null) {
            try {
                isConnected = mService.isConnectBt();
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }

        return isConnected;
    }

    private String callRemoteGetConnectedDevice() {
        String deviceMac = "";
        if (mService != null) {
            try {
                deviceMac = mService.getConnectedDevice();
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }

        return deviceMac;
    }

    private void callRemoteConnect(String name, String mac) {
        if (mac == null || mac.length() == 0) {
            Toast.makeText(this, "ble device mac address is not correctly!", Toast.LENGTH_SHORT).show();
            return;
        }

        if (mService != null) {
            try {
                mService.connectBt(name, mac);
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private void callRemoteDisconnect() {

        if (mService != null) {
            try {
                mService.disconnectBt(true);
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }


    private int callRemoteIsAuthrize() {
        int isAuthrize = 0;
        if (mService != null) {
            try {
                isAuthrize = mService.isAuthrize();
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }

        return isAuthrize;
    }

    private int callRemoteSetOption(BleClientOption opt) {
        int result = 0;
        if (mService != null) {
            try {
                result = mService.setOption(opt);
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }

        return result;
    }


    private int callRemoteSetUserInfo() {
        int result = 0;
        if (mService != null) {
            try {
                result = mService.setUserInfo();
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }

        return result;
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            Process.killProcess(Process.myPid());
            finish();
        }
        return super.onKeyDown(keyCode, event);
    }

    @Override
    protected void onPause() {
        Log.i(TAG, "onPause");
        super.onPause();
    }

    @Override
    protected void onStop() {
        Log.i(TAG, "onStop");

        super.onStop();
    }

    @Override
    protected void onDestroy() {
        Log.i(TAG, "onDestroy");

        if (mIsBound) {
            unbindService(mServiceConnection);
        }
        super.onDestroy();
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.data_text:
                svLog.setVisibility(svLog.getVisibility() == View.VISIBLE ? View.GONE : View.VISIBLE);
                break;
            case R.id.bind:
                Intent intent = new Intent(IRemoteService.class.getName());
                intent.setClassName("com.sxr.sdk.ble.keepfit.client", "com.sxr.sdk.ble.keepfit.client.SampleBleService");
                bindService(intent, mServiceConnection, BIND_AUTO_CREATE);
                mIsBound = true;
                break;
            case R.id.unbind:
                if (mIsBound) {
                    btBind.setEnabled(true);
                    btUnbind.setEnabled(false);
                    btScan.setEnabled(false);
                    btConnect.setEnabled(false);
                    btDisconnect.setEnabled(false);
//        			btReadCurSteps.setEnabled(false);
                    btReadFw.setEnabled(false);

                    btConnect.setText(R.string.connect);
                    btDisconnect.setText(R.string.disconnect);
//    				btReadCurSteps.setText(R.string.read_cur_steps);

                    btSyncPersonalInfo.setEnabled(false);
                    getcursportdata.setEnabled(false);
                    set_time.setEnabled(false);
                    set_parameters.setEnabled(false);

                    try {
                        mService.unregisterCallback(mServiceCallback);
                    } catch (RemoteException e) {
                        e.printStackTrace();
                    }
                    unbindService(mServiceConnection);
                    mIsBound = false;
                }
                break;
            case R.id.scan:
                callRemoteScanDevice();
                break;
            case R.id.disconnect:
                callRemoteDisconnect();
                break;
            case R.id.bNotify:
                callNotify();
                break;
            case R.id.set_parameters:
                callSetParameters();
                break;
            case R.id.set_time:
                callSetDeviceTime();
                break;
            case R.id.set_userinfo:
                UserProfile userProfile = new UserProfile(10000, 170, 60, 50, 0, 1, 24);
                BleClientOption opt = new BleClientOption(userProfile, null, null);
                int result = callRemoteSetOption(opt);
                callRemoteSetUserInfo();
                break;
            case R.id.getcursportdata:
                callgetCurSportData();
                break;
            case R.id.set_vir:
                callSet_vir();
                break;
            case R.id.set_photo:
                callRemoteSetphoto();
                break;
            case R.id.set_idletime:
                callRemoteSetIdletime();
                break;
            case R.id.set_sleep:
                callRemoteSetSleepTime();
                break;
            case R.id.read_batery:
                callRemoteGetDeviceBatery();
                break;
            case R.id.read_fw:
                callRemoteGetDeviceInfo();
                break;
            case R.id.set_alarm:
                callSetAlarm();
                break;
            case R.id.send_msg:
                callGetOtaInfo();
                break;
            case R.id.set_autoheart:
                callRemoteSetAutoHeartMode(true);
                break;
            case R.id.set_fuzhu:
                DeviceProfile deviceProfile = new DeviceProfile(true, true, false, 18, 20, 00, 00);
                BleClientOption opt2 = new BleClientOption(null, deviceProfile, null);
                int result2 = callRemoteSetOption(opt2);
                callRemoteSetDeviceMode();
                break;
            case R.id.set_showmode:
                callRemoteSetHourFormat();
                break;
            case R.id.openheart:
                callRemoteSetHeartRateMode(true);
                break;
            case R.id.closeheart:
                callRemoteSetHeartRateMode(false);
                break;
            case R.id.getdata:
                int type = Integer.valueOf(et_getdata.getText().toString());
                int day = Integer.valueOf(et_getday.getText().toString());

                callRemoteGetData(type, day);
                break;
            case R.id.setLanguage:
                callRemoteSetLanguage();
                break;
            case R.id.send_weather:
//            	Weather weather = new Weather();
                Weather weather = new Weather((int) (System.currentTimeMillis() / 1000), 300, 400, 7, 28, 2, 0, 0, 0, -20); //时间 白天、晚上天气 、最低最高温 空气质量、PM2.5 UV AQI 当前温度
                BleClientOption opt3 = new BleClientOption(null, null, null, weather);
                callRemoteSetOption(opt3);
                callRemoteSetWeather();
                break;
            case R.id.bt_getmutipleSportData:
                callRemoteGetMutipleData(0);
                break;
            case R.id.bt_open_blood:
                callRemoteOpenBlood(true);
                break;
            case R.id.bt_close_blood:
                callRemoteOpenBlood(false);
                break;
            case R.id.bt_setgoalstep:
                callRemoteSetGoalStep(50);
                break;
            case R.id.bt_setHeartRateArea:
                callRemoteSetHeartRateArea(true, 150, 80);
                break;
            case R.id.bGetDeviceCode:
                callGetDeviceCode();
                break;
            case R.id.bSetDeviceCode:
                byte[] bytes = new byte[18];
                for (int i = 0; i < bytes.length; i++)
                    bytes[i] = (byte) (int) (Math.random() * 100);
                callSetDeviceCode(bytes);
                break;
            case R.id.bGetBandFunction:
                callRemoteGetFunction();
                break;
            case R.id.bSetUuid:
                setUuid();
                break;
        }
    }

    private boolean bOpen = false;
    private void setUuid() {
        if (mService != null) {
            try {
                bOpen = !bOpen;
                mService.setUuid(new String[0], new String[0], bOpen);
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }


    private void callGetDeviceCode() {
        if (mService != null) {
            try {
                mService.getDeviceCode();
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private void callSetDeviceCode(byte[] bytes) {
        if (mService != null) {
            try {
                showToast("callSetDeviceCode", SysUtils.printHexString(bytes));
                mService.setDeviceCode(bytes);
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private void callSetParameters() {
        int result;
        if (mService != null) {
            try {
                DeviceProfile deviceProfile = new DeviceProfile(true, true, false, 1, 2, 00, 00);
                BleClientOption opt2 = new BleClientOption(null, deviceProfile, null);
                int result2 = callRemoteSetOption(opt2);
                result = mService.setDeviceInfo();
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private void callSetAlarm() {
        boolean result;
        if (mService != null) {
            try {
                ArrayList<AlarmInfoItem> lAlarmInfo = new ArrayList<AlarmInfoItem>();
                AlarmInfoItem item = new AlarmInfoItem(1, 1, 1, 11, 1, 1, 1, 1, 1, 1, 1, "要睡觉le", false);
                lAlarmInfo.add(item);
                BleClientOption bco = new BleClientOption(null, null, lAlarmInfo);
                mService.setOption(bco);
                mService.setAlarm();
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private void callNotify() {
        boolean result;
        if (mService != null) {
            try {
                String type = ((EditText) findViewById(R.id.etType)).getText().toString();
                String name = ((EditText) findViewById(R.id.etName)).getText().toString();
                String content = ((EditText) findViewById(R.id.etContent)).getText().toString();
                result = mService.setNotify(System.currentTimeMillis() + "", Integer.parseInt(type), name, content);
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private void callGetOtaInfo() {
        int result;
        if (mService != null) {
            try {
                result = mService.getOtaInfo(true);
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private void callSetDeviceTime() {
        int result;
        if (mService != null) {
            try {
                result = mService.setDeviceTime();
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private void callgetCurSportData() {
        int result;
        if (mService != null) {
            try {
                result = mService.getCurSportData();
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private void callSet_vir() {
        int result;
        if (mService != null) {
            try {
                result = mService.sendVibrationSignal(4); //震动4次
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private void callRemoteSetphoto() {
        int result;
        if (mService != null) {
            try {
                result = mService.setPhontMode(true);
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private void callRemoteSetIdletime() {
        int result;
        if (mService != null) {
            try {
                result = mService.setIdleTime(300, 14, 00, 18, 00);
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private void callRemoteSetSleepTime() {
        int result;
        if (mService != null) {
            try {
                result = mService.setSleepTime(12, 00, 14, 00, 22, 00, 8, 00);
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private void callRemoteGetDeviceBatery() {
        int result;
        if (mService != null) {
            try {
                result = mService.getDeviceBatery();
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private void callRemoteGetDeviceInfo() {
        int result;
        if (mService != null) {
            try {
                result = mService.getDeviceInfo();
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private void callRemoteSetDeviceMode() {
        int result;
        if (mService != null) {
            try {
                result = mService.setDeviceMode(3);
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private void callRemoteSetHourFormat() {
        int result;
        if (mService != null) {
            try {
                boolean is24HourFormat = DateFormat.is24HourFormat(this);
                result = mService.setHourFormat(is24HourFormat == true ? 0 : 1);
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private void callRemoteSetHeartRateMode(boolean enable) {
        int result;
        if (mService != null) {
            try {
                result = mService.setHeartRateMode(enable, 60);
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private void callRemoteSetAutoHeartMode(boolean enable) {
        int result;
        if (mService != null) {
            try {
                result = mService.setAutoHeartMode(enable, 18, 00, 19, 00, 15, 2); //18:00 - 19:00  15min 2min
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private void callRemoteGetData(int type, int day) {
        Log.i(TAG, "callRemoteGetData");
        int result;
        if (mService != null) {
            try {
                result = mService.getDataByDay(type, day);
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private void callRemoteGetFunction() {
    	Log.i(TAG, "callRemoteGetFunction");
    	int result;
    	if (mService != null) {
            try {
                result = mService.getBandFunction();
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
	}

    private void callRemoteSetLanguage() {
        Log.i(TAG, "callRemoteGetData");
        int result;
        if (mService != null) {
            try {
                result = mService.setLanguage();
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private void callRemoteSetWeather() {
        Log.i(TAG, "callRemoteGetData");
        int result;
        if (mService != null) {
            try {
                result = mService.sendWeather();
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private void callRemoteGetMutipleData(int day) {
        Log.i(TAG, "callRemoteGetMutipleData");
        int result;
        if (mService != null) {
            try {
                result = mService.getMultipleSportData(day);
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private void callRemoteOpenBlood(boolean enable) {
        Log.i(TAG, "callRemoteGetMutipleData");
        int result;
        if (mService != null) {
            try {
                result = mService.setBloodPressureMode(enable);
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private void callRemoteSetGoalStep(int step) {
        Log.i(TAG, "callRemoteOpenBlood");
        int result;
        if (mService != null) {
            try {
                result = mService.setGoalStep(step);
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    private void callRemoteSetHeartRateArea(boolean enable, int max, int min) {
        Log.i(TAG, "callRemoteOpenBlood");
        int result;
        if (mService != null) {
            try {
                result = mService.setDeviceHeartRateArea(enable, max, min);
            } catch (RemoteException e) {
                e.printStackTrace();
                Toast.makeText(this, "Remote call error!", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, "Service is not available yet!", Toast.LENGTH_SHORT).show();
        }
    }

    public boolean dismissPopWindow() {
        if (window != null) {
            window.dismiss();
            window = null;

            return true;
        }

        return false;
    }

    public void popWindow(View parent, int windowRes) {
        if (window == null) {
            LayoutInflater lay = (LayoutInflater) getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            View popView = lay.inflate(windowRes, null);

            nearbyItemList = new ArrayList<BleDeviceItem>();

            ListView nearbyListView = (ListView) popView.findViewById(R.id.nearby_device_listView);

            nearbyListAdapter = new listDeviceViewAdapter(this, nearbyItemList);
            nearbyListAdapter.setType(listDeviceViewAdapter.DEVICE_NEARBY);
            nearbyListView.setAdapter(nearbyListAdapter);

            nearbyListView.setOnItemClickListener(new OnItemClickListener() {
                @Override
                public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                    try {
                        ViewHolder holder = (ViewHolder) view.getTag();
                        callRemoteScanDevice();
                        callRemoteDisconnect();
                        curMac = holder.mac;
                        callRemoteConnect(holder.name, holder.mac);

                        dismissPopWindow();
                    } catch (Exception e) {
                        e.printStackTrace();
                        Log.e("BLE service", "ble connect ble device: excption");
                    }
                }
            });

            popView.setOnKeyListener(new OnKeyListener() {

                @Override
                public boolean onKey(View arg0, int arg1, KeyEvent arg2) {
                    window.dismiss();
                    window = null;
                    return false;
                }

            });

            window = new PopupWindow(popView, LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT, true);
            window.setOutsideTouchable(true);
            window.setFocusable(true);
            window.update();
            window.showAtLocation(parent, Gravity.CENTER_VERTICAL, 0, 0);
            window.setOnDismissListener(new PopupWindow.OnDismissListener() {
                @Override
                public void onDismiss() {
                    bStart = true;
                    callRemoteScanDevice();
                    window = null;
                }
            });
        }
    }

    private class ViewHolder {
        TextView tvName;
        TextView address;
        TextView rssi;
        String name;
        String mac;
    }

    class listDeviceViewAdapter extends BaseAdapter implements
            OnItemSelectedListener {

        private static final int DEVICE_NEARBY = 0;
        int count = 0;
        private LayoutInflater layoutInflater;
        Context local_context;
        float xDown = 0, yDown = 0, xUp = 0, yUp = 0;
        private List<BleDeviceItem> itemList;
        private int type;
        protected AnimationDrawable adCallBand;

        public listDeviceViewAdapter(Context context, List<BleDeviceItem> list) {
            layoutInflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            //layoutInflater = LayoutInflater.from(context);    
            local_context = context;
            itemList = list;
        }

        public int getCount() {
            return itemList.size();
        }

        public Object getItem(int pos) {
            return pos;
        }

        public long getItemId(int pos) {
            return pos;
        }

        public View getView(int pos, View v, ViewGroup p) {
            View view;
            ViewHolder viewHolder;

            BleDeviceItem item = itemList.get(pos);

            view = layoutInflater.inflate(R.layout.device_listitem_text, null);
            viewHolder = new ViewHolder();

            view.setTag(viewHolder);
            viewHolder.tvName = (TextView) view.findViewById(R.id.ItemTitle);
            viewHolder.address = (TextView) view.findViewById(R.id.ItemDate);
            viewHolder.rssi = (TextView) view.findViewById(R.id.ItemRssi);

            viewHolder.tvName.setText(item.getBleDeviceName());
            viewHolder.address.setText(item.getBleDeviceAddress());
            int rssi = item.getRssi();
            viewHolder.rssi.setText(String.valueOf(rssi));
            viewHolder.name = item.getBleDeviceName();
            viewHolder.mac = item.getBleDeviceAddress();

            return view;
        }

        @Override
        public void onItemSelected(AdapterView<?> parent, View view, int position,
                                   long id) {
        }

        @Override
        public void onNothingSelected(AdapterView<?> parent) {
        }

        public int getType() {
            return type;
        }

        public void setType(int type) {
            this.type = type;
        }

    }

}
