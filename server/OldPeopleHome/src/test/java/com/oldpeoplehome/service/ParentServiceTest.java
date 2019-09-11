package com.oldpeoplehome.service;

import com.oldpeoplehome.BaseTest;
import com.oldpeoplehome.entity.Parent;
import org.junit.Test;
import org.springframework.beans.factory.annotation.Autowired;

/**
 * Created By Jiangyuwei on 2019/8/21 12:10
 * Description:
 */
public class ParentServiceTest extends BaseTest {

    @Autowired
    private ParentService parentService;

    @Test
    public void testFind(){
        System.out.println(parentService.findByID(1));
    }

    @Test
    public void testInsert(){
        Parent parent = new Parent();
        parent.setParentSex("a");
        parent.setParentPassword("a");
        parent.setParentAccount("a");
        parent.setParentName("a");
        parent.mySetParentBirth("2011-2-2");
        parent.setParentLongId("a");
        parentService.insert(parent);
        System.out.println(parentService.findByLongId("a"));
    }

    @Test
    public void delete(){
        parentService.delete(Long.valueOf(22));
        System.out.println(parentService.findAll());
    }
}
