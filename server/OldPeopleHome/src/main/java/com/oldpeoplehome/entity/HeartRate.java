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
    private double rate1;
    private double rate2;
    private double oxy;

    public HeartRate() {
    }

    public HeartRate(long parentId, Timestamp time, double rate1, double rate2, double oxy) {
        this.parentId = parentId;
        this.time = time;
        this.rate1 = rate1;
        this.rate2 = rate2;
        this.oxy = oxy;
    }
    public HeartRate(long parentId, String time, double rate1, double rate2, double oxy) {
        this.parentId = parentId;
        this.time = Timestamp.valueOf(time);
        this.rate1 = rate1;
        this.rate2 = rate2;
        this.oxy = oxy;
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

    public double getRate1() {
        return rate1;
    }

    public void setRate1(double rate1) {
        this.rate1 = rate1;
    }

    public double getRate2() {
        return rate2;
    }

    public void setRate2(double rate2) {
        this.rate2 = rate2;
    }

    public double getOxy() {
        return oxy;
    }

    public void setOxy(double oxy) {
        this.oxy = oxy;
    }

    @Override
    public String toString() {
        return "HeartRate{" +
                "parentId=" + parentId +
                ", time=" + time +
                ", rate1=" + rate1 +
                ", rate2=" + rate2 +
                ", oxy=" + oxy +
                '}';
    }
}
