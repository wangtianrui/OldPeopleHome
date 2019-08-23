package com.oldpeoplehome.service;

import com.oldpeoplehome.dto.HeartRateFilter;
import com.oldpeoplehome.entity.HeartRate;

import java.util.List;

/**
 * Created By Jiangyuwei on 2019/8/22 21:55
 * Description:
 */
public interface HeartRateService {

    List<HeartRate> findByParent(HeartRateFilter rateFilter);
    List<HeartRate> findByTime(HeartRateFilter rateFilter);
    List<HeartRate> findByParentAndTime(HeartRateFilter rateFilter);

    void insert(HeartRate heartRate);

}
