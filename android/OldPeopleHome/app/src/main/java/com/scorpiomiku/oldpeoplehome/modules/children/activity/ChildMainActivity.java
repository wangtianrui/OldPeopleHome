package com.scorpiomiku.oldpeoplehome.modules.children.activity;

import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.design.widget.BottomNavigationView;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.widget.ArrayAdapter;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.Spinner;
import android.widget.SpinnerAdapter;

import com.jaredrummler.materialspinner.MaterialSpinner;
import com.jaredrummler.materialspinner.MaterialSpinnerAdapter;
import com.scorpiomiku.oldpeoplehome.R;
import com.scorpiomiku.oldpeoplehome.adapter.PeopleSpinnerAdapter;
import com.scorpiomiku.oldpeoplehome.base.BaseActivity;
import com.scorpiomiku.oldpeoplehome.base.BaseFragment;
import com.scorpiomiku.oldpeoplehome.modules.children.fragment.EnvironmentFragment;
import com.scorpiomiku.oldpeoplehome.modules.children.fragment.HeartRateFragment;
import com.scorpiomiku.oldpeoplehome.modules.children.fragment.MapFragment;
import com.scorpiomiku.oldpeoplehome.modules.children.fragment.MyInformationFragment;
import com.scorpiomiku.oldpeoplehome.modules.children.fragment.SleepFragment;
import com.scorpiomiku.oldpeoplehome.modules.children.fragment.StepFragment;
import com.scorpiomiku.oldpeoplehome.utils.LogUtils;
import com.scorpiomiku.oldpeoplehome.utils.StatusBarUtils;

import java.util.ArrayList;

import butterknife.BindView;
import butterknife.ButterKnife;

/**
 * Created by ScorpioMiku on 2019/8/21.
 */

public class ChildMainActivity extends BaseActivity {
    @BindView(R.id.spinner)
    Spinner spinner;
    @BindView(R.id.container)
    RelativeLayout container;
    @BindView(R.id.navigation)
    BottomNavigationView navigation;
    @BindView(R.id.tool_bar)
    LinearLayout toolBar;
    private BottomNavigationView.OnNavigationItemSelectedListener mOnNavigationItemSelectedListener;
    private FragmentManager fragmentManager;
    private BaseFragment[] fragments = {
            new StepFragment(),
            new SleepFragment(),
            new HeartRateFragment(),
            new EnvironmentFragment(),
            new MapFragment()
    };
    private int[] barColors = {
            R.color.step_bg,
            R.color.sleep_bg,
            R.color.heart_rate_bg,
            R.color.environment_bg,
            R.color.informaiton_bg
    };
    private int[] toolBarColors = {
            Color.rgb(236, 128, 96),
            Color.rgb(61, 78, 202),
            Color.rgb(214, 63, 156),
            Color.rgb(47, 182, 137),
            Color.rgb(63, 81, 181),
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
                    case R.id.navigation_map:
                        changeFragment(4);
                        return true;

                }
                return false;
            }
        };
        navigation.setOnNavigationItemSelectedListener(mOnNavigationItemSelectedListener);
        initFragmentManager();
        checkPermission();
        initToolbar();
    }

    @Override
    public int getLayoutId() {
        return R.layout.activity_child_main;
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
//        spinner.setBackgroundColor(toolBarColors[0]);
        toolBar.setBackgroundColor(toolBarColors[0]);
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
//                spinner.setBackgroundColor(toolBarColors[i]);
                toolBar.setBackgroundColor(toolBarColors[i]);
            } else {
                fragmentTransaction.hide(fragments[i]);
            }
        }
        fragmentTransaction.commit();
    }


    /**
     * 初始化Toolbar
     */
    private void initToolbar() {
        ArrayList<String> oldPeopleList = new ArrayList<>();
        oldPeopleList.add("Joy");
        oldPeopleList.add("Tom");
        oldPeopleList.add("Jony");
        oldPeopleList.add("Wang");
        PeopleSpinnerAdapter adapter
                = new PeopleSpinnerAdapter(getApplication(), oldPeopleList);
        spinner.setAdapter(adapter);
    }
}
