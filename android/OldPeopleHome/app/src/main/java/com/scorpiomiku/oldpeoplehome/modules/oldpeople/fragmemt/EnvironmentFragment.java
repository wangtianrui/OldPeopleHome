package com.scorpiomiku.oldpeoplehome.modules.oldpeople.fragmemt;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.data.Entry;
import com.google.gson.JsonObject;
import com.scorpiomiku.oldpeoplehome.R;
import com.scorpiomiku.oldpeoplehome.base.BaseFragment;
import com.scorpiomiku.oldpeoplehome.utils.ChartUtils;
import com.scorpiomiku.oldpeoplehome.utils.LogUtils;
import com.scorpiomiku.oldpeoplehome.utils.TimeUtils;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.Unbinder;
import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.Response;

/**
 * Created by ScorpioMiku on 2019/8/18.
 */

public class EnvironmentFragment extends BaseFragment {
    @BindView(R.id.temperature_text)
    TextView temperatureText;
    @BindView(R.id.humidity_text)
    TextView humidityText;
    @BindView(R.id.temperature_chart)
    LineChart temperatureChart;
    @BindView(R.id.humidity_chart)
    LineChart humidityChart;
    Unbinder unbinder;

    private String temperature;
    private String humidity;
    private Boolean first = true;

    @SuppressLint("HandlerLeak")
    @Override
    protected Handler initHandle() {
        return new Handler() {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 1:
                        temperatureText.setText(temperature);
                        humidityText.setText(humidity);
                        upWeather();
                        break;
                }
            }
        };
    }

    @Override
    protected int getLayoutId() {
        return R.layout.fragment_old_environment;
    }

    @Override
    protected void refreshData() {

    }

    @Override
    protected void initView() {
        initChart();
        getWeatherData();
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        // TODO: inflate a fragment view
        View rootView = super.onCreateView(inflater, container, savedInstanceState);
        unbinder = ButterKnife.bind(this, rootView);
        return rootView;
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        unbinder.unbind();
    }

    /**
     * 初始化Chart
     */
    private void initChart() {
        ArrayList<Entry> temperatureValues = new ArrayList<>();
        float[] temperature = {20f, 90f, 60f, 88f};
        temperatureValues.add(new Entry(0, 0));
        for (int i = 0; i < temperature.length; i++) {
            temperatureValues.add(new Entry(i + 1, temperature[i]));
        }
        ChartUtils.initSingleLineChart(temperatureChart, temperatureValues, "近15天平均温度", 0xFF01B67A);


        ArrayList<Entry> humidityValues = new ArrayList<>();
        float[] levels = {20f, 90f, 60f, 88f};
        humidityValues.add(new Entry(0, 0));
        for (int i = 0; i < levels.length; i++) {
            humidityValues.add(new Entry(i + 1, levels[i]));
        }
        ChartUtils.initSingleLineChart(humidityChart, humidityValues, "近15天平均湿度", 0xFF01B67A);
    }

    /**
     * 获取天气
     */
    private void getWeatherData() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                if (!first) {
                    try {
                        Thread.sleep(600000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                } else {
                    first = false;
                }
                getWebUtils().getWeather(new Callback() {
                    @Override
                    public void onFailure(Call call, IOException e) {
                        LogUtils.loge(e.getMessage());
                    }

                    @Override
                    public void onResponse(Call call, Response response) throws IOException {
                        JsonObject jsonObject = getWebUtils().getJsonObj(response);
                        jsonObject = getWebUtils().getJsonObj(jsonObject.get("result").toString());
                        temperature = jsonObject.get("temperature").toString();
                        humidity = String.valueOf(Float.valueOf(jsonObject.get("humidity").toString()) * 100);
                        handler.sendEmptyMessage(1);
                    }
                });
            }
        }).start();
    }

    /**
     * 上传天气数据
     */
    private void upWeather() {
        data.clear();
        data.put("roomId", "1");
        data.put("time", TimeUtils.getTime());
        data.put("temperature", temperature);
        data.put("humidity", humidity);
        data.put("isin", TimeUtils.getIsIn());
        getWebUtils().upRoomState(data, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                LogUtils.loge(e.getMessage());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                LogUtils.logd("上传天气数据成功");
                getWeatherData();
            }
        });
    }
}
