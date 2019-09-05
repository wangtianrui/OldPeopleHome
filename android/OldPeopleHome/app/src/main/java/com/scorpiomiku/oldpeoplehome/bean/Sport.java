package com.scorpiomiku.oldpeoplehome.bean;

/**
 * Created by ScorpioMiku on 2019/9/5.
 */

public class Sport {
    private String motionParentId;
    private String motionDate;
    private String motionCount;
    private String motionDistance;
    private String motionEnergy;
    private String motionTime;

    @Override
    public String toString() {
        return "Sport{" +
                "motionParentId='" + motionParentId + '\'' +
                ", motionDate='" + motionDate + '\'' +
                ", motionCount='" + motionCount + '\'' +
                ", motionDistance='" + motionDistance + '\'' +
                ", motionEnergy='" + motionEnergy + '\'' +
                ", motionTime='" + motionTime + '\'' +
                '}';
    }

    public String getMotionParentId() {
        return motionParentId;
    }

    public void setMotionParentId(String motionParentId) {
        this.motionParentId = motionParentId;
    }

    public String getMotionDate() {
        return motionDate;
    }

    public void setMotionDate(String motionDate) {
        this.motionDate = motionDate;
    }

    public String getMotionCount() {
        return motionCount;
    }

    public void setMotionCount(String motionCount) {
        this.motionCount = motionCount;
    }

    public String getMotionDistance() {
        return motionDistance;
    }

    public void setMotionDistance(String motionDistance) {
        this.motionDistance = motionDistance;
    }

    public String getMotionEnergy() {
        return motionEnergy;
    }

    public void setMotionEnergy(String motionEnergy) {
        this.motionEnergy = motionEnergy;
    }

    public String getMotionTime() {
        return motionTime;
    }

    public void setMotionTime(String motionTime) {
        this.motionTime = motionTime;
    }
}
