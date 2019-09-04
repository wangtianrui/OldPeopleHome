package com.scorpiomiku.oldpeoplehome.modules.oldpeople.fragmemt;

import android.os.Bundle;
import android.os.Handler;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.data.Entry;
import com.scorpiomiku.oldpeoplehome.R;
import com.scorpiomiku.oldpeoplehome.base.BaseFragment;
import com.scorpiomiku.oldpeoplehome.modules.oldpeople.activity.OldPeopleMainActivity;
import com.scorpiomiku.oldpeoplehome.utils.ChartUtils;
import com.scorpiomiku.oldpeoplehome.utils.LogUtils;

import java.util.ArrayList;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;
import butterknife.Unbinder;
import me.itangqi.waveloadingview.WaveLoadingView;

/**
 * Created by ScorpioMiku on 2019/8/18.
 */

public class HeartRateFragment extends BaseFragment {
    @BindView(R.id.waveLoadingView)
    WaveLoadingView waveLoadingView;
    @BindView(R.id.heart_rate_text)
    TextView heartRateText;
    @BindView(R.id.begin)
    TextView begin;
    @BindView(R.id.chart)
    LineChart chart;
    Unbinder unbinder;
    @BindView(R.id.diastolic)
    TextView diastolic;
    @BindView(R.id.systolic)
    TextView systolic;
    @BindView(R.id.oxy)
    TextView oxy;
    @BindView(R.id.progress_bar)
    RelativeLayout progressBar;
    private Boolean loading = false;

    @Override
    protected Handler initHandle() {
        return null;
    }

    @Override
    protected int getLayoutId() {
        return R.layout.fragment_old_heart_rate;
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
     * 初始化折线图
     */
    private void initChart() {
        ArrayList<Entry> pointValues = new ArrayList<>();
        int i;
        float[] levels = {20f, 90f, 60f, 88f, 100f};
        pointValues.add(new Entry(0, 0));
        for (i = 0; i < levels.length; i++) {
            pointValues.add(new Entry(i + 1, levels[i]));
        }
        ChartUtils.initSingleLineChart(chart, pointValues, "近15天平均心率", 0xFFF56EC0);
    }

    @Override
    public void refreshUi(String step, String cal, String distance, String sportTime, String[] heartRate, String sleepType) {
        super.refreshUi(step, cal, distance, sportTime, heartRate, sleepType);
        heartRateText.setText(heartRate[5]);
    }


    @OnClick(R.id.begin)
    public void onViewClicked() {
        assert ((OldPeopleMainActivity) getContext()) != null;
        ((OldPeopleMainActivity) getContext()).setHeartRateMode(begin);
    }

    /**
     * 修改数值
     *
     * @param heart
     */
    public void changeText(String heart, String systolic, String diastolic, String oxy) {
        if (systolic.equals(this.systolic.getText().toString())) {
            if (!loading) {
                progressBar.setVisibility(View.VISIBLE);
            }
        } else {
            loading = false;
            progressBar.setVisibility(View.GONE);
        }
        heartRateText.setText(heart);
        this.diastolic.setText(diastolic);
        this.systolic.setText(systolic);
        this.oxy.setText(oxy);
        data.clear();

    }
}
