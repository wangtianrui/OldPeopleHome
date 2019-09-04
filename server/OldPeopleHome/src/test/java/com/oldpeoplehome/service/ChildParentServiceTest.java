package com.oldpeoplehome.service;

import com.oldpeoplehome.BaseTest;
import com.oldpeoplehome.entity.ChildParent;
import org.junit.Test;
import org.springframework.beans.factory.annotation.Autowired;

/**
 * Created By Jiangyuwei on 2019/9/4 21:27
 * Description:
 */
public class ChildParentServiceTest extends BaseTest {

    @Autowired
    private ChildParentService childParentService;

    @Test
    public void test(){
        ChildParent childParent = new ChildParent(1,1,"父子");
        childParentService.combine(childParent);
    }

}
