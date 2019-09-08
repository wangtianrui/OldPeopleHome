package com.oldpeoplehome.entity;

/**
 * Created By Jiangyuwei on 2019/8/19 14:16
 * Description:
 */
public class Room {

    private int roomId;
    private String roomLocation;
    private String roomName;
    private long parentId;
    private Parent parent;
    private float temp;

    public Room() {
    }

    public Room(String roomLocation, String roomName) {
        this.roomLocation = roomLocation;
        this.roomName = roomName;

    }
    public Room(String roomLocation, String roomName, float temp) {
        this.roomLocation = roomLocation;
        this.roomName = roomName;
        this.temp = temp;
    }

    public Room(String roomLocation, String roomName, Long parentId, float temp) {
        this.roomLocation = roomLocation;
        this.roomName = roomName;
        this.parentId = parentId;
        this.temp = temp;
    }

    public Room(Integer roomId, String roomLocation, String roomName, Long parentId, float temp) {
        this.roomId = roomId;
        this.roomName = roomName;
        this.roomLocation = roomLocation;
        this.parentId = parentId;
        this.temp = temp;
    }

    public int getRoomId() {
        return roomId;
    }

    public void setRoomId(Integer roomId) {
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

    public long getParentId() {
        return parentId;
    }

    public void setParentId(Long parentId) {
        this.parentId = parentId;
    }

    public float getTemp() {
        return temp;
    }

    public void setTemp(float temp) {
        this.temp = temp;
    }
    public void mySetTemp(String temp) {
        this.temp = Float.valueOf(temp);
    }

    public void mySetRoomName(String roomName){
        this.roomName = roomName;
    }
    public void mySetRoomLocation(String roomLocation) {
        this.roomLocation = roomLocation;
    }

    public void mySetParentId(String parentId) {
        this.parentId = Long.valueOf(parentId);
    }

    @Override
    public String toString() {
        return "Room{" +
                "roomId=" + roomId +
                ", roomLocation='" + roomLocation + '\'' +
                ", roomName='" + roomName + '\'' +
                ", parentId=" + parentId +
                ", parent=" + parent +
                ", temp=" + temp +
                '}';
    }
}
