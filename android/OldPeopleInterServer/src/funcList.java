import java.util.HashMap;

/**
 * Created by ScorpioMiku on 2019/9/3.
 */

public class funcList {

    private String typeCode;
    private String type;
    private float cycle;
    private float data;

    @Override
    public String toString() {
        return "funcList{" +
                "typeCode='" + typeCode + '\'' +
                ", type='" + type + '\'' +
                ", cycle=" + cycle +
                ", data=" + data +
                '}';
    }

    public String getTypeCode() {
        return typeCode;
    }

    public void setTypeCode(String typeCode) {
        this.typeCode = typeCode;
    }

    public String getType() {
        return type;
    }

    public void setType(String type) {
        this.type = type;
    }

    public float getCycle() {
        return cycle;
    }

    public void setCycle(float cycle) {
        this.cycle = cycle;
    }

    public float getData() {
        return data;
    }

    public void setData(float data) {
        this.data = data;
    }
}
