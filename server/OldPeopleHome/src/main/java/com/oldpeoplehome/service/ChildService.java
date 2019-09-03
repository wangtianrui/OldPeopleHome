package com.oldpeoplehome.service;

import com.oldpeoplehome.entity.Child;

import java.util.List;

/**
 * Created By Jiangyuwei on 2019/8/21 11:14
 * Description:
 */
public interface ChildService {

    Child findById(long childId);
    Child findByLongId(String childLongId);
    Child findByName(String childName);
    Child login(String account);
    List<Child> findAll();
    void delete(long childId);
    void update(Child child);
    void insert(Child child);

}
