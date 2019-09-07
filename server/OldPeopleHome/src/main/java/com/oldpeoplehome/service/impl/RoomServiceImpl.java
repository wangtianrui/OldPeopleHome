package com.oldpeoplehome.service.impl;

import com.oldpeoplehome.dao.RoomDao;
import com.oldpeoplehome.entity.Room;
import com.oldpeoplehome.service.RoomService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.awt.print.Book;
import java.util.List;

/**
 * Created By Jiangyuwei on 2019/8/19 21:27
 * Description:
 */
@Service
public class RoomServiceImpl implements RoomService {

    @Autowired
    private RoomDao roomDao;

    @Override
    public Room findById(int roomId) {
        return roomDao.findById(roomId);
    }

    @Override
    public List<Room> findAll() {
        return roomDao.findAll();
    }

    @Override
    public int insert(Room room) {
        return roomDao.insertRoom(room);
    }

    @Override
    public int update(Room room) {
        return roomDao.updateRoom(room);
    }

    @Override
    public List<Room> findEmpty() {
        return roomDao.findEmptyRoom();
    }

    @Override
    public int delete(int roomId) {
        return roomDao.deleteRoom(roomId);
    }

}
