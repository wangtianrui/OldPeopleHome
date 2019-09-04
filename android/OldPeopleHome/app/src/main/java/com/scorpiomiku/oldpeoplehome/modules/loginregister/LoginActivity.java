package com.scorpiomiku.oldpeoplehome.modules.loginregister;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.scorpiomiku.oldpeoplehome.R;
import com.scorpiomiku.oldpeoplehome.base.BaseActivity;
import com.scorpiomiku.oldpeoplehome.modules.children.activity.ChildMainActivity;
import com.scorpiomiku.oldpeoplehome.modules.oldpeople.activity.OldPeopleMainActivity;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

public class LoginActivity extends BaseActivity {


    @BindView(R.id.login_edt_username)
    EditText loginEdtUsername;
    @BindView(R.id.login_edt_password)
    EditText loginEdtPassword;
    @BindView(R.id.login_btn_login)
    Button loginBtnLogin;
    @BindView(R.id.linear_layout_btn_register)
    LinearLayout linearLayoutBtnRegister;
    @BindView(R.id.login_find_pwd)
    TextView loginFindPwd;

    @Override
    protected Handler initHandle() {
        return null;
    }

    @Override
    public void iniview() {
        checkPermission();
    }

    @Override
    public int getLayoutId() {
        return R.layout.activity_login;
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

    @OnClick({R.id.login_btn_login, R.id.linear_layout_btn_register})
    public void onViewClicked(View view) {
        switch (view.getId()) {
            case R.id.login_btn_login:
                Intent intent = new Intent(LoginActivity.this, ChildMainActivity.class);
                startActivity(intent);
                finish();
                break;
            case R.id.linear_layout_btn_register:
                break;
        }
    }


}
