package com.scorpiomiku.oldpeoplehome.modules.children.fragment;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.google.gson.Gson;
import com.google.gson.JsonArray;
import com.scorpiomiku.oldpeoplehome.R;
import com.scorpiomiku.oldpeoplehome.base.BaseFragment;
import com.scorpiomiku.oldpeoplehome.bean.OldPeople;
import com.scorpiomiku.oldpeoplehome.bean.Sport;
import com.scorpiomiku.oldpeoplehome.utils.LogUtils;

import java.io.IOException;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.Unbinder;
import me.itangqi.waveloadingview.WaveLoadingView;
import me.zhouzhuo.zzhorizontalprogressbar.ZzHorizontalProgressBar;
import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.Response;

/**
 * Created by ScorpioMiku on 2019/8/18.
 */

public class StepFragment extends BaseFragment {
    @BindView(R.id.waveLoadingView)
    WaveLoadingView waveLoadingView;
    @BindView(R.id.target_step)
    TextView targetStep;
    @BindView(R.id.finish_percent)
    TextView finishPercent;
    @BindView(R.id.calorie_holder)
    TextView calorieHolder;
    @BindView(R.id.calorie_text)
    TextView calorieText;
    @BindView(R.id.calorie_pb)
    ZzHorizontalProgressBar caloriePb;
    @BindView(R.id.distance_holder)
    TextView distanceHolder;
    @BindView(R.id.distance_text)
    TextView distanceText;
    @BindView(R.id.distance_pb)
    ZzHorizontalProgressBar distancePb;
    @BindView(R.id.time_holder)
    TextView timeHolder;
    @BindView(R.id.time_text)
    TextView timeText;
    @BindView(R.id.time_pb)
    ZzHorizontalProgressBar timePb;
    Unbinder unbinder;
    @BindView(R.id.step_count)
    TextView stepCount;

    private Sport sport;

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
        return R.layout.fragment_child_step;
    }

    @SuppressLint("SetTextI18n")
    @Override
    protected void refreshData() {
        stepCount.setText(sport.getMotionCount());
        int percent = (int) (Float.valueOf(sport.getMotionCount()) / 150f);
        caloriePb.setProgress(percent - 6);
        distancePb.setProgress(percent - 1);
        timePb.setProgress(percent + 8);
        finishPercent.setText(percent + "");
        calorieText.setText(sport.getMotionEnergy());
        distanceText.setText(sport.getMotionDistance());
        timeText.setText(sport.getMotionTime());
        waveLoadingView.setProgressValue(percent);
    }

    @Override
    protected void initView() {

    }

    @Override
    public void refreshUi(OldPeople oldPeople) {
        super.refreshUi(oldPeople);
        getWebUtils().getSport(oldPeople.getParentId(), new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                LogUtils.loge(e.getMessage());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                try {
                    JsonArray jsonElements = getWebUtils().getJsonArray(response);
                    Gson gson = new Gson();
                    Sport[] sports = gson.fromJson(jsonElements, Sport[].class);
                    sport = sports[sports.length - 1];
                    handler.sendEmptyMessage(1);
                } catch (Exception e) {
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
}
