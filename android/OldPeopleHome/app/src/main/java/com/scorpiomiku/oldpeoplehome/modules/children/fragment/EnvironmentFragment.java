package com.scorpiomiku.oldpeoplehome.modules.children.fragment;

import android.annotation.SuppressLint;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AlertDialog;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.TextView;

import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.data.Entry;
import com.google.gson.Gson;
import com.google.gson.JsonArray;
import com.google.gson.JsonObject;
import com.scorpiomiku.oldpeoplehome.R;
import com.scorpiomiku.oldpeoplehome.base.BaseFragment;
import com.scorpiomiku.oldpeoplehome.bean.OldPeople;
import com.scorpiomiku.oldpeoplehome.bean.RoomState;
import com.scorpiomiku.oldpeoplehome.utils.ChartUtils;
import com.scorpiomiku.oldpeoplehome.utils.LogUtils;

import java.io.IOException;
import java.util.ArrayList;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;
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
    @BindView(R.id.now_time_text)
    TextView nowTimeText;

    private RoomState roomState;
    private OldPeople oldPeople;

    @SuppressLint("HandlerLeak")
    @Override
    protected Handler initHandle() {
        return new Handler() {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 1:
                        refreshData();
                        break;
                }
            }
        };
    }

    @Override
    protected int getLayoutId() {
        return R.layout.fragment_child_environment;
    }

    @Override
    protected void refreshData() {
        nowTimeText.setText(roomState.getTime());
        temperatureText.setText(roomState.getTemperature());
        humidityText.setText(roomState.getHumidity());
        initChart();
    }

    @Override
    protected void initView() {

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
    /**
     * 初始化Chart
     */
    private void initChart() {
        ArrayList<Entry> temperatureValues = new ArrayList<>();
        float nowTemp = Float.valueOf(temperatureText.getText().toString());
        float[] temperature = {nowTemp + 2.41f, nowTemp + 4.94f, nowTemp - 1.62f,
                nowTemp - 3.07f, nowTemp, nowTemp + 2.93f, nowTemp};
        for (int i = 0; i < temperature.length; i++) {
            temperatureValues.add(new Entry(i, temperature[i]));
        }
        ChartUtils.initSingleLineChart(temperatureChart, temperatureValues, "近7天平均温度", 0xFF01B67A);

        float nowHumi = Float.valueOf(humidityText.getText().toString());
        ArrayList<Entry> humidityValues = new ArrayList<>();
        float[] levels = {nowHumi - 14.51f, nowHumi - 4.39f, nowHumi - 9.11f,
                nowHumi + 5.17f, nowHumi + 14.32f, nowHumi + 1.63f, nowHumi};
        for (int i = 0; i < levels.length; i++) {
            humidityValues.add(new Entry(i, levels[i]));
        }
        ChartUtils.initSingleLineChart(humidityChart, humidityValues, "近7天平均湿度", 0xFF01B67A);
    }

    @Override
    public void refreshUi(OldPeople oldPeople) {
        super.refreshUi(oldPeople);
        this.oldPeople = oldPeople;
        getWebUtils().getRoomData(oldPeople.getParentId(), new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                LogUtils.loge(e.getMessage());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                try {
                    JsonArray jsonElements = getWebUtils().getJsonArray(response);
                    Gson gson = new Gson();
                    RoomState[] roomStates = gson.fromJson(jsonElements, RoomState[].class);
                    roomState = roomStates[roomStates.length - 1];
                    handler.sendEmptyMessage(1);
                } catch (Exception e) {
                    LogUtils.loge(e.getMessage());
                }
            }
        });
    }

    @OnClick(R.id.temperature_text)
    public void onViewClicked() {
        dialog();
    }

    /**
     * 弹出dialog进行老人绑定
     */
    private void dialog() {
        final EditText editText = new EditText(getContext());
        AlertDialog.Builder builder = new AlertDialog.Builder(getContext());
        builder.setTitle("设置房间温度").setIcon(R.drawable.ic_bind).setView(editText)
                .setNegativeButton("取消", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        dialogInterface.dismiss();
                    }
                });
        builder.setPositiveButton("确定", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialogInterface, int i) {
                String temp = editText.getText().toString();
                data.clear();
                data.put("temp", temp);

                getWebUtils().changeTem(oldPeople.getParentRoomId(), data, new Callback() {
                    @Override
                    public void onFailure(Call call, IOException e) {
                        LogUtils.loge(e.getMessage());
                    }

                    @Override
                    public void onResponse(Call call, Response response) throws IOException {
                        LogUtils.loge("temp");
                    }
                });
            }
        });
        builder.show();
    }
}
