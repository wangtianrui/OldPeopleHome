package com.oldpeoplehome.dto;

import java.sql.Date;

/**
 * Created By Jiangyuwei on 2019/8/22 16:56
 * Description:
 */
public class SleepFilter {

    private Date startDate;
    private Date endDate;
    private long parentId;

    public SleepFilter(long motionParentId) {
        this.parentId = motionParentId;
    }

    public SleepFilter(Date startDate, long motionParentId) {
        this.startDate = startDate;
        this.parentId = motionParentId;
    }

    public SleepFilter(Date startDate, Date endDate, long motionParentId) {
        this.startDate = startDate;
        this.endDate = endDate;
        this.parentId = motionParentId;
    }

    public Date getStartDate() {
        return startDate;
    }

    public void setStartDate(Date startDate) {
        this.startDate = startDate;
    }

    public void setStartDate(String startDate) {
        this.startDate = Date.valueOf(startDate);
    }

    public Date getEndDate() {
        return endDate;
    }

    public void setEndDate(Date endDate) {
        this.endDate = endDate;
    }

    public void setEndDate(String endDate) {
        this.endDate = Date.valueOf(endDate);
    }

    public long getParentId() {
        return parentId;
    }

    public void setParentId(long parentId) {
        this.parentId = parentId;
    }

    @Override
    public String toString() {
        return "SleepFilter{" +
                "startDate=" + startDate +
                ", endDate=" + endDate +
                ", parentId=" + parentId +
                '}';
    }
}
