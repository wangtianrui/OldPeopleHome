package com.scorpiomiku.oldpeoplehome.modules.loginregister;

import android.annotation.SuppressLint;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AlertDialog;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.Toast;

import com.scorpiomiku.oldpeoplehome.R;
import com.scorpiomiku.oldpeoplehome.base.BaseActivity;
import com.scorpiomiku.oldpeoplehome.utils.LogUtils;

import java.io.IOException;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;
import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.Response;

/**
 * Created by ScorpioMiku on 2019/8/18.
 */

public class RegisterActivity extends BaseActivity {
    @BindView(R.id.phone)
    EditText phone;
    @BindView(R.id.id_card)
    EditText idCard;
    @BindView(R.id.name)
    EditText name;
    @BindView(R.id.account)
    EditText account;
    @BindView(R.id.man_rb)
    RadioButton manRb;
    @BindView(R.id.woman_rb)
    RadioButton womanRb;
    @BindView(R.id.password)
    EditText password;
    @BindView(R.id.register_button)
    Button registerButton;
    @BindView(R.id.sex_r_g)
    RadioGroup sexRG;
    private String sexText;
    private String phoneText;
    private String nameText;
    private String accountText;
    private String passwordText;
    private String idCardText;


    @SuppressLint("HandlerLeak")
    @Override
    protected Handler initHandle() {
        return new Handler() {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 1:
                        finish();
                        break;
                }
            }
        };
    }

    @Override
    public void iniview() {
        sexRG.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup radioGroup, int checkedId) {
                if (checkedId == manRb.getId()) {
                    sexText = "男";
                } else if (checkedId == womanRb.getId()) {
                    sexText = "女";
                }
            }
        });
    }

    @Override
    public int getLayoutId() {
        return R.layout.activity_register;
    }

    @Override
    public void refreshData() {

    }

    @Override
    public void onPointerCaptureChanged(boolean hasCapture) {

    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // TODO: add setContentView(...) invocation
        ButterKnife.bind(this);
    }

    @OnClick(R.id.register_button)
    public void onViewClicked() {
        dialog();
        getText();
        data.clear();
        data.put("childSex", sexText);
        data.put("childPassword", passwordText);
        data.put("childAccount", accountText);
        data.put("childName", nameText);
        data.put("childLongId", idCardText);
        data.put("childPhone", phoneText);
//        getWebUtils().registerChild(data, new Callback() {
//            @Override
//            public void onFailure(Call call, IOException e) {
//                LogUtils.loge(e.getMessage());
//            }
//
//            @Override
//            public void onResponse(Call call, Response response) throws IOException {
//                handler.sendEmptyMessage(1);
//
//            }
//        });
    }

    /**
     * 获得数据
     */
    private void getText() {
        nameText = name.getText().toString();
        phoneText = phone.getText().toString();
        idCardText = idCard.getText().toString();
        accountText = account.getText().toString();
        passwordText = password.getText().toString();
    }

    private void dialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this).setIcon(R.mipmap.ic_launcher).setTitle("错误提醒")
                .setMessage("注册失败，该账户已存在").setPositiveButton("确定", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        //ToDo: 你想做的事情
                    }
                }).setNegativeButton("取消", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        //ToDo: 你想做的事情
                        dialogInterface.dismiss();
                    }
                });
        builder.create().show();
    }

}
