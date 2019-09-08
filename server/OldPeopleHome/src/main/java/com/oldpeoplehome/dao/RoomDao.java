package com.oldpeoplehome.dao;

import com.oldpeoplehome.entity.Room;

import java.util.List;

/**
 * Created By Jiangyuwei on 2019/8/19 14:28
 * Description:
 */
public interface RoomDao {
    //通过id查房间
    Room findById(int roomId);
    List<Room> findEmptyRoom();
    //查所有房间
    List<Room> findAll();
    //添加房间
    int insertRoom(Room room);
    //删除房间
    int deleteRoom(int roomId);
    //更新
    int updateRoom(Room room);



}
