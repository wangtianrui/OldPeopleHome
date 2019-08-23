package com.oldpeoplehome.entity;

import java.sql.Date;
import java.sql.Time;

/**
 * Created By Jiangyuwei on 2019/8/22 16:51
 * Description:
 */
public class Sleep {

    private long parentId;
    private Date date;
    private Time lightTime;
    private Time deepTime;
    private Time awakeTime;

    public Sleep() {
    }

    public Sleep(long parentId, String date, String lightTime, String deepTime, String awakeTime) {
        this.parentId = parentId;
        this.date = Date.valueOf(date);
        this.lightTime = Time.valueOf(lightTime);
        this.deepTime = Time.valueOf(deepTime);
        this.awakeTime = Time.valueOf(awakeTime);
    }

    public Sleep(long parentId, Date date, Time lightTime, Time deepTime, Time awakeTime) {
        this.parentId = parentId;
        this.date = date;
        this.lightTime = lightTime;
        this.deepTime = deepTime;
        this.awakeTime = awakeTime;
    }

    public Date getDate() {
        return date;
    }

    public void setDate(Date date) {
        this.date = date;
    }

    public void setDate(String date) {
        this.date = Date.valueOf(date);
    }

    public Time getLightTime() {
        return lightTime;
    }

    public void setLightTime(Time lightTime) {
        this.lightTime = lightTime;
    }

    public void setLightTime(String lightTime) {
        this.lightTime = Time.valueOf(lightTime);
    }

    public Time getDeepTime() {
        return deepTime;
    }

    public void setDeepTime(Time deepTime) {
        this.deepTime = deepTime;
    }

    public void setDeepTime(String deepTime) {
        this.deepTime = Time.valueOf(deepTime);
    }

    public Time getAwakeTime() {
        return awakeTime;
    }

    public void setAwakeTime(Time awakeTime) {
        this.awakeTime = awakeTime;
    }

    public void setAwakeTime(String awakeTime) {
        this.awakeTime = Time.valueOf(awakeTime);
    }

    public long getParentId() {
        return parentId;
    }

    public void setParentId(long parentId) {
        this.parentId = parentId;
    }

    @Override
    public String toString() {
        return "Sleep{" +
                "parentId=" + parentId +
                ", date=" + date +
                ", lightTime=" + lightTime +
                ", deepTime=" + deepTime +
                ", awakeTime=" + awakeTime +
                '}';
    }
}
