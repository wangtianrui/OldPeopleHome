package com.oldpeoplehome.entity;

import com.fasterxml.jackson.annotation.JsonFormat;

import java.sql.Timestamp;

/**
 * Created By Jiangyuwei on 2019/8/22 21:41
 * Description:
 */
public class HeartRate {

    private long parentId;
    private Timestamp time;
    private double rate;

    public HeartRate() {
    }

    public HeartRate(long parentId, String time, double rate) {
        this.parentId = parentId;
        this.time = Timestamp.valueOf(time);
        this.rate = rate;
    }

    public HeartRate(long parentId, Timestamp time, double rate) {
        this.parentId = parentId;
        this.time = time;
        this.rate = rate;
    }

    public long getParentId() {
        return parentId;
    }

    public void setParentId(long parentId) {
        this.parentId = parentId;
    }
    @JsonFormat(pattern="yyyy-MM-dd HH:mm:ss",timezone = "GMT+8")
    public Timestamp getTime() {
        return time;
    }

    public void setTime(Timestamp time) {
        this.time = time;
    }

    public double getRate() {
        return rate;
    }

    public void setRate(double rate) {
        this.rate = rate;
    }

    @Override
    public String toString() {
        return "HeartRateDao{" +
                "parentId=" + parentId +
                ", time=" + time +
                ", rate=" + rate +
                '}';
    }
}
