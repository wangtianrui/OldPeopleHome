package com.oldpeoplehome.dao;

import com.oldpeoplehome.entity.Child;

import java.util.List;

/**
 * Created By Jiangyuwei on 2019/8/21 10:16
 * Description:
 */
public interface ChildDao {

    Child findById(long childId);
    Child findByLongId(String childLongId);
    Child findByName(String childName);
    List<Child> findAll();
    void delete(long childId);
    void update(Child child);
    void insert(Child child);

}
