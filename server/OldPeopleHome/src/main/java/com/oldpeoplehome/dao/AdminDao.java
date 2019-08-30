package com.oldpeoplehome.dao;

import com.oldpeoplehome.entity.Admin;

/**
 * Created By Jiangyuwei on 2019/8/30 11:25
 * Description:
 */
public interface AdminDao {

    Admin findByAccount(String account);

}
