package com.oldpeoplehome.entity;

import java.sql.Date;

/**
 * Created By Jiangyuwei on 2019/8/19 14:18
 * Description:
 */
public class Parent {

    private long parentId;
    private long parentLongId;
    private String parentName;
    private String parentSex;
    private String parentAccount;
    private String parentPassword;
    private double parentHeight;
    private double parentWeight;
    private Date parentBirth;
    private long parentPhone;
    private int parentRoomId;

    public Parent() {
    }

    public Parent(long parentId, long parentLongId, String parentName, String parentSex, String parentAccount, String parentPassword, double parentHeight, double parentWeight, Date parentBirth, long parentPhone, int parentRoomId) {
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
        this.parentRoomId = parentRoomId;
    }

    public long getParentId() {
        return parentId;
    }

    public void setParentId(long parentId) {
        this.parentId = parentId;
    }

    public long getParentLongId() {
        return parentLongId;
    }

    public void setParentLongId(long parentLongId) {
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

    public void setParentHeight(double parentHeight) {
        this.parentHeight = parentHeight;
    }

    public double getParentWeight() {
        return parentWeight;
    }

    public void setParentWeight(double parentWeight) {
        this.parentWeight = parentWeight;
    }

    public Date getParentBirth() {
        return parentBirth;
    }

    public void setParentBirth(Date parentBirth) {
        this.parentBirth = parentBirth;
    }

    public long getParentPhone() {
        return parentPhone;
    }

    public void setParentPhone(long parentPhone) {
        this.parentPhone = parentPhone;
    }

    public int getParentRoomId() {
        return parentRoomId;
    }

    public void setParentRoomId(int parentRoomId) {
        this.parentRoomId = parentRoomId;
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
                ", parentRoomId=" + parentRoomId +
                '}';
    }
}
