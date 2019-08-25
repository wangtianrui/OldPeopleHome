package com.scorpiomiku.oldpeoplehome.adapter;

import android.annotation.SuppressLint;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.scorpiomiku.oldpeoplehome.R;

import java.util.List;

/**
 * Created by ScorpioMiku on 2019/8/21.
 */

public class PeopleSpinnerAdapter extends BaseAdapter {
    private Context context;
    private List<String> people;

    public PeopleSpinnerAdapter(Context context, List<String> people) {
        this.context = context;
        this.people = people;
    }

    @Override
    public int getCount() {
        return people.size();
    }

    @Override
    public Object getItem(int i) {
        return people.get(i);
    }

    @Override
    public long getItemId(int i) {
        return i;
    }

    @SuppressLint("ViewHolder")
    @Override
    public View getView(int i, View view, ViewGroup viewGroup) {
        view = LayoutInflater.from(context).inflate(R.layout.child_main_spinner_item, null);
        if (view != null) {
            ImageView avatar = view.findViewById(R.id.spinner_avatar);
            TextView name = view.findViewById(R.id.spinner_name);
            name.setText(people.get(i));
        }
        return view;
    }


}
