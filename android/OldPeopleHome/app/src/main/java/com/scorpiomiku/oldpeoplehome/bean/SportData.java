package com.scorpiomiku.oldpeoplehome.bean;

/**
 * Created by ScorpioMiku on 2019/8/23.
 */

public class SportData {

    private String oldPeopleId;
    private int type;
    private String time;
    private int step;
    private int distance;
    private int cal;
    private int cursleeptime;
    private int totalrunningtime;
    private int steptime;

    public SportData(int type, String time, int step, int distance, int cal, int cursleeptime,
                     int totalrunningtime, int steptime, String oldPeopleId) {
        this.type = type;
        this.time = time;
        this.step = step;
        this.distance = distance;
        this.cal = cal;
        this.cursleeptime = cursleeptime;
        this.totalrunningtime = totalrunningtime;
        this.steptime = steptime;
        this.oldPeopleId = oldPeopleId;
    }

    @Override
    public String toString() {
        return "SportData{" +
                "oldPeopleId='" + oldPeopleId + '\'' +
                ", type=" + type +
                ", time='" + time + '\'' +
                ", step=" + step +
                ", distance=" + distance +
                ", cal=" + cal +
                ", cursleeptime=" + cursleeptime +
                ", totalrunningtime=" + totalrunningtime +
                ", steptime=" + steptime +
                '}';
    }

    public String getOldPeopleId() {
        return oldPeopleId;
    }

    public void setOldPeopleId(String oldPeopleId) {
        this.oldPeopleId = oldPeopleId;
    }

    public int getType() {
        return type;
    }

    public void setType(int type) {
        this.type = type;
    }

    public String getTime() {
        return time;
    }

    public void setTime(String time) {
        this.time = time;
    }

    public int getStep() {
        return step;
    }

    public void setStep(int step) {
        this.step = step;
    }

    public int getDistance() {
        return distance;
    }

    public void setDistance(int distance) {
        this.distance = distance;
    }

    public int getCal() {
        return cal;
    }

    public void setCal(int cal) {
        this.cal = cal;
    }

    public int getCursleeptime() {
        return cursleeptime;
    }

    public void setCursleeptime(int cursleeptime) {
        this.cursleeptime = cursleeptime;
    }

    public int getTotalrunningtime() {
        return totalrunningtime;
    }

    public void setTotalrunningtime(int totalrunningtime) {
        this.totalrunningtime = totalrunningtime;
    }

    public int getSteptime() {
        return steptime;
    }

    public void setSteptime(int steptime) {
        this.steptime = steptime;
    }
}
