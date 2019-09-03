package com.scorpiomiku.wifitrick;

/**
 * Created by ScorpioMiku on 2019/9/3.
 */

public class Node {
    private String nwkAddr;
    private String parAddr;
    private String macAddr;
    private funcList[] funcList;

    public com.scorpiomiku.wifitrick.funcList[] getFuncList() {
        return funcList;
    }

    public void setFuncList(com.scorpiomiku.wifitrick.funcList[] funcList) {
        this.funcList = funcList;
    }

    @Override
    public String toString() {
        return "Node{" +
                "nwkAddr='" + nwkAddr + '\'' +
                ", parAddr='" + parAddr + '\'' +
                ", macAddr='" + macAddr + '\'' +
                ", funcList=" + funcList +
                '}';
    }

    public String getNwkAddr() {
        return nwkAddr;
    }

    public void setNwkAddr(String nwkAddr) {
        this.nwkAddr = nwkAddr;
    }

    public String getParAddr() {
        return parAddr;
    }

    public void setParAddr(String parAddr) {
        this.parAddr = parAddr;
    }

    public String getMacAddr() {
        return macAddr;
    }

    public void setMacAddr(String macAddr) {
        this.macAddr = macAddr;
    }


}
