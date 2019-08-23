package com.oldpeoplehome.service;

import com.oldpeoplehome.dto.MotionFilter;
import com.oldpeoplehome.dto.SleepFilter;
import com.oldpeoplehome.entity.Motion;
import com.oldpeoplehome.entity.Sleep;

import java.util.List;

/**
 * Created By Jiangyuwei on 2019/8/22 17:03
 * Description:
 */
public interface SleepService {
    List<Sleep> findByParentId(SleepFilter sleepFilter);
    List<Sleep> findByParentIdAndDate(SleepFilter sleepFilter);
    List<Sleep> findByDate(SleepFilter sleepFilter);

    void insert(Sleep Sleep);
}
