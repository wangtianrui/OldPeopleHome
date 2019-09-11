package com.scorpiomiku.oldpeoplehome.modules.oldpeople.fragmemt;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.RelativeLayout;

import com.scorpiomiku.oldpeoplehome.R;
import com.scorpiomiku.oldpeoplehome.base.BaseFragment;
import com.scorpiomiku.oldpeoplehome.modules.loginregister.LoginActivity;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;
import butterknife.Unbinder;

/**
 * Created by ScorpioMiku on 2019/8/18.
 */

public class MyInformationFragment extends BaseFragment {
    @BindView(R.id.change)
    RelativeLayout change;
    Unbinder unbinder;

    @Override
    protected Handler initHandle() {
        return null;
    }

    @Override
    protected int getLayoutId() {
        return R.layout.fragment_old_my;
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

    @OnClick(R.id.change)
    public void onViewClicked() {
        getActivity().finish();
        Intent intent = new Intent(getContext(), LoginActivity.class);
        startActivity(intent);
    }
}
