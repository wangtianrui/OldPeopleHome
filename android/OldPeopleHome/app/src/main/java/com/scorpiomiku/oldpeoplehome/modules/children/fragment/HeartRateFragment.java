package com.scorpiomiku.oldpeoplehome.modules.children.fragment;

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
import com.google.gson.Gson;
import com.google.gson.JsonArray;
import com.scorpiomiku.oldpeoplehome.R;
import com.scorpiomiku.oldpeoplehome.base.BaseFragment;
import com.scorpiomiku.oldpeoplehome.bean.HeartRate;
import com.scorpiomiku.oldpeoplehome.bean.OldPeople;
import com.scorpiomiku.oldpeoplehome.utils.ChartUtils;
import com.scorpiomiku.oldpeoplehome.utils.LogUtils;

import java.io.IOException;
import java.util.ArrayList;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.Unbinder;
import me.itangqi.waveloadingview.WaveLoadingView;
import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.Response;

/**
 * Created by ScorpioMiku on 2019/8/18.
 */

public class HeartRateFragment extends BaseFragment {
    @BindView(R.id.waveLoadingView)
    WaveLoadingView waveLoadingView;
    @BindView(R.id.heart_rate_text)
    TextView heartRateText;
    @BindView(R.id.chart)
    LineChart chart;
    Unbinder unbinder;
    @BindView(R.id.diastolic)
    TextView diastolic;
    @BindView(R.id.systolic)
    TextView systolic;
    @BindView(R.id.oxy)
    TextView oxy;
    private HeartRate heartRate;

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
        return R.layout.fragment_child_heart_rate;
    }

    @Override
    protected void refreshData() {
        heartRateText.setText(heartRate.getRate());
        systolic.setText(heartRate.getRate1());
        diastolic.setText(heartRate.getRate2());
        oxy.setText(heartRate.getOxy());
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
     * 初始化折线图
     */
    private void initChart() {
        ArrayList<Entry> pointValues = new ArrayList<>();
        int i;
        float nowHeart = Float.valueOf(heartRateText.getText().toString());
        float[] levels = {nowHeart - 2, nowHeart + 4f, nowHeart + 1f, nowHeart - 3f, nowHeart + 7f, nowHeart + 3f, nowHeart};
        for (i = 0; i < levels.length; i++) {
            pointValues.add(new Entry(i, levels[i]));
        }
        ChartUtils.initSingleLineChart(chart, pointValues, "近7天平均心率", 0xFFF56EC0);
    }

    @Override
    public void refreshUi(OldPeople oldPeople) {
        super.refreshUi(oldPeople);
        getWebUtils().getHeartRates(oldPeople.getParentId(), new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                LogUtils.loge(e.getMessage());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                try {
                    JsonArray jsonElements = getWebUtils().getJsonArray(response);
                    Gson gson = new Gson();
                    HeartRate[] heartRates = gson.fromJson(jsonElements, HeartRate[].class);
                    heartRate = heartRates[heartRates.length - 1];
                    handler.sendEmptyMessage(1);
                } catch (Exception e) {
                    LogUtils.loge(e.getMessage());
                }
            }
        });
    }
}
