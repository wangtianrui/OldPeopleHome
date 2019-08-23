package com.oldpeoplehome.service.impl;

import com.oldpeoplehome.dao.SleepDao;
import com.oldpeoplehome.dto.SleepFilter;
import com.oldpeoplehome.entity.Sleep;
import com.oldpeoplehome.service.SleepService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

/**
 * Created By Jiangyuwei on 2019/8/22 17:04
 * Description:
 */
@Service
public class SleepServiceImpl implements SleepService {
    @Autowired
    private SleepDao sleepDao;

    @Override
    public List<Sleep> findByParentId(SleepFilter sleepFilter) {
        return sleepDao.findByParentId(sleepFilter);
    }

    @Override
    public List<Sleep> findByParentIdAndDate(SleepFilter sleepFilter) {
        return sleepDao.findByParentIdAndDate(sleepFilter);
    }

    @Override
    public List<Sleep> findByDate(SleepFilter sleepFilter) {
        return sleepDao.findByDate(sleepFilter);
    }

    @Override
    public void insert(Sleep Sleep) {
        sleepDao.insert(Sleep);
    }
}
