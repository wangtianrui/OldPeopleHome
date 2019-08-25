package com.oldpeoplehome.dto;

import java.sql.Timestamp;

/**
 * Created By Jiangyuwei on 2019/8/22 21:45
 * Description:
 */
public class HeartRateFilter {

    private long parentId;
    private Timestamp startDate;
    private Timestamp endDate;

    public HeartRateFilter() {
    }

    public HeartRateFilter(long parentId) {
        this.parentId = parentId;
    }

    public HeartRateFilter(Timestamp startDate,long parentId) {
        this.parentId = parentId;
        this.startDate = startDate;
    }

    public HeartRateFilter( String startDate,long parentId) {
        this.parentId = parentId;
        this.startDate = Timestamp.valueOf(startDate);
    }
    public HeartRateFilter(String startDate, String endDate,long parentId) {
        this.parentId = parentId;
        this.startDate = Timestamp.valueOf(startDate);
        this.endDate = Timestamp.valueOf(endDate);
    }
    public HeartRateFilter(Timestamp startDate, Timestamp endDate,long parentId) {
        this.parentId = parentId;
        this.startDate = startDate;
        this.endDate = endDate;
    }

    public long getParentId() {
        return parentId;
    }

    public void setParentId(long parentId) {
        this.parentId = parentId;
    }

    public Timestamp getStartDate() {
        return startDate;
    }

    public void setStartDate(Timestamp startDate) {
        this.startDate = startDate;
    }

    public Timestamp getEndDate() {
        return endDate;
    }

    public void setEndDate(Timestamp endDate) {
        this.endDate = endDate;
    }

    @Override
    public String toString() {
        return "HeartRateFilter{" +
                "parentId=" + parentId +
                ", startDate=" + startDate +
                ", endDate=" + endDate +
                '}';
    }
}
