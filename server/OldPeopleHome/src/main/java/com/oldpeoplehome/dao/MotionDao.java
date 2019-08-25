package com.oldpeoplehome.dao;

import com.oldpeoplehome.dto.MotionFilter;
import com.oldpeoplehome.entity.Motion;

import java.util.List;

/**
 * Created By Jiangyuwei on 2019/8/21 15:58
 * Description:
 */
public interface MotionDao {

    List<Motion> findByParentId(MotionFilter motionFilter);
    List<Motion> findByParentIdAndDate(MotionFilter motionFilter);
    List<Motion> findByDate(MotionFilter motionFilter);


    void insert(Motion motion);

}
