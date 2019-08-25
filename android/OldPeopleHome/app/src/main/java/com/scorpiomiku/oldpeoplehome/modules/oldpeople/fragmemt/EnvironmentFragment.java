package com.scorpiomiku.oldpeoplehome.modules.oldpeople.fragmemt;

import android.os.Bundle;
import android.os.Handler;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.data.Entry;
import com.scorpiomiku.oldpeoplehome.R;
import com.scorpiomiku.oldpeoplehome.base.BaseFragment;
import com.scorpiomiku.oldpeoplehome.utils.ChartUtils;

import java.util.ArrayList;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.Unbinder;

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

    @Override
    protected Handler initHandle() {
        return null;
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
}
