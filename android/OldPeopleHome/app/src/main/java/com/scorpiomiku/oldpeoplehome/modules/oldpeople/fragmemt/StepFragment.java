package com.scorpiomiku.oldpeoplehome.modules.oldpeople.fragmemt;

import android.os.Bundle;
import android.os.Handler;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.scorpiomiku.oldpeoplehome.R;
import com.scorpiomiku.oldpeoplehome.base.BaseFragment;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.Unbinder;
import me.itangqi.waveloadingview.WaveLoadingView;
import me.zhouzhuo.zzhorizontalprogressbar.ZzHorizontalProgressBar;

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

    @Override
    protected Handler initHandle() {
        return null;
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
     * @param percent
     * @param cal
     * @param distance
     */
    public void refreshUi(String step, String percent, String cal, String distance) {
        nowStep.setText(step);
        calorieText.setText(cal);
        distanceText.setText(distance);
    }
}
