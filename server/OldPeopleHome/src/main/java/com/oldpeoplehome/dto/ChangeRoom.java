package com.oldpeoplehome.dto;

import java.sql.Timestamp;

/**
 * Created By Jiangyuwei on 2019/8/21 9:44
 * Description:
 */
public class ChangeRoom {

    private Timestamp startDate;
    private Timestamp endDate;
    private long parentId;

    public ChangeRoom() {
    }

    public ChangeRoom(long parentId) {
        this.parentId = parentId;
    }

    public ChangeRoom(Timestamp startDate, long parentId) {
        this.startDate = startDate;
        this.parentId = parentId;
    }

    public ChangeRoom(Timestamp startDate, Timestamp endDate, long parentId) {
        this.startDate = startDate;
        this.endDate = endDate;
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

    public long getParentId() {
        return parentId;
    }

    public void setParentId(long parentId) {
        this.parentId = parentId;
    }

    @Override
    public String toString() {
        return "ChangeRoom{" +
                "startDate=" + startDate +
                ", endDate=" + endDate +
                ", parentId=" + parentId +
                '}';
    }
}
