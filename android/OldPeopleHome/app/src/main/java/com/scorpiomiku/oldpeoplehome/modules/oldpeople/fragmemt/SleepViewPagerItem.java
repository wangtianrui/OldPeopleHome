package com.scorpiomiku.oldpeoplehome.modules.oldpeople.fragmemt;

import android.os.Bundle;
import android.os.Handler;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.github.mikephil.charting.charts.PieChart;
import com.github.mikephil.charting.components.Description;
import com.github.mikephil.charting.components.Legend;
import com.github.mikephil.charting.data.PieData;
import com.github.mikephil.charting.data.PieDataSet;
import com.github.mikephil.charting.data.PieEntry;
import com.github.mikephil.charting.formatter.PercentFormatter;
import com.scorpiomiku.oldpeoplehome.R;
import com.scorpiomiku.oldpeoplehome.base.BaseFragment;
import com.scorpiomiku.oldpeoplehome.bean.SleepData;
import com.scorpiomiku.oldpeoplehome.utils.TimeUtils;

import java.sql.Time;
import java.util.ArrayList;
import java.util.List;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.Unbinder;

/**
 * Created by ScorpioMiku on 2019/8/20.
 */

public class SleepViewPagerItem extends BaseFragment {
    @BindView(R.id.pie_chart)
    PieChart sleepPie;
    Unbinder unbinder;
    @BindView(R.id.time_text)
    TextView timeText;
    @BindView(R.id.deep_text_view)
    TextView deepTextView;
    @BindView(R.id.light_text_view)
    TextView lightTextView;
    @BindView(R.id.awake_text_view)
    TextView awakeTextView;
    private SleepData sleepData;
    private float deepPercent;
    private float lightPercent;
    private float awakePercent;


    public static SleepViewPagerItem instance(SleepData sleepData) {
        SleepViewPagerItem sleepViewPagerItem = new SleepViewPagerItem();
        sleepViewPagerItem.setSleepData(sleepData);
        return sleepViewPagerItem;
    }

    public void setSleepData(SleepData sleepData) {
        this.sleepData = sleepData;
    }

    @Override
    protected Handler initHandle() {
        return null;
    }

    @Override
    protected int getLayoutId() {
        return R.layout.fragment_sleep_item;
    }

    @Override
    protected void refreshData() {

    }

    @Override
    protected void initView() {
        deepTextView.setText(sleepData.getDeepTime());
        lightTextView.setText(sleepData.getLightTime());
        awakeTextView.setText(sleepData.getAwakeTime());
        timeText.setText(sleepData.getDate());
        int wholeTime = TimeUtils.getSleepWholeTime(sleepData);
        deepPercent = ((float) TimeUtils.timeString2Second(sleepData.getDeepTime())) / wholeTime * 100f;
        lightPercent = ((float) TimeUtils.timeString2Second(sleepData.getLightTime())) / wholeTime * 100f;
        awakePercent = ((float) TimeUtils.timeString2Second(sleepData.getAwakeTime())) / wholeTime * 100f;
        initPie();
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
     * 绘制饼图
     */
    private void initPie() {
        List<PieEntry> strings = new ArrayList<>();
        strings.add(new PieEntry(deepPercent, "深度睡眠"));
        strings.add(new PieEntry(lightPercent, "浅度睡眠"));
        strings.add(new PieEntry(awakePercent, "清醒时间"));

        PieDataSet dataSet = new PieDataSet(strings, "");

        ArrayList<Integer> colors = new ArrayList<Integer>();
        colors.add(getResources().getColor(R.color.sleep_deep));
        colors.add(getResources().getColor(R.color.sleep_shallow));
        colors.add(getResources().getColor(R.color.sleep_no));
        dataSet.setColors(colors);
        dataSet.setValueTextSize(12f);
        PieData pieData = new PieData(dataSet);
        pieData.setDrawValues(true);
        pieData.setValueFormatter(new PercentFormatter());
        pieData.setValueTextSize(12f);
        pieData.setValueTextColor(getResources().getColor(R.color.place_holder));


        sleepPie.setData(pieData);
        sleepPie.invalidate();

        Description description = new Description();
        description.setText("");
        sleepPie.setDescription(description);
        sleepPie.setHoleRadius(0f);
        sleepPie.setTransparentCircleRadius(0f);
        Legend legend = sleepPie.getLegend();
        legend.setEnabled(false);

    }
}
