package com.scorpiomiku.oldpeoplehome.adapter;

import android.support.v7.widget.RecyclerView;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import com.bumptech.glide.Glide;
import com.scorpiomiku.oldpeoplehome.R;
import com.scorpiomiku.oldpeoplehome.bean.OldPeople;
import com.scorpiomiku.oldpeoplehome.utils.StaticUtils;

import butterknife.BindView;
import butterknife.ButterKnife;

/**
 * Created by ScorpioMiku on 2019/9/5.
 */

public class DrawerPeopleListHolder extends RecyclerView.ViewHolder {
    @BindView(R.id.avatar)
    ImageView avatar;
    @BindView(R.id.name)
    TextView name;

    public DrawerPeopleListHolder(View itemView) {
        super(itemView);
        ButterKnife.bind(this, itemView);
    }

    public void bindView(OldPeople oldPeople, int position) {
        Glide.with(itemView.getContext()).load(StaticUtils.oldPeopleAvatars[position]).into(avatar);
        name.setText(oldPeople.getParentName());
    }
}
