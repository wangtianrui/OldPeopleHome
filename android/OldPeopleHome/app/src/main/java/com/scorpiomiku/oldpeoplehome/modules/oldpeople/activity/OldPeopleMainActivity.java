package com.scorpiomiku.oldpeoplehome.modules.oldpeople.activity;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.design.widget.BottomNavigationView;
import android.support.v4.app.ActivityCompat;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.v4.content.ContextCompat;
import android.view.MenuItem;
import android.widget.RelativeLayout;

import com.scorpiomiku.oldpeoplehome.R;
import com.scorpiomiku.oldpeoplehome.base.BaseActivity;
import com.scorpiomiku.oldpeoplehome.base.BaseFragment;
import com.scorpiomiku.oldpeoplehome.modules.oldpeople.fragmemt.EnvironmentFragment;
import com.scorpiomiku.oldpeoplehome.modules.oldpeople.fragmemt.HeartRateFragment;
import com.scorpiomiku.oldpeoplehome.modules.oldpeople.fragmemt.MyInformationFragment;
import com.scorpiomiku.oldpeoplehome.modules.oldpeople.fragmemt.SleepFragment;
import com.scorpiomiku.oldpeoplehome.modules.oldpeople.fragmemt.StepFragment;
import com.scorpiomiku.oldpeoplehome.utils.StatusBarUtils;

import butterknife.BindView;
import butterknife.ButterKnife;

/**
 * Created by ScorpioMiku on 2019/8/18.
 */

public class OldPeopleMainActivity extends BaseActivity {
    @BindView(R.id.container)
    RelativeLayout container;
    @BindView(R.id.navigation)
    BottomNavigationView navigation;

    private BottomNavigationView.OnNavigationItemSelectedListener mOnNavigationItemSelectedListener;
    private FragmentManager fragmentManager;
    private BaseFragment[] fragments = {
            new StepFragment(),
            new SleepFragment(),
            new HeartRateFragment(),
            new EnvironmentFragment(),
            new MyInformationFragment()
    };
    private int[] barColors = {
            R.color.step_bg,
            R.color.sleep_bg,
            R.color.heart_rate_bg,
            R.color.environment_bg,
            R.color.colorPrimaryDark
    };

    @Override
    protected Handler initHandle() {
        return null;
    }

    @Override
    public void iniview() {
        fragmentManager = getSupportFragmentManager();
        mOnNavigationItemSelectedListener = new BottomNavigationView.OnNavigationItemSelectedListener() {

            @Override
            public boolean onNavigationItemSelected(@NonNull MenuItem item) {
                switch (item.getItemId()) {
                    case R.id.navigation_step:
                        changeFragment(0);
                        return true;
                    case R.id.navigation_sleep:
                        changeFragment(1);
                        return true;
                    case R.id.navigation_heart_rate:
                        changeFragment(2);
                        return true;
                    case R.id.navigation_environmental:
                        changeFragment(3);
                        return true;
                    case R.id.navigation_user:
                        changeFragment(4);
                        return true;
                }
                return false;
            }
        };
        navigation.setOnNavigationItemSelectedListener(mOnNavigationItemSelectedListener);
        initFragmentManager();
        checkPermission();
    }

    @Override
    public int getLayoutId() {
        return R.layout.activity_old_people_main;
    }

    @Override
    public void refreshData() {

    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // TODO: add setContentView(...) invocation
        ButterKnife.bind(this);
    }

    /**
     * 初始化fragment管理器
     */
    private void initFragmentManager() {
        FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
        fragmentTransaction.add(R.id.container, fragments[0]);
        fragmentTransaction.add(R.id.container, fragments[1]);
        fragmentTransaction.add(R.id.container, fragments[2]);
        fragmentTransaction.add(R.id.container, fragments[3]);
        fragmentTransaction.add(R.id.container, fragments[4]);

        fragmentTransaction.hide(fragments[1]);
        fragmentTransaction.hide(fragments[2]);
        fragmentTransaction.hide(fragments[3]);
        fragmentTransaction.hide(fragments[4]);
        fragmentTransaction.commit();
        StatusBarUtils.setWindowStatusBarColor(this, barColors[0]);
    }

    /**
     * 修改fragment
     *
     * @param index
     */
    private void changeFragment(int index) {
        FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
        for (int i = 0; i < fragments.length; i++) {
            if (i == index) {
                fragmentTransaction.show(fragments[i]);
                StatusBarUtils.setWindowStatusBarColor(this, barColors[i]);
            } else {
                fragmentTransaction.hide(fragments[i]);
            }
        }
        fragmentTransaction.commit();
    }


}
