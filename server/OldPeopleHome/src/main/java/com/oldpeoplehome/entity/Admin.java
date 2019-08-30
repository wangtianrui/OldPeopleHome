package com.oldpeoplehome.entity;

/**
 * Created By Jiangyuwei on 2019/8/30 11:10
 * Description:
 */
public class Admin {

    private String adminAccount;
    private String adminPassword;

    public Admin() {
    }

    public Admin(String adminAccount, String adminPassword) {
        this.adminAccount = adminAccount;
        this.adminPassword = adminPassword;
    }

    public String getAdminAccount() {
        return adminAccount;
    }

    public void setAdminAccount(String adminAccount) {
        this.adminAccount = adminAccount;
    }

    public String getAdminPassword() {
        return adminPassword;
    }

    public void setAdminPassword(String adminPassword) {
        this.adminPassword = adminPassword;
    }

    @Override
    public String toString() {
        return "Admin{" +
                "adminAccount='" + adminAccount + '\'' +
                ", adminPassword='" + adminPassword + '\'' +
                '}';
    }
}
