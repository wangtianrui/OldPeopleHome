package com.scorpiomiku.oldpeoplehome.modules.children.activity;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.NonNull;
import android.support.design.widget.BottomNavigationView;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.Spinner;
import android.widget.TextView;

import com.google.gson.Gson;
import com.google.gson.JsonArray;
import com.google.gson.JsonObject;
import com.mxn.soul.flowingdrawer_core.FlowingDrawer;
import com.mxn.soul.flowingdrawer_core.FlowingMenuLayout;
import com.scorpiomiku.oldpeoplehome.R;
import com.scorpiomiku.oldpeoplehome.adapter.DrawerPeopleListAdapter;
import com.scorpiomiku.oldpeoplehome.adapter.PeopleSpinnerAdapter;
import com.scorpiomiku.oldpeoplehome.base.BaseActivity;
import com.scorpiomiku.oldpeoplehome.base.BaseFragment;
import com.scorpiomiku.oldpeoplehome.bean.OldPeople;
import com.scorpiomiku.oldpeoplehome.bean.User;
import com.scorpiomiku.oldpeoplehome.modules.children.fragment.EnvironmentFragment;
import com.scorpiomiku.oldpeoplehome.modules.children.fragment.HeartRateFragment;
import com.scorpiomiku.oldpeoplehome.modules.children.fragment.MapFragment;
import com.scorpiomiku.oldpeoplehome.modules.children.fragment.SleepFragment;
import com.scorpiomiku.oldpeoplehome.modules.children.fragment.StepFragment;
import com.scorpiomiku.oldpeoplehome.utils.LogUtils;
import com.scorpiomiku.oldpeoplehome.utils.StatusBarUtils;
import com.scorpiomiku.oldpeoplehome.utils.TimeUtils;

import java.io.IOException;
import java.sql.Time;
import java.util.ArrayList;
import java.util.Arrays;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;
import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.Response;

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
    @BindView(R.id.avatar)
    ImageView avatar;
    @BindView(R.id.menu_layout)
    FlowingMenuLayout menuLayout;
    @BindView(R.id.drawerlayout)
    FlowingDrawer drawerlayout;
    @BindView(R.id.drawer_avatar)
    ImageView drawerAvatar;
    @BindView(R.id.drawer_name)
    TextView drawerName;
    @BindView(R.id.drawer_phone)
    TextView drawerPhone;
    @BindView(R.id.people_recycler_view)
    RecyclerView peopleRecyclerView;
    @BindView(R.id.bind_older)
    ImageView bindOlder;
    @BindView(R.id.title_time_text)
    TextView titleTimeText;
    private OldPeople nowSelectOlder;
    private DrawerPeopleListAdapter adapter;
    private PeopleSpinnerAdapter peopleSpinnerAdapter;
    private ArrayList<OldPeople> oldPeoples = new ArrayList<>();
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

    @SuppressLint("HandlerLeak")
    @Override
    protected Handler initHandle() {
        return new Handler() {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 1:
                        getBindOlders();
                        //绑定老人成功
                        break;
                    case 2:
                        LogUtils.shortToast("该老人已绑定过咯");
                        break;
                    case 3:
                        adapter.notifyDataSetChanged();
                        peopleSpinnerAdapter.notifyDataSetChanged();
                        nowSelectOlder = oldPeoples.get(0);
                        if (nowSelectOlder != null) {
                            changeData();
                        }
                        //获取到了所有绑定了的老人
                        break;
                }
            }
        };
    }

    @Override
    public void iniview() {
        setUser((User) getIntent().getSerializableExtra("user"));
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
        initToolbar();
        initDrawer();
        initDrawerRecycler();
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
        spinner.setBackgroundColor(toolBarColors[0]);
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
                spinner.setBackgroundColor(toolBarColors[i]);
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
        peopleSpinnerAdapter
                = new PeopleSpinnerAdapter(getApplication(), oldPeoples);
        spinner.setAdapter(peopleSpinnerAdapter);
        spinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                nowSelectOlder = oldPeoples.get(i);
                changeData();
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });
        titleTimeText.setText(TimeUtils.getUpDate());
    }

    /**
     * 切换老人的数据
     */
    private void changeData() {
        for (int i = 0; i < fragments.length; i++) {
            fragments[i].refreshUi(nowSelectOlder);
        }
    }


    /**
     * 初始化drawer
     */
    private void initDrawer() {
        drawerName.setText(getChildUser().getChildName());
        drawerPhone.setText(getChildUser().getChildPhone());
    }


    @OnClick({R.id.avatar, R.id.bind_older})
    public void onViewClicked(View view) {
        switch (view.getId()) {
            case R.id.avatar:
                drawerlayout.openMenu();
                break;
            case R.id.bind_older:
                dialog();
                break;
        }
    }

    /**
     * 弹出dialog进行老人绑定
     */
    private void dialog() {
        final EditText editText = new EditText(this);
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("输入老人ID").setIcon(R.drawable.ic_bind).setView(editText)
                .setNegativeButton("取消", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        dialogInterface.dismiss();
                    }
                });
        builder.setPositiveButton("确定", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialogInterface, int i) {
                String id = editText.getText().toString();
                data.clear();
                data.put("child", getChildUser().getChildId());
                data.put("parent", id);
                data.put("relation", "父子");
                getWebUtils().childBindOldPeople(data, new Callback() {
                    @Override
                    public void onFailure(Call call, IOException e) {
                        LogUtils.loge(e.getMessage());
                    }

                    @Override
                    public void onResponse(Call call, Response response) throws IOException {
                        try {
                            JsonObject jsonObject = getWebUtils().getJsonObj(response);
                            Gson gson = new Gson();
                            OldPeople oldPeople = gson.fromJson(jsonObject, OldPeople.class);
                            oldPeoples.add(oldPeople);
                            handler.sendEmptyMessage(1);
                        } catch (Exception e) {
                            LogUtils.loge(e.getMessage());
                            handler.sendEmptyMessage(2);
                        }
                    }
                });
            }
        });
        builder.show();
    }

    /**
     * 获取已绑定了的老人
     */
    private void getBindOlders() {
        getWebUtils().getBindOldPeople(getChildUser().getChildId(), new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                LogUtils.loge(e.getMessage());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                try {
                    JsonArray jsonElements = getWebUtils().getJsonArray(response);
                    Gson gson = new Gson();
                    OldPeople[] oldPeople = gson.fromJson(jsonElements, OldPeople[].class);
                    oldPeoples.clear();
                    oldPeoples.addAll(Arrays.asList(oldPeople));
                    handler.sendEmptyMessage(3);
                } catch (Exception e) {
                    LogUtils.loge("没有绑定老人" + e.getMessage());
                }
            }
        });
    }

    /**
     * 初始化列表
     */
    private void initDrawerRecycler() {
        adapter = new DrawerPeopleListAdapter(this, oldPeoples);
        peopleRecyclerView.setAdapter(adapter);
        peopleRecyclerView.setLayoutManager(new LinearLayoutManager(this));
        peopleRecyclerView.setNestedScrollingEnabled(false);
        getBindOlders();
    }
}
