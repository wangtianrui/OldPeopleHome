package com.oldpeoplehome.entity;

/**
 * Created By Jiangyuwei on 2019/8/21 10:18
 * Description:
 */
public class Child {

    private long childId;
    private String childLongId;
    private String childName;
    private String childSex;
    private String childAccount;
    private String childPassword;
    private String childPhone;
    private Parent parent;

    public Child() {
    }

    public Child(String childLongId, String childName, String childSex, String childAccount, String childPassword, String childPhone) {
        this.childLongId = childLongId;
        this.childName = childName;
        this.childSex = childSex;
        this.childAccount = childAccount;
        this.childPassword = childPassword;
        this.childPhone = childPhone;
    }

    public Child(Long childId, String childLongId, String childName, String childSex, String childAccount, String childPassword, String childPhone) {
        this.childId = childId;
        this.childLongId = childLongId;
        this.childName = childName;
        this.childSex = childSex;
        this.childAccount = childAccount;
        this.childPassword = childPassword;
        this.childPhone = childPhone;
    }

    public long getChildId() {
        return childId;
    }

    public void setChildId(long childId) {
        this.childId = childId;
    }

    public String getChildLongId() {
        return childLongId;
    }

    public void setChildLongId(String childLongId) {
        this.childLongId = childLongId;
    }
    public void mySetChildLongId(String childLongId) {
        this.childLongId = childLongId;
    }

    public String getChildName() {
        return childName;
    }

    public void setChildName(String childName) {
        this.childName = childName;
    }
    public void mySetChildName(String childName) {
        this.childName = childName;
    }

    public String getChildSex() {
        return childSex;
    }

    public void setChildSex(String childSex) {
        this.childSex = childSex;
    }
    public void mySetChildSex(String childSex) {
        this.childSex = childSex;
    }

    public String getChildAccount() {
        return childAccount;
    }

    public void setChildAccount(String childAccount) {
        this.childAccount = childAccount;
    }
    public void mySetChildAccount(String childAccount) {
        this.childAccount = childAccount;
    }

    public String getChildPassword() {
        return childPassword;
    }

    public void setChildPassword(String childPassword) {
        this.childPassword = childPassword;
    }
    public void mySetChildPassword(String childPassword) {
        this.childPassword = childPassword;
    }

    public String getChildPhone() {
        return childPhone;
    }

    public void setChildPhone(String childPhone) {
        this.childPhone = childPhone;
    }
    public void mySetChildPhone(String childPhone) {
        this.childPhone = childPhone;
    }

    @Override
    public String toString() {
        return "Child{" +
                "childId=" + childId +
                ", childLongId='" + childLongId + '\'' +
                ", childName='" + childName + '\'' +
                ", childSex='" + childSex + '\'' +
                ", childAccount='" + childAccount + '\'' +
                ", childPassword='" + childPassword + '\'' +
                ", childPhone='" + childPhone + '\'' +
                '}';
    }
}
