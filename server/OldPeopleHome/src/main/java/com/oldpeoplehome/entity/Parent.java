package com.oldpeoplehome.entity;

import java.sql.Date;

/**
 * Created By Jiangyuwei on 2019/8/19 14:18
 * Description:
 */
public class Parent {

    private long parentId;
    private String parentLongId;
    private String parentName;
    private String parentSex;
    private String parentAccount;
    private String parentPassword;
    private double parentHeight;
    private double parentWeight;
    private Date parentBirth;
    private String parentPhone;
    private int parentRoomId;
    private Room parentRoom;
    private Child child;

    public Parent() {
    }

    public Parent(long parentId, String parentLongId, String parentName, String parentSex, String parentAccount, String parentPassword, double parentHeight, double parentWeight, Date parentBirth, String parentPhone) {
        this.parentId = parentId;
        this.parentLongId = parentLongId;
        this.parentName = parentName;
        this.parentSex = parentSex;
        this.parentAccount = parentAccount;
        this.parentPassword = parentPassword;
        this.parentHeight = parentHeight;
        this.parentWeight = parentWeight;
        this.parentBirth = parentBirth;
        this.parentPhone = parentPhone;
    }

    public Parent(String parentLongId, String parentName, String parentSex, String parentAccount, String parentPassword, Double parentHeight, Double parentWeight, Date parentBirth, String parentPhone) {
        this.parentLongId = parentLongId;
        this.parentName = parentName;
        this.parentSex = parentSex;
        this.parentAccount = parentAccount;
        this.parentPassword = parentPassword;
        this.parentHeight = parentHeight;
        this.parentWeight = parentWeight;
        this.parentBirth = parentBirth;
        this.parentPhone = parentPhone;
    }
    public Parent(Long parentId, String parentLongId, String parentName, String parentSex, String parentAccount, String parentPassword, Double parentHeight, Double parentWeight, Date parentBirth, String parentPhone) {
        this.parentId = parentId;
        this.parentLongId = parentLongId;
        this.parentName = parentName;
        this.parentSex = parentSex;
        this.parentAccount = parentAccount;
        this.parentPassword = parentPassword;
        this.parentHeight = parentHeight;
        this.parentWeight = parentWeight;
        this.parentBirth = parentBirth;
        this.parentPhone = parentPhone;
    }

    public long getParentId() {
        return parentId;
    }

    public void setParentId(Long parentId) {
        this.parentId = parentId;
    }

    public String getParentLongId() {
        return parentLongId;
    }

    public void setParentLongId(String parentLongId) {
        this.parentLongId = parentLongId;
    }

    public String getParentName() {
        return parentName;
    }

    public void setParentName(String parentName) {
        this.parentName = parentName;
    }

    public String getParentSex() {
        return parentSex;
    }

    public void setParentSex(String parentSex) {
        this.parentSex = parentSex;
    }

    public String getParentAccount() {
        return parentAccount;
    }

    public void setParentAccount(String parentAccount) {
        this.parentAccount = parentAccount;
    }

    public String getParentPassword() {
        return parentPassword;
    }

    public void setParentPassword(String parentPassword) {
        this.parentPassword = parentPassword;
    }

    public double getParentHeight() {
        return parentHeight;
    }

    public void setParentHeight(Double parentHeight) {
        this.parentHeight = parentHeight;
    }

    public double getParentWeight() {
        return parentWeight;
    }

    public void setParentWeight(Double parentWeight) {
        this.parentWeight = parentWeight;
    }

    public Date getParentBirth() {
        return parentBirth;
    }

    public void setParentBirth(Date parentBirth) {
        this.parentBirth = parentBirth;
    }
    public void setParentBirth(String parentBirth) {
        this.parentBirth = Date.valueOf(parentBirth);
    }

    public String getParentPhone() {
        return parentPhone;
    }

    public void setParentPhone(String parentPhone) {
        this.parentPhone = parentPhone;
    }

    public int getParentRoomId() {
        return parentRoomId;
    }

    public void setParentRoomId(Integer parentRoomId) {
        this.parentRoomId = parentRoomId;
    }

    public Room getParentRoom() {
        return parentRoom;
    }

    public void setParentRoom(Room parentRoom) {
        this.parentRoom = parentRoom;
    }

    @Override
    public String toString() {
        return "Parent{" +
                "parentId=" + parentId +
                ", parentLongId=" + parentLongId +
                ", parentName='" + parentName + '\'' +
                ", parentSex='" + parentSex + '\'' +
                ", parentAccount='" + parentAccount + '\'' +
                ", parentPassword='" + parentPassword + '\'' +
                ", parentHeight=" + parentHeight +
                ", parentWeight=" + parentWeight +
                ", parentBirth=" + parentBirth +
                ", parentPhone=" + parentPhone +
                '}';
    }
}
