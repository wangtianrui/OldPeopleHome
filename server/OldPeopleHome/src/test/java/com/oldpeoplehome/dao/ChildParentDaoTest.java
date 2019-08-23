package com.oldpeoplehome.dao;

import com.oldpeoplehome.BaseTest;
import com.oldpeoplehome.entity.ChildParent;
import org.junit.Test;
import org.springframework.beans.factory.annotation.Autowired;

/**
 * Created By Jiangyuwei on 2019/8/21 14:33
 * Description:
 */
public class ChildParentDaoTest extends BaseTest {

    @Autowired
    private ChildParentDao childParentDao;

    @Test
    public void test(){
        System.out.println(childParentDao.findByChild(1));
        System.out.println(childParentDao.findByParent(1));
    }

    @Test
    public void testInsert(){
        ChildParent childParent = new ChildParent(3,2,"养子");
        childParentDao.insert(childParent);
    }

}
