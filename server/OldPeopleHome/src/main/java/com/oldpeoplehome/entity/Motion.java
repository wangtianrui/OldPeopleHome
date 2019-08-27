package com.oldpeoplehome.entity;

import java.sql.Date;
import java.sql.Time;

/**
 * Created By Jiangyuwei on 2019/8/21 15:58
 * Description:
 */
public class Motion {
    private long motionParentId;
    private Date motionDate;
    private long motionCount;
    private double motionDistance;
    private double motionEnergy;
    private Time motionTime;

    public Motion() {
    }

    public Motion(long motionParentId, String motionDate, long motionCount, double motionDistance, double motionEnergy, String motionTime) {
        this.motionParentId = motionParentId;
        this.motionDate = Date.valueOf(motionDate);
        this.motionCount = motionCount;
        this.motionDistance = motionDistance;
        this.motionEnergy = motionEnergy;
        this.motionTime = Time.valueOf(motionTime);
    }

    public Motion(long motionParentId, Date motionDate, long motionCount, double motionDistance, double motionEnergy, Time motionTime) {
        this.motionParentId = motionParentId;
        this.motionDate = motionDate;
        this.motionCount = motionCount;
        this.motionDistance = motionDistance;
        this.motionEnergy = motionEnergy;
        this.motionTime = motionTime;
    }


    public long getMotionParentId() {
        return motionParentId;
    }

    public void setMotionParentId(long parentId) {
        this.motionParentId = parentId;
    }

    public Date getMotionDate() {
        return motionDate;
    }

    public void setMotionDate(Date motionDate) {
        this.motionDate = motionDate;
    }
    public void setMotionDate(String motionDate) {
        this.motionDate = Date.valueOf(motionDate);
    }

    public long getMotionCount() {
        return motionCount;
    }

    public void setMotionCount(long motionCount) {
        this.motionCount = motionCount;
    }

    public double getMotionDistance() {
        return motionDistance;
    }

    public void setMotionDistance(double motionDistance) {
        this.motionDistance = motionDistance;
    }

    public Time getMotionTime() {
        return motionTime;
    }

    public void setMotionTime(Time motionTime) {
        this.motionTime = motionTime;
    }

    public double getmotionEnergy() {
        return motionEnergy;
    }

    public void setmotionEnergy(double motionEnergy) {
        this.motionEnergy = motionEnergy;
    }

    @Override
    public String toString() {
        return "Motion{" +
                "motionParentId=" + motionParentId +
                ", motionDate=" + motionDate +
                ", motionCount=" + motionCount +
                ", motionDistance=" + motionDistance +
                ", motionEnergy=" + motionEnergy +
                ", motionTime=" + motionTime +
                '}';
    }
}
