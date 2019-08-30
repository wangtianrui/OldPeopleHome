package com.oldpeoplehome.service.impl;

import com.oldpeoplehome.dao.AdminDao;
import com.oldpeoplehome.entity.Admin;
import com.oldpeoplehome.service.AdminService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

/**
 * Created By Jiangyuwei on 2019/8/30 11:31
 * Description:
 */
@Service
public class AdminServiceImpl implements AdminService {

    @Autowired
    private AdminDao adminDao;

    @Override
    public Admin login(String account) {
        return adminDao.findByAccount(account);
    }
}
