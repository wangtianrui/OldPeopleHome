package com.oldpeoplehome.dao;

import com.oldpeoplehome.BaseTest;
import com.oldpeoplehome.entity.Child;
import org.junit.Test;
import org.springframework.beans.factory.annotation.Autowired;

/**
 * Created By Jiangyuwei on 2019/8/21 10:57
 * Description:
 */
public class ChildDaoTest extends BaseTest {

    @Autowired
    private ChildDao childDao;

    @Test
    public void testInsert(){
        Child child = new Child("519404401440404404", "鲍欠日", "女", "bao", "123", "12345678900");
        childDao.insert(child);
    }
    @Test
    public void testFInd(){
        System.out.println("name:"+ childDao.findAll());
        System.out.println("id"+childDao.findById(1));
        System.out.println("name"+childDao.findByName("鲍欠月"));
        System.out.println("longid"+childDao.findByLongId("519404400440404404"));
    }
    @Test
    public void testUpdate(){
        Child child = childDao.findById(1);
        child.setChildPhone("1020");
        childDao.update(child);
    }

    @Test
    public void testDelete(){
        childDao.delete(2);
    }
}
