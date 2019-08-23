package com.oldpeoplehome.dto;

import java.sql.Date;
import java.sql.Timestamp;

/**
 * Created By Jiangyuwei on 2019/8/22 17:29
 * Description:
 */
public class RoomStateFilter {

    private Timestamp startDate;
    private Timestamp endDate;
    private int roomId;

    public RoomStateFilter(Timestamp startDate, Timestamp endDate, int roomId) {
        this.startDate = startDate;
        this.endDate = endDate;
        this.roomId = roomId;
    }

    public RoomStateFilter(Timestamp startDate, int roomId) {
        this.startDate = startDate;
        this.roomId = roomId;
    }

    public RoomStateFilter() {
    }

    public RoomStateFilter(int roomId) {
        this.roomId = roomId;
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

    public int getRoomId() {
        return roomId;
    }

    public void setRoomId(int roomId) {
        this.roomId = roomId;
    }

    @Override
    public String toString() {
        return "RoomStateFilter{" +
                "startDate=" + startDate +
                ", endDate=" + endDate +
                ", roomId=" + roomId +
                '}';
    }
}
