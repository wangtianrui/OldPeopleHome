package com.scorpiomiku.oldpeoplehome.adapter;

import android.content.Context;
import android.support.annotation.NonNull;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.scorpiomiku.oldpeoplehome.R;
import com.scorpiomiku.oldpeoplehome.bean.OldPeople;

import java.util.List;

/**
 * Created by ScorpioMiku on 2019/9/5.
 */

public class DrawerPeopleListAdapter extends RecyclerView.Adapter<DrawerPeopleListHolder> {
    private Context context;
    private List<OldPeople> list;

    public DrawerPeopleListAdapter(Context context, List<OldPeople> list) {
        this.context = context;
        this.list = list;
    }

    @NonNull
    @Override
    public DrawerPeopleListHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(context).inflate(R.layout.item_drawer_oldpeople, parent, false);
        return new DrawerPeopleListHolder(view);
    }

    @Override
    public void onBindViewHolder(@NonNull DrawerPeopleListHolder holder, int position) {
        holder.bindView(list.get(position), position);
    }

    @Override
    public int getItemCount() {
        return list.size();
    }
}
