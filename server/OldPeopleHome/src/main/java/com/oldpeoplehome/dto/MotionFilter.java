package com.oldpeoplehome.dto;

import java.sql.Date;

/**
 * Created By Jiangyuwei on 2019/8/21 17:13
 * Description:
 */
public class MotionFilter {
    private Date startDate;
    private Date endDate;
    private long motionParentId;

    public MotionFilter() {
    }

    public MotionFilter(long motionParentId) {
        this.motionParentId = motionParentId;
    }

    public MotionFilter(Date startDate, long motionParentId) {
        this.startDate = startDate;
        this.motionParentId = motionParentId;
    }

    public MotionFilter(Date startDate, Date endDate, long motionParentId) {
        this.startDate = startDate;
        this.endDate = endDate;
        this.motionParentId = motionParentId;
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

    public long getMotionParentId() {
        return motionParentId;
    }

    public void setMotionParentId(long motionParentId) {
        this.motionParentId = motionParentId;
    }

    @Override
    public String toString() {
        return "MotionFilter{" +
                "startDate=" + startDate +
                ", endDate=" + endDate +
                ", parentId=" + motionParentId +
                '}';
    }
}
