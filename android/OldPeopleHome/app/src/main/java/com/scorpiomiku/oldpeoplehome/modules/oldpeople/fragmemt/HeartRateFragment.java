package com.scorpiomiku.oldpeoplehome.modules.oldpeople.fragmemt;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.data.Entry;
import com.google.gson.Gson;
import com.google.gson.JsonArray;
import com.scorpiomiku.oldpeoplehome.R;
import com.scorpiomiku.oldpeoplehome.base.BaseFragment;
import com.scorpiomiku.oldpeoplehome.bean.HeartRate;
import com.scorpiomiku.oldpeoplehome.modules.oldpeople.activity.OldPeopleMainActivity;
import com.scorpiomiku.oldpeoplehome.utils.ChartUtils;
import com.scorpiomiku.oldpeoplehome.utils.LogUtils;
import com.scorpiomiku.oldpeoplehome.utils.TimeUtils;

import java.io.IOException;
import java.util.ArrayList;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;
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
    @BindView(R.id.title_time_text)
    TextView titleTimeText;
    private Boolean loading = false;
    private float heartRate;
    private boolean firstInit = true;
    private float firstHeart;

    @SuppressLint("HandlerLeak")
    @Override
    protected Handler initHandle() {
        return new Handler() {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 1:
                        getWebUtils().upHeartRates(data, new okhttp3.Callback() {
                            @Override
                            public void onFailure(Call call, IOException e) {
                                LogUtils.loge(e.getMessage());
                            }

                            @Override
                            public void onResponse(Call call, Response response) throws IOException {
                                LogUtils.logd("心率上传成功");
                            }
                        });
                        break;
                    case 2:
                        //获取数据
                        initChart();
                        break;
                }
            }
        };
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
        titleTimeText.setText(TimeUtils.getUpDate());
        getWebUtils().getHeartRates(((OldPeopleMainActivity) getActivity()).getOldPeopleUser().getParentId(), new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                LogUtils.loge(e.getMessage());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                try {
                    JsonArray jsonElements = getWebUtils().getJsonArray(response);
                    LogUtils.loge(jsonElements.toString());
                    Gson gson = new Gson();
                    HeartRate[] heartRates = gson.fromJson(jsonElements, HeartRate[].class);
                    firstHeart = Float.valueOf(heartRates[heartRates.length - 1].getRate());
                    handler.sendEmptyMessage(2);
                }catch (Exception e){
                    LogUtils.loge(e.getMessage());
                }
            }
        });
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
        if (firstInit) {
            ArrayList<Entry> pointValues = new ArrayList<>();
            int i;
            float[] levels = {firstHeart - 2, firstHeart + 4f, firstHeart + 1f, firstHeart - 3f, firstHeart + 7f, firstHeart + 3f, firstHeart};
            for (i = 0; i < levels.length; i++) {
                pointValues.add(new Entry(i, levels[i]));
            }
            ChartUtils.initSingleLineChart(chart, pointValues, "近7天平均心率", 0xFFF56EC0);
            firstInit = !firstInit;
        } else {
            ArrayList<Entry> pointValues = new ArrayList<>();
            int i;
            float nowHeart = Float.valueOf(heartRateText.getText().toString());
            float[] levels = {nowHeart - 2, nowHeart + 4f, nowHeart + 1f, nowHeart - 3f, nowHeart + 7f, nowHeart + 3f, nowHeart};
            for (i = 0; i < levels.length; i++) {
                pointValues.add(new Entry(i, levels[i]));
            }
            ChartUtils.initSingleLineChart(chart, pointValues, "近7天平均心率", 0xFFF56EC0);
        }
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
            }
        } else {
            loading = false;
            progressBar.setVisibility(View.GONE);
            data.clear();
            data.put("parentId", ((OldPeopleMainActivity) getActivity()).getOldPeopleUser().getParentId());
            data.put("time", TimeUtils.getTime());
            data.put("rate1", systolic);
            data.put("rate2", diastolic);
            data.put("rate", heart);
            data.put("oxy", oxy);
            initChart();
            handler.sendEmptyMessage(1);
        }
        heartRateText.setText(heart);
        this.diastolic.setText(systolic);
        this.systolic.setText(diastolic);
        this.oxy.setText(oxy);
    }
}
