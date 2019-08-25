package com.scorpiomiku.oldpeoplehome.bean;

public class BleDeviceItem {
    private String bleDeviceName;
    private String bleDeviceAddress;
    private String nickname;
    private String bindedDate;
    private int rssi;
    private String type;

    public BleDeviceItem() {
    }

    public BleDeviceItem(String deviceName, String deviceAddress, String nickname, String bindedDate, int rssi, String type) {
        setBleDeviceName(deviceName);
        setBleDeviceAddress(deviceAddress);
        setNickname(nickname);
        setBindedDate(bindedDate);
        setRssi(rssi);
        setType(type);
    }

    public String getBleDeviceName() {
        return bleDeviceName;
    }

    public void setBleDeviceName(String bleDeviceName) {
        this.bleDeviceName = bleDeviceName;
    }

    public String getBleDeviceAddress() {
        return bleDeviceAddress;
    }

    public void setBleDeviceAddress(String bleDeviceAddress) {
        this.bleDeviceAddress = bleDeviceAddress;
    }

    public int getRssi() {
        return rssi;
    }

    public void setRssi(int rssi) {
        this.rssi = rssi;
    }

    public String getNickname() {
        return nickname;
    }

    public void setNickname(String nickname) {
        this.nickname = nickname;
    }

    public String getBindedDate() {
        return bindedDate;
    }

    public void setBindedDate(String bindedDate) {
        this.bindedDate = bindedDate;
    }

    public String getType() {
        return type;
    }

    public void setType(String type) {
        this.type = type;
    }

    @Override
    public String toString() {
        return "BleDeviceItem{" +
                "bleDeviceName='" + bleDeviceName + '\'' +
                ", bleDeviceAddress='" + bleDeviceAddress + '\'' +
                ", nickname='" + nickname + '\'' +
                ", bindedDate='" + bindedDate + '\'' +
                ", rssi=" + rssi +
                ", type='" + type + '\'' +
                '}';
    }
}
