package com.scorpiomiku.oldpeoplehome.utils;

import android.graphics.Color;
import android.text.format.DateFormat;

import com.github.mikephil.charting.animation.Easing;
import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.components.AxisBase;
import com.github.mikephil.charting.components.Description;
import com.github.mikephil.charting.components.Legend;
import com.github.mikephil.charting.components.XAxis;
import com.github.mikephil.charting.components.YAxis;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;
import com.github.mikephil.charting.formatter.IAxisValueFormatter;
import com.github.mikephil.charting.formatter.IndexAxisValueFormatter;
import com.github.mikephil.charting.interfaces.datasets.ILineDataSet;
import com.scorpiomiku.oldpeoplehome.ui.MyMarkerView;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * Created by ScorpioMiku on 2019/7/4.
 */

public class ChartUtils {
    /**
     * 创建一条折线
     *
     * @param linename
     * @return
     */
    public static void initSingleLineChart(LineChart mLineChart, List<Entry> list, String linename, int fillColor) {

        LineDataSet dataSet = new LineDataSet(list, "Label"); // add entries to dataset
        dataSet.setColor(fillColor);//线条颜色
        dataSet.setCircleColor(Color.parseColor("#7d7d7d"));//圆点颜色
        dataSet.setDrawFilled(true);
        dataSet.setFillColor(fillColor);
        dataSet.setLineWidth(1f);//线条宽度

        //设置样式
        YAxis rightAxis = mLineChart.getAxisRight();

        //设置图表右边的y轴禁用
        rightAxis.setEnabled(false);
        YAxis leftAxis = mLineChart.getAxisLeft();
        //设置图表左边的y轴禁用
        leftAxis.setEnabled(false);
        //设置x轴
        XAxis xAxis = mLineChart.getXAxis();
        xAxis.setTextColor(Color.parseColor("#333333"));
        xAxis.setTextSize(11f);
        xAxis.setAxisMinimum(0f);
        xAxis.setDrawAxisLine(true);//是否绘制轴线
        xAxis.setDrawGridLines(false);//设置x轴上每个点对应的线
        xAxis.setDrawLabels(true);//绘制标签  指x轴上的对应数值
        xAxis.setPosition(XAxis.XAxisPosition.BOTTOM);//设置x轴的显示位置
        xAxis.setGranularity(1f);//禁止放大后x轴标签重绘
        List<String> xList = new ArrayList<>();
        int today = TimeUtils.getDay();
        for (int i = list.size() - 1; i >= 0; i--) {
            int temp = today - i;
            if (temp <= 0) {
                temp += 31;
            }
            xList.add(String.valueOf(temp).concat("号"));
        }
        xAxis.setValueFormatter(new IndexAxisValueFormatter(xList));

        //透明化图例
        Legend legend = mLineChart.getLegend();
        legend.setForm(Legend.LegendForm.NONE);
        legend.setTextColor(Color.WHITE);

        //隐藏x轴描述
        Description description = new Description();
        description.setText(linename);
        description.setTextSize(12f);
//        description.setTextColor(0x000);
        description.setTextColor(Color.WHITE);
        mLineChart.setDescription(description);

        //chart设置数据
        LineData lineData = new LineData(dataSet);
        //是否绘制线条上的文字
        lineData.setDrawValues(true);
        mLineChart.setData(lineData);
        mLineChart.invalidate(); // refresh


    }

}
