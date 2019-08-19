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

import java.util.ArrayList;
import java.util.List;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.Unbinder;
import me.itangqi.waveloadingview.WaveLoadingView;

/**
 * Created by ScorpioMiku on 2019/8/18.
 */

public class SleepFragment extends BaseFragment {
    @BindView(R.id.waveLoadingView)
    WaveLoadingView waveLoadingView;
    @BindView(R.id.sleep_text)
    TextView sleepText;
    @BindView(R.id.sleep_pie)
    PieChart sleepPie;
    Unbinder unbinder;

    @Override
    protected Handler initHandle() {
        return null;
    }

    @Override
    protected int getLayoutId() {
        return R.layout.fragment_old_sleep;
    }

    @Override
    protected void refreshData() {

    }

    @Override
    protected void initView() {
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
        strings.add(new PieEntry(30f, "深度睡眠"));
        strings.add(new PieEntry(70f, "浅度睡眠"));

        PieDataSet dataSet = new PieDataSet(strings, "");

        ArrayList<Integer> colors = new ArrayList<Integer>();
        colors.add(getResources().getColor(R.color.sleep_deep));
        colors.add(getResources().getColor(R.color.sleep_shallow));
        dataSet.setColors(colors);
        dataSet.setValueTextSize(16f);
        PieData pieData = new PieData(dataSet);
        pieData.setDrawValues(true);
        pieData.setValueFormatter(new PercentFormatter());
        pieData.setValueTextSize(16f);
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
