package com.scorpiomiku.oldpeoplehome.modules.children.fragment;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.baidu.mapapi.CoordType;
import com.baidu.mapapi.SDKInitializer;
import com.baidu.mapapi.map.MapStatus;
import com.baidu.mapapi.map.MapStatusUpdate;
import com.baidu.mapapi.map.MapStatusUpdateFactory;
import com.baidu.mapapi.map.MapView;
import com.baidu.mapapi.map.Overlay;
import com.baidu.mapapi.map.OverlayOptions;
import com.baidu.mapapi.map.PolylineOptions;
import com.baidu.mapapi.model.LatLng;
import com.google.gson.Gson;
import com.google.gson.JsonArray;
import com.scorpiomiku.oldpeoplehome.R;
import com.scorpiomiku.oldpeoplehome.base.BaseFragment;
import com.scorpiomiku.oldpeoplehome.bean.Location;
import com.scorpiomiku.oldpeoplehome.modules.oldpeople.activity.OldPeopleMainActivity;
import com.scorpiomiku.oldpeoplehome.utils.LogUtils;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Locale;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.Unbinder;
import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.Response;

/**
 * Created by ScorpioMiku on 2019/8/21.
 */

public class MapFragment extends BaseFragment {
    @BindView(R.id.map)
    MapView map;
    Unbinder unbinder;
    LatLng GEO_ZHONGBEI = new LatLng(38.019467, 112.455778);

    private ArrayList<Location> locations = new ArrayList<>();

    @SuppressLint("HandlerLeak")
    @Override
    protected Handler initHandle() {
        return new Handler() {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 1:
                        drawLine();
                        break;
                }
            }
        };
    }

    @Override
    protected int getLayoutId() {
        return R.layout.fragment_child_map;
    }

    @Override
    protected void refreshData() {
        getWebUtils().getLocation(((OldPeopleMainActivity) getActivity()).getOldPeopleUser().getParentId(), new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                LogUtils.loge(e.getMessage());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                try {
                    JsonArray jsonElements = getWebUtils().getJsonArray(response);
                    Gson gson = new Gson();
                    Location[] locas = gson.fromJson(jsonElements, Location[].class);
                    locations.clear();
                    locations.addAll(Arrays.asList(locas));
                    handler.sendEmptyMessage(1);
                } catch (Exception e) {
                    LogUtils.loge(e.getMessage());
                }
            }
        });
    }

    @Override
    protected void initView() {
        //设置默认位置
        MapStatusUpdate status = MapStatusUpdateFactory.newLatLng(GEO_ZHONGBEI);
        map.getMap().setMapStatus(status);
        //设置缩放级别
        MapStatus.Builder builder = new MapStatus.Builder();
        builder.zoom(16.5f);
        map.getMap().setMapStatus(MapStatusUpdateFactory.newMapStatus(builder.build()));
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        // TODO: inflate a fragment view
        SDKInitializer.initialize(getActivity().getApplicationContext());
        SDKInitializer.setCoordType(CoordType.BD09LL);
        View rootView = super.onCreateView(inflater, container, savedInstanceState);
        unbinder = ButterKnife.bind(this, rootView);
        return rootView;
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        unbinder.unbind();
    }

    @Override
    public void onResume() {
        super.onResume();
        //在activity执行onResume时执行mMapView. onResume ()，实现地图生命周期管理
        map.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();
        //在activity执行onPause时执行mMapView. onPause ()，实现地图生命周期管理
        map.onPause();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        //在activity执行onDestroy时执行mMapView.onDestroy()，实现地图生命周期管理
        map.onDestroy();
    }

    /**
     * 轨迹
     */
    private void drawLine() {
        List<LatLng> points = new ArrayList<LatLng>();
        for (int i = 0; i < locations.size(); i++) {
            points.add(new LatLng(
                    Double.valueOf(locations.get(i).getLatitude()),
                    Double.valueOf(locations.get(i).getLongitude())
            ));
        }
        //设置折线的属性
        OverlayOptions mOverlayOptions = new PolylineOptions()
                .width(10)
                .color(0xAAFF0000)
                .points(points);
        //在地图上绘制折线
        //mPloyline 折线对象
        Overlay mPolyline = map.getMap().addOverlay(mOverlayOptions);
    }
}
