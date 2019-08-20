package com.oldpeoplehome.dao;

import com.oldpeoplehome.entity.Parent;

import java.awt.print.PrinterAbortException;
import java.util.List;

/**
 * Created By Jiangyuwei on 2019/8/20 13:27
 * Description:
 */
public interface ParentDao {

    Parent findByID(long id);
    Parent findByLongId(String longId);
    Parent findByRoomId(int roomId);
    Parent findByName(String name);
    List<Parent> findAll();
    void insert(Parent parent);
    void update(Parent parent);
    void delete(long id);

}
