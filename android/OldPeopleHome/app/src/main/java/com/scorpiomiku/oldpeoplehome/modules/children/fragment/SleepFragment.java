package com.scorpiomiku.oldpeoplehome.modules.children.fragment;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;
import android.support.v4.view.ViewPager;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.google.gson.Gson;
import com.google.gson.JsonArray;
import com.rbrooks.indefinitepagerindicator.IndefinitePagerIndicator;
import com.scorpiomiku.oldpeoplehome.R;
import com.scorpiomiku.oldpeoplehome.base.BaseFragment;
import com.scorpiomiku.oldpeoplehome.bean.OldPeople;
import com.scorpiomiku.oldpeoplehome.bean.SleepData;
import com.scorpiomiku.oldpeoplehome.modules.oldpeople.fragmemt.*;
import com.scorpiomiku.oldpeoplehome.utils.LogUtils;
import com.scorpiomiku.oldpeoplehome.utils.TimeUtils;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;

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

public class SleepFragment extends BaseFragment {
    @BindView(R.id.waveLoadingView)
    WaveLoadingView waveLoadingView;
    @BindView(R.id.sleep_text)
    TextView sleepText;

    Unbinder unbinder;
    @BindView(R.id.viewpager_pager_indicator)
    IndefinitePagerIndicator viewpagerPagerIndicator;
    @BindView(R.id.view_pager)
    ViewPager viewPager;
    private ArrayList<SleepData> sleepDatas = new ArrayList<>();
    private SleepData yesterday;

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
                        initViewPager();
                        break;
                }
            }
        };
    }


    @Override
    protected int getLayoutId() {
        return R.layout.fragment_child_sleep;
    }

    @Override
    protected void refreshData() {
        sleepText.setText(TimeUtils.getWholeTimeString(TimeUtils.getSleepWholeTime(yesterday)));
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
     * 初始化Viewpager
     */
    private void initViewPager() {
        FragmentManager fragmentManager = getFragmentManager();
        viewPager.setAdapter(new FragmentPagerAdapter(fragmentManager) {
            @Override
            public Fragment getItem(int i) {
                return com.scorpiomiku.oldpeoplehome.modules.oldpeople.fragmemt.SleepViewPagerItem.instance(sleepDatas.get(i));
            }

            @Override
            public int getCount() {
                return sleepDatas.size();
            }
        });
        viewpagerPagerIndicator.attachToViewPager(viewPager);
    }

    @Override
    public void refreshUi(OldPeople oldPeople) {
        super.refreshUi(oldPeople);

        getWebUtils().getSleepData(oldPeople.getParentId(), new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                LogUtils.loge(e.getMessage());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                try {
                    JsonArray jsonElements = getWebUtils().getJsonArray(response);
                    Gson gson = new Gson();
                    SleepData[] sleeps = gson.fromJson(jsonElements, SleepData[].class);
                    Arrays.sort(sleeps);
                    yesterday = sleeps[sleeps.length - 1];
                    sleepDatas.clear();
                    sleepDatas.addAll(Arrays.asList(sleeps));
                    handler.sendEmptyMessage(1);
                } catch (Exception e) {
                    LogUtils.loge(e.getMessage());
                }
            }
        });
    }
}
