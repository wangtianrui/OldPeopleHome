package com.oldpeoplehome.dao;

import com.oldpeoplehome.entity.Child;
import com.oldpeoplehome.entity.ChildParent;
import com.oldpeoplehome.entity.Parent;

import java.util.List;

/**
 * Created By Jiangyuwei on 2019/8/21 14:20
 * Description:
 */
public interface ChildParentDao {
    List<Parent> findByChild(long childId);
    List<Child> findByParent(long parentId);
    void insert(ChildParent childParent);
}
