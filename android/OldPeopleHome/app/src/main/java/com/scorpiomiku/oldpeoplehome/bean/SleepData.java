package com.scorpiomiku.oldpeoplehome.bean;

import android.support.annotation.NonNull;

import com.scorpiomiku.oldpeoplehome.utils.LogUtils;
import com.scorpiomiku.oldpeoplehome.utils.TimeUtils;

/**
 * Created by ScorpioMiku on 2019/9/4.
 */

public class SleepData implements Comparable<SleepData> {
    private String parentId;
    private String date;
    private String deepTime;
    private String lightTime;
    private String awakeTime;

    public String getParentId() {
        return parentId;
    }

    public void setParentId(String parentId) {
        this.parentId = parentId;
    }

    public String getDate() {
        return date;
    }

    public void setDate(String date) {
        this.date = date;
    }

    public String getDeepTime() {
        return deepTime;
    }

    public void setDeepTime(String deepTime) {
        this.deepTime = deepTime;
    }

    public String getLightTime() {
        return lightTime;
    }

    public void setLightTime(String lightTime) {
        this.lightTime = lightTime;
    }

    public String getAwakeTime() {
        return awakeTime;
    }

    public void setAwakeTime(String awakeTime) {
        this.awakeTime = awakeTime;
    }

    @Override
    public int compareTo(@NonNull SleepData sleepData) {
        return TimeUtils.compareTime(date, sleepData.getDate());
    }

    @Override
    public String toString() {
        return "SleepData{" +
                "parentId='" + parentId + '\'' +
                ", date='" + date + '\'' +
                ", deepTime='" + deepTime + '\'' +
                ", lightTime='" + lightTime + '\'' +
                ", awakeTime='" + awakeTime + '\'' +
                '}';
    }
}
