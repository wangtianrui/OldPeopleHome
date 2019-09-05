package com.scorpiomiku.oldpeoplehome.bean;

/**
 * Created by ScorpioMiku on 2019/9/4.
 */

public class OldPeople implements User {
    private String parentId;
    private String parentLongId;
    private String parentName;
    private String parentSex;
    private String parentAccount;
    private String parentPassword;
    private String parentHeight;
    private String parentWeight;
    private String parentBirth;
    private String parentPhone;
    private String parentRoomId;

    @Override
    public String toString() {
        return "OldPeople{" +
                "parentId='" + parentId + '\'' +
                ", parentLongId='" + parentLongId + '\'' +
                ", parentName='" + parentName + '\'' +
                ", parentSex='" + parentSex + '\'' +
                ", parentAccount='" + parentAccount + '\'' +
                ", parentPassword='" + parentPassword + '\'' +
                ", parentHeight='" + parentHeight + '\'' +
                ", parentWeight='" + parentWeight + '\'' +
                ", parentBirth='" + parentBirth + '\'' +
                ", parentPhone='" + parentPhone + '\'' +
                ", parentRoomId='" + parentRoomId + '\'' +
                '}';
    }

    public String getParentId() {
        return parentId;
    }

    public void setParentId(String parentId) {
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

    public String getParentHeight() {
        return parentHeight;
    }

    public void setParentHeight(String parentHeight) {
        this.parentHeight = parentHeight;
    }

    public String getParentWeight() {
        return parentWeight;
    }

    public void setParentWeight(String parentWeight) {
        this.parentWeight = parentWeight;
    }

    public String getParentBirth() {
        return parentBirth;
    }

    public void setParentBirth(String parentBirth) {
        this.parentBirth = parentBirth;
    }

    public String getParentPhone() {
        return parentPhone;
    }

    public void setParentPhone(String parentPhone) {
        this.parentPhone = parentPhone;
    }

    public String getParentRoomId() {
        return parentRoomId;
    }

    public void setParentRoomId(String parentRoomId) {
        this.parentRoomId = parentRoomId;
    }
}
