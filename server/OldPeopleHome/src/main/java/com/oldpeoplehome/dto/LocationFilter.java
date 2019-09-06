package com.oldpeoplehome.dto;

import java.sql.Date;
import java.sql.Timestamp;

/**
 * Created By Jiangyuwei on 2019/9/6 11:27
 * Description:
 */
public class LocationFilter {

    private Timestamp startTime;
    private Timestamp endTime;
    private long parentId;

    public LocationFilter() {
    }

    public LocationFilter(long parentId) {
        this.parentId = parentId;
    }

    public LocationFilter(String startTime, long parentId) {
        this.startTime = Timestamp.valueOf(startTime);
        this.parentId = parentId;
    }

    public LocationFilter(String startTime, String endTime, long parentId) {
        this.startTime = Timestamp.valueOf(startTime);
        this.endTime = Timestamp.valueOf(endTime);
        this.parentId = parentId;
    }

    public Timestamp getStartTime() {
        return startTime;
    }

    public void setStartTime(String startTime) {
        this.startTime = Timestamp.valueOf(startTime);
    }
    public void setStartTime(Timestamp startTime) {
        this.startTime = startTime;
    }

    public Timestamp getEndTime() {
        return endTime;
    }

    public void setEndTime(Timestamp endTime) {
        this.endTime = endTime;
    }
    public void setEndTime(String endTime) {
        this.endTime = Timestamp.valueOf(endTime);
    }

    public long getParentId() {
        return parentId;
    }

    public void setParentId(long parentId) {
        this.parentId = parentId;
    }

}
