package com.oldpeoplehome.service.impl;

import com.oldpeoplehome.dao.ChildDao;
import com.oldpeoplehome.entity.Child;
import com.oldpeoplehome.service.ChildService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

/**
 * Created By Jiangyuwei on 2019/8/21 11:14
 * Description:
 */
@Service
public class ChildServiceImpl implements ChildService {

    @Autowired
    private ChildDao childDao;

    @Override
    public Child findById(long childId) {
        return childDao.findById(childId);
    }

    @Override
    public Child findByLongId(String childLongId) {
        return childDao.findByLongId(childLongId);
    }

    @Override
    public Child findByName(String childName) {
        return childDao.findByName(childName);
    }

    @Override
    public List<Child> findAll() {
        return childDao.findAll();
    }

    @Override
    public void delete(long childId) {
        childDao.delete(childId);
    }

    @Override
    public void update(Child child) {
        childDao.update(child);
    }

    @Override
    public void insert(Child child) {
        childDao.insert(child);
    }
}
