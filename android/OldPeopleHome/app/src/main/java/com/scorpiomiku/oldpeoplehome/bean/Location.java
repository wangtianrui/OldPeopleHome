package com.scorpiomiku.oldpeoplehome.bean;

import android.support.annotation.NonNull;

/**
 * Created by ScorpioMiku on 2019/9/6.
 */

public class Location {
    private String parent;
    private String longitude;
    private String latitude;
    private String time;

    @Override
    public String toString() {
        return "Location{" +
                "parent='" + parent + '\'' +
                ", longitude='" + longitude + '\'' +
                ", latitude='" + latitude + '\'' +
                ", time='" + time + '\'' +
                '}';
    }

    public String getParent() {
        return parent;
    }

    public void setParent(String parent) {
        this.parent = parent;
    }

    public String getLongitude() {
        return longitude;
    }

    public void setLongitude(String longitude) {
        this.longitude = longitude;
    }

    public String getLatitude() {
        return latitude;
    }

    public void setLatitude(String latitude) {
        this.latitude = latitude;
    }

    public String getTime() {
        return time;
    }

    public void setTime(String time) {
        this.time = time;
    }


}
