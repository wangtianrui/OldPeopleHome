package com.oldpeoplehome.dao;

import com.oldpeoplehome.dto.HeartRateFilter;
import com.oldpeoplehome.dto.RoomStateFilter;
import com.oldpeoplehome.entity.HeartRate;
import com.oldpeoplehome.entity.RoomState;

import java.util.List;

/**
 * Created By Jiangyuwei on 2019/8/22 21:44
 * Description:
 */
public interface HeartRateDao {

    List<HeartRate> findByParent(HeartRateFilter rateFilter);
    List<HeartRate> findByTime(HeartRateFilter rateFilter);
    List<HeartRate> findByParentAndTime(HeartRateFilter rateFilter);

    void insert(HeartRate heartRate);

}
