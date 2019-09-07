package com.oldpeoplehome.entity;

import com.fasterxml.jackson.annotation.JsonFormat;

import java.sql.Timestamp;

/**
 * Created By Jiangyuwei on 2019/9/6 11:15
 * Description:
 */
public class Location {
    
    private Long parentId;
    private Float longitude;
    private Float latitude;
    private Timestamp time;

    public Location() {
    }

    public Location(Long parentId, Float longitude, Float latitude, Timestamp time) {
        this.parentId = parentId;
        this.longitude = longitude;
        this.latitude = latitude;
        this.time = time;
    }
    public Location(Long parentId, Float longitude, Float latitude, String time) {
        this.parentId = parentId;
        this.longitude = longitude;
        this.latitude = latitude;
        this.time = Timestamp.valueOf(time);
    }

    public Long getParentId() {
        return parentId;
    }

    public void setParentId(Long parentId) {
        this.parentId = parentId;
    }

    public Float getLongitude() {
        return longitude;
    }

    public void setLongitude(Float longitude) {
        this.longitude = longitude;
    }

    public Float getLatitude() {
        return latitude;
    }

    public void setLatitude(Float latitude) {
        this.latitude = latitude;
    }

    @JsonFormat(pattern="yyyy-MM-dd HH:mm:ss",timezone = "GMT+8")
    public Timestamp getTime() {
        return time;
    }

    public void setTime(Timestamp time) {
        this.time = time;
    }
    
    public void setTime(String time) {
        this.time = Timestamp.valueOf(time);
    }

    @Override
    public String toString() {
        return "Location{" +
                "parentId=" + parentId +
                ", longitude=" + longitude +
                ", latitude=" + latitude +
                ", time=" + time +
                '}';
    }
}
