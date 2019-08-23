package com.oldpeoplehome.dao;

import com.oldpeoplehome.dto.MotionFilter;
import com.oldpeoplehome.dto.SleepFilter;
import com.oldpeoplehome.entity.Motion;
import com.oldpeoplehome.entity.Sleep;

import java.util.List;

/**
 * Created By Jiangyuwei on 2019/8/22 16:55
 * Description:
 */
public interface SleepDao {

    List<Sleep> findByParentId(SleepFilter sleepFilter);
    List<Sleep> findByParentIdAndDate(SleepFilter sleepFilter);
    List<Sleep> findByDate(SleepFilter sleepFilter);

    void insert(Sleep Sleep);

}
