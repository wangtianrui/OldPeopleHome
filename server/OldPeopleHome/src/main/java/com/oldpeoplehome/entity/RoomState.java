package com.oldpeoplehome.entity;

import com.fasterxml.jackson.annotation.JsonFormat;

import java.sql.Time;
import java.sql.Timestamp;

/**
 * Created By Jiangyuwei on 2019/8/22 17:22
 * Description:
 */
public class RoomState {

    private int roomId;
    private Timestamp time;
    private double temperature;
    private double humidity;
    private int isin;

    public RoomState() {
    }

    public RoomState(int rid, String time, double temperature, double humidity, int isin) {
        this.roomId = rid;
        this.time = Timestamp.valueOf(time);
        this.temperature = temperature;
        this.humidity = humidity;
        this.isin = isin;
    }
    public RoomState(int rid, Timestamp time, double temperature, double humidity, int isin) {
        this.roomId = rid;
        this.time = time;
        this.temperature = temperature;
        this.humidity = humidity;
        this.isin = isin;
    }

    public int getRoomId() {
        return roomId;
    }

    public void setRoomId(int roomId) {
        this.roomId = roomId;
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

    public double getTemperature() {
        return temperature;
    }

    public void setTemperature(double temperature) {
        this.temperature = temperature;
    }

    public double getHumidity() {
        return humidity;
    }

    public void setHumidity(double humidity) {
        this.humidity = humidity;
    }

    public int getIsin() {
        return isin;
    }

    public void setIsin(int isin) {
        this.isin = isin;
    }

    @Override
    public String toString() {
        return "RoomState{" +
                "rid=" + roomId +
                ", time=" + time +
                ", temperature=" + temperature +
                ", humidity=" + humidity +
                ", isin=" + isin +
                '}';
    }
}
