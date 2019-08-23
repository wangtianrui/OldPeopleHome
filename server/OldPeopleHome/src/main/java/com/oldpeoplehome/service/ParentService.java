package com.oldpeoplehome.service;

import com.oldpeoplehome.entity.Parent;

import java.util.List;

/**
 * Created By Jiangyuwei on 2019/8/20 18:07
 * Description:
 */
public interface ParentService {
    Parent findByID(long id);
    Parent findByLongId(String longId);
    Parent findByRoomId(int roomId);
    Parent findByName(String name);
    List<Parent> findAll();
    void insert(Parent parent);
    void update(Parent parent);
    void delete(long id);
    Parent changeRoom(Parent parent, int RoomId);
}
