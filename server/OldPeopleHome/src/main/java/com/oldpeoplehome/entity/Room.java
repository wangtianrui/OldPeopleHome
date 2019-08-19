package com.oldpeoplehome.entity;

/**
 * Created By Jiangyuwei on 2019/8/19 14:16
 * Description:
 */
public class Room {

    private int roomId;
    private String roomLocation;
    private String roomName;

    public Room() {
    }

    public Room(int roomId, String roomLocation, String roomName) {
        this.roomId = roomId;
        this.roomName = roomName;
        this.roomLocation = roomLocation;
    }

    public int getRoomId() {
        return roomId;
    }

    public void setRoomId(int roomId) {
        this.roomId = roomId;
    }

    public String getRoomName() {
        return roomName;
    }

    public void setRoomName(String roomName) {
        this.roomName = roomName;
    }

    public String getRoomLocation() {
        return roomLocation;
    }

    public void setRoomLocation(String roomLocation) {
        this.roomLocation = roomLocation;
    }

    @Override
    public String toString() {
        return "Room{" +
                "roomId=" + roomId +
                ", roomName='" + roomName + '\'' +
                ", roomLocation='" + roomLocation + '\'' +
                '}';
    }
}
