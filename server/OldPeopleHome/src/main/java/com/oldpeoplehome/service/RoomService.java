package com.oldpeoplehome.service;

import com.oldpeoplehome.entity.Room;

import java.util.List;

/**
 * Created By Jiangyuwei on 2019/8/19 21:26
 * Description:
 */
public interface RoomService {

    Room findById(int roomId);

    List<Room> findAll();

    int insert(Room room);

    int update(Room room);

    int delete(int roomId);

}
