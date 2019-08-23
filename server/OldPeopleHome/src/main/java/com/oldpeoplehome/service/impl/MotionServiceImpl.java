package com.oldpeoplehome.service.impl;

import com.oldpeoplehome.dao.MotionDao;
import com.oldpeoplehome.dto.MotionFilter;
import com.oldpeoplehome.entity.Motion;
import com.oldpeoplehome.service.MotionService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

/**
 * Created By Jiangyuwei on 2019/8/22 14:02
 * Description:
 */
@Service
public class MotionServiceImpl implements MotionService {

    @Autowired
    private MotionDao motionDao;

    @Override
    public List<Motion> findByParentId(MotionFilter motionFilter) {
        return motionDao.findByParentId(motionFilter);
    }

    @Override
    public List<Motion> findByParentIdAndDate(MotionFilter motionFilter) {
        return motionDao.findByParentIdAndDate(motionFilter);
    }

    @Override
    public List<Motion> findByDate(MotionFilter motionFilter) {
        return motionDao.findByDate(motionFilter);
    }

    @Override
    public void insert(Motion motion) {
        motionDao.insert(motion);
    }
}
