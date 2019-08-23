package com.oldpeoplehome.service;

import com.oldpeoplehome.entity.Child;
import com.oldpeoplehome.entity.ChildParent;
import com.oldpeoplehome.entity.Parent;

import java.util.List;

/**
 * Created By Jiangyuwei on 2019/8/21 15:24
 * Description:
 */
public interface ChildParentService {

    List<Parent> findByChild(long childId);
    List<Child> findByParent(long parentId);
    void combine(ChildParent childParent);

}
