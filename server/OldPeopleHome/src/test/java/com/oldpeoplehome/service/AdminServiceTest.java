package com.oldpeoplehome.service;

import com.oldpeoplehome.BaseTest;
import org.junit.Test;
import org.springframework.beans.factory.annotation.Autowired;

/**
 * Created By Jiangyuwei on 2019/8/30 11:33
 * Description:
 */
public class AdminServiceTest extends BaseTest {

    @Autowired
    private AdminService adminService;

    @Test
    public void testLogin(){

        System.out.println(adminService.login("root"));

    }

}
