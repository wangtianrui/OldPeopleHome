package com.oldpeoplehome.dao;

import com.oldpeoplehome.BaseTest;
import com.oldpeoplehome.dto.MotionFilter;
import com.oldpeoplehome.entity.Motion;
import org.junit.Test;
import org.springframework.beans.factory.annotation.Autowired;

import java.sql.Date;
import java.util.List;

/**
 * Created By Jiangyuwei on 2019/8/21 17:30
 * Description:
 */
public class MotionDaoTest extends BaseTest {

    @Autowired
    private MotionDao motionDao;
    @Test
    public void test(){
        MotionFilter motionFilter = new MotionFilter(1);

//        System.out.println(motionDao.findByParentId(motionFilter));
//        motionFilter = new MotionFilter(Date.valueOf("2019-8-21"), 1);
//        System.out.println(motionDao.findByParentIdAndDate(motionFilter));
        motionFilter = new MotionFilter( Date.valueOf("2019-8-21"), Date.valueOf("2019-8-23"), 1);
        System.out.println(motionDao.findByDate(motionFilter));
    }

    @Test
    public void testInsert(){
        Motion motion = new Motion(2, "2019-2-2", 1239, 11, "1:1:1");
        motionDao.insert(motion);
    }

}
