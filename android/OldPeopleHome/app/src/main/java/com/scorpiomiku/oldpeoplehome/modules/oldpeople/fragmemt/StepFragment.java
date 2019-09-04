package com.scorpiomiku.oldpeoplehome.modules.oldpeople.fragmemt;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.scorpiomiku.oldpeoplehome.R;
import com.scorpiomiku.oldpeoplehome.base.BaseFragment;
import com.scorpiomiku.oldpeoplehome.utils.LogUtils;
import com.scorpiomiku.oldpeoplehome.utils.TimeUtils;

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
    @BindView(R.id.now_step)
    TextView nowStep;
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
    @BindView(R.id.title_time_text)
    TextView titleTimeText;

    @SuppressLint("HandlerLeak")
    @Override
    protected Handler initHandle() {
        return new Handler() {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 1:
                        getWebUtils().UpSport(data, new okhttp3.Callback() {
                            @Override
                            public void onFailure(Call call, IOException e) {
                                LogUtils.loge(e.getMessage());
                            }

                            @Override
                            public void onResponse(Call call, Response response) throws IOException {
                                LogUtils.logd("运动数据上传成功");
                            }
                        });
                        break;
                }
            }
        };
    }

    @Override
    protected int getLayoutId() {
        return R.layout.fragment_old_step;
    }

    @Override
    protected void refreshData() {

    }

    @Override
    protected void initView() {

    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        // TODO: inflate a fragment view
        View rootView = super.onCreateView(inflater, container, savedInstanceState);
        unbinder = ButterKnife.bind(this, rootView);
        titleTimeText.setText(TimeUtils.getUpDate());
        return rootView;
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        unbinder.unbind();
    }

    /**
     * 刷新数据
     *
     * @param step
     * @param cal
     * @param distance
     * @param sportTime
     * @param heartRate
     * @param sleepType
     */
    @Override
    public void refreshUi(String step, String cal, String distance, String sportTime, String[] heartRate, String sleepType) {
        super.refreshUi(step, cal, distance, sportTime, heartRate, sleepType);
        nowStep.setText(step);
        calorieText.setText(cal);
        distanceText.setText(distance);
        timeText.setText(sportTime);
        data.clear();
        data.put("parent", "1");
        data.put("date", TimeUtils.getUpDate());
        data.put("count", step);
        data.put("distance", distance);
        data.put("time", sportTime);
        data.put("energy", cal);
        handler.sendEmptyMessage(1);
        float temp = Float.valueOf(step) / 10000 * 100;
        waveLoadingView.setProgressValue((int) temp);
        caloriePb.setProgress((int) (temp - 6));
        distancePb.setProgress((int) (temp - 1));
        timePb.setProgress((int) (temp + 8));
        finishPercent.setText((int) temp + "");
    }
}
