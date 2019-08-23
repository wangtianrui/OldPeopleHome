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

    public Room() {
    }

    public Room(String roomLocation, String roomName) {
        this.roomLocation = roomLocation;
        this.roomName = roomName;
    }

    public Room(String roomLocation, String roomName, Long parentId) {
        this.roomLocation = roomLocation;
        this.roomName = roomName;
        this.parentId = parentId;
    }

    public Room(Integer roomId, String roomLocation, String roomName, Long parentId) {
        this.roomId = roomId;
        this.roomName = roomName;
        this.roomLocation = roomLocation;
        this.parentId = parentId;
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

    @Override
    public String toString() {
        return "Room{" +
                "roomId=" + roomId +
                ", roomLocation='" + roomLocation + '\'' +
                ", roomName='" + roomName + '\'' +
                ", parentId=" + parentId +
                ", parent=" + parent +
                '}';
    }
}
