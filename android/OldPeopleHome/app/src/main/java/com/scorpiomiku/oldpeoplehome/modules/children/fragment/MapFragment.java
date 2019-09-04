package com.scorpiomiku.oldpeoplehome.modules.children.fragment;

import android.os.Bundle;
import android.os.Handler;
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
import com.scorpiomiku.oldpeoplehome.R;
import com.scorpiomiku.oldpeoplehome.base.BaseFragment;

import java.util.ArrayList;
import java.util.List;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.Unbinder;

/**
 * Created by ScorpioMiku on 2019/8/21.
 */

public class MapFragment extends BaseFragment {
    @BindView(R.id.map)
    MapView map;
    Unbinder unbinder;
    LatLng GEO_ZHONGBEI = new LatLng(38.019467, 112.455778);

    @Override
    protected Handler initHandle() {
        return null;
    }

    @Override
    protected int getLayoutId() {
        return R.layout.fragment_child_map;
    }

    @Override
    protected void refreshData() {

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
        drawLine();
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
//        LatLng p1 = new LatLng(38.019467, 112.455778);
//        LatLng p2 = new LatLng(38.019467, 112.465778);1
//        LatLng p3 = new LatLng(38.019467, 112.455978);
        List<LatLng> points = new ArrayList<LatLng>();
//        points.add(p1);
//        points.add(p2);
//        points.add(p3);
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
