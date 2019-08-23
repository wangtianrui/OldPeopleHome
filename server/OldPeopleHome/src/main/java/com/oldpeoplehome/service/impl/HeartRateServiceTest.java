package com.oldpeoplehome.service.impl;

import com.oldpeoplehome.dao.HeartRateDao;
import com.oldpeoplehome.dto.HeartRateFilter;
import com.oldpeoplehome.entity.HeartRate;
import com.oldpeoplehome.service.HeartRateService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

/**
 * Created By Jiangyuwei on 2019/8/22 21:56
 * Description:
 */
@Service
public class HeartRateServiceTest implements HeartRateService {

    @Autowired
    private HeartRateDao heartRateDao;
    @Override
    public List<HeartRate> findByParent(HeartRateFilter rateFilter) {
        return heartRateDao.findByParent(rateFilter);
    }

    @Override
    public List<HeartRate> findByTime(HeartRateFilter rateFilter) {
        return heartRateDao.findByTime(rateFilter);
    }

    @Override
    public List<HeartRate> findByParentAndTime(HeartRateFilter rateFilter) {
        return heartRateDao.findByParentAndTime(rateFilter);
    }

    @Override
    public void insert(HeartRate heartRate) {
        heartRateDao.insert(heartRate);
    }
}
