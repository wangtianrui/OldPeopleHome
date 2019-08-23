package com.oldpeoplehome.service.impl;

import com.oldpeoplehome.dao.ChildParentDao;
import com.oldpeoplehome.entity.Child;
import com.oldpeoplehome.entity.ChildParent;
import com.oldpeoplehome.entity.Parent;
import com.oldpeoplehome.service.ChildParentService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

/**
 * Created By Jiangyuwei on 2019/8/21 15:24
 * Description:
 */
@Service
public class ChildParentServiceImpl implements ChildParentService {

    @Autowired
    private ChildParentDao childParentDao;

    @Override
    public List<Parent> findByChild(long childId) {
        return childParentDao.findByChild(childId);
    }

    @Override
    public List<Child> findByParent(long parentId) {
        return childParentDao.findByParent(parentId);
    }

    @Override
    public void combine(ChildParent childParent) {
        childParentDao.insert(childParent);
    }
}
