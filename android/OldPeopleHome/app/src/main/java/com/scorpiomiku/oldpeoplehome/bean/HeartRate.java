package com.scorpiomiku.oldpeoplehome.bean;

/**
 * Created by ScorpioMiku on 2019/9/5.
 */

public class HeartRate {
    private String parentId;
    private String time;
    private String rate1;
    private String rate2;
    private String oxy;
    private String rate;

    @Override
    public String toString() {
        return "HeartRate{" +
                "parentId='" + parentId + '\'' +
                ", time='" + time + '\'' +
                ", rate1='" + rate1 + '\'' +
                ", rate2='" + rate2 + '\'' +
                ", oxy='" + oxy + '\'' +
                ", rate='" + rate + '\'' +
                '}';
    }

    public String getRate() {
        return rate;
    }

    public void setRate(String rate) {
        this.rate = rate;
    }

    public String getParentId() {
        return parentId;
    }

    public void setParentId(String parentId) {
        this.parentId = parentId;
    }

    public String getTime() {
        return time;
    }

    public void setTime(String time) {
        this.time = time;
    }

    public String getRate1() {
        return rate1;
    }

    public void setRate1(String rate1) {
        this.rate1 = rate1;
    }

    public String getRate2() {
        return rate2;
    }

    public void setRate2(String rate2) {
        this.rate2 = rate2;
    }

    public String getOxy() {
        return oxy;
    }

    public void setOxy(String oxy) {
        this.oxy = oxy;
    }
}
