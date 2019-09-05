package com.scorpiomiku.oldpeoplehome.bean;

import java.io.Serializable;

/**
 * Created by ScorpioMiku on 2019/9/4.
 */

public class Child implements User, Serializable {
    private String childSex;
    private String childPassword;
    private String childAccount;
    private String childName;
    private String childLongId;
    private String childPhone;
    private String childId;

    public String getChildId() {
        return childId;
    }

    public void setChildId(String childId) {
        this.childId = childId;
    }

    public String getChildSex() {
        return childSex;
    }

    public void setChildSex(String childSex) {
        this.childSex = childSex;
    }

    public String getChildPassword() {
        return childPassword;
    }

    public void setChildPassword(String childPassword) {
        this.childPassword = childPassword;
    }

    public String getChildAccount() {
        return childAccount;
    }

    public void setChildAccount(String childAccount) {
        this.childAccount = childAccount;
    }

    public String getChildName() {
        return childName;
    }

    public void setChildName(String childName) {
        this.childName = childName;
    }

    public String getChildLongId() {
        return childLongId;
    }

    public void setChildLongId(String childLongId) {
        this.childLongId = childLongId;
    }

    public String getChildPhone() {
        return childPhone;
    }

    public void setChildPhone(String childPhone) {
        this.childPhone = childPhone;
    }

    @Override
    public String toString() {
        return "Child{" +
                "childSex='" + childSex + '\'' +
                ", childPassword='" + childPassword + '\'' +
                ", childAccount='" + childAccount + '\'' +
                ", childName='" + childName + '\'' +
                ", childLongId='" + childLongId + '\'' +
                ", childPhone='" + childPhone + '\'' +
                '}';
    }
}
