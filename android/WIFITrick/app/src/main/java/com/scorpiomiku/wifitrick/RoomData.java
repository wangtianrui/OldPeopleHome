package com.scorpiomiku.wifitrick;

import java.util.HashMap;

/**
 * Created by ScorpioMiku on 2019/9/3.
 */

public class RoomData {
    private String isin;
    private String temporature;
    private String humidity;
    private String time;
    private String roomId;

    public RoomData(String isin, String temporature, String humidity, String time, String roomId) {
        this.isin = isin;
        this.temporature = temporature;
        this.humidity = humidity;
        this.time = time;
        this.roomId = roomId;
    }

    public HashMap<String, String> getMap() {
        HashMap<String, String> mapData = new HashMap<>();
        mapData.put("roomId", getRoomId());
        mapData.put("time", TimeUtils.getTime());
        mapData.put("temperature", getTemporature());
        mapData.put("humidity", getHumidity());
        mapData.put("isin", getIsin());
        return mapData;
    }

    public String getIsin() {
        return isin;
    }

    public void setIsin(String isin) {
        this.isin = isin;
    }

    public String getTemporature() {
        return temporature;
    }

    public void setTemporature(String temporature) {
        this.temporature = temporature;
    }

    public String getHumidity() {
        return humidity;
    }

    public void setHumidity(String humidity) {
        this.humidity = humidity;
    }

    public String getTime() {
        return time;
    }

    public void setTime(String time) {
        this.time = time;
    }

    public String getRoomId() {
        return roomId;
    }

    public void setRoomId(String roomId) {
        this.roomId = roomId;
    }

    @Override
    public String toString() {
        return "RoomData{" +
                "isin='" + isin + '\'' +
                ", temporature='" + temporature + '\'' +
                ", humidity='" + humidity + '\'' +
                ", time='" + time + '\'' +
                ", roomId='" + roomId + '\'' +
                '}';
    }
}
