package com.scorpiomiku.oldpeoplehome.bean;

/**
 * Created by ScorpioMiku on 2019/9/5.
 */

public class RoomState {
    private String roomId;
    private String time;
    private String temperature;
    private String humidity;
    private String isin;

    @Override
    public String toString() {
        return "RoomState{" +
                "roomId='" + roomId + '\'' +
                ", time='" + time + '\'' +
                ", temperature='" + temperature + '\'' +
                ", humidity='" + humidity + '\'' +
                ", isin='" + isin + '\'' +
                '}';
    }

    public String getRoomId() {
        return roomId;
    }

    public void setRoomId(String roomId) {
        this.roomId = roomId;
    }

    public String getTime() {
        return time;
    }

    public void setTime(String time) {
        this.time = time;
    }

    public String getTemperature() {
        return temperature;
    }

    public void setTemperature(String temperature) {
        this.temperature = temperature;
    }

    public String getHumidity() {
        return humidity;
    }

    public void setHumidity(String humidity) {
        this.humidity = humidity;
    }

    public String getIsin() {
        return isin;
    }

    public void setIsin(String isin) {
        this.isin = isin;
    }
}
