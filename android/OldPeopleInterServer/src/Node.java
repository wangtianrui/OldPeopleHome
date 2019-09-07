/**
 * Created by ScorpioMiku on 2019/9/3.
 */

public class Node {
    private String nwkAddr;
    private String parAddr;
    private String macAddr;
    private funcList[] funcList;

    public funcList[] getFuncList() {
        return funcList;
    }

    public void setFuncList(funcList[] funcList) {
        this.funcList = funcList;
    }

    @Override
    public String toString() {
        return "Node{" +
                "nwkAddr='" + nwkAddr + '\'' +
                ", parAddr='" + parAddr + '\'' +
                ", macAddr='" + macAddr + '\'' +
                ", funcList=" + funcListString() +
                '}';
    }

    private String funcListString() {
        String temp = "";
        for (int i = 0; i < funcList.length; i++) {
            temp += funcList[i].toString();
        }
        return temp;
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
