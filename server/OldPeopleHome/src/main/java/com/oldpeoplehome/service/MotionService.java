package com.oldpeoplehome.service;

import com.oldpeoplehome.dto.MotionFilter;
import com.oldpeoplehome.entity.Motion;

import java.util.List;

/**
 * Created By Jiangyuwei on 2019/8/22 13:52
 * Description:
 */
public interface MotionService {

    List<Motion> findByParentId(MotionFilter motionFilter);
    List<Motion> findByParentIdAndDate(MotionFilter motionFilter);
    List<Motion> findByDate(MotionFilter motionFilter);
    void insert(Motion motion);

}
