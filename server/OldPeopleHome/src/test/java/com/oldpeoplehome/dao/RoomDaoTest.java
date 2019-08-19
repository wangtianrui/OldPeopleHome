package com.oldpeoplehome.dao;

import com.oldpeoplehome.BaseTest;
import com.oldpeoplehome.entity.Room;
import org.junit.Test;
import org.springframework.beans.factory.annotation.Autowired;

import java.util.List;

/**
 * Created By Jiangyuwei on 2019/8/19 15:01
 * Description:
 */
public class RoomDaoTest extends BaseTest {

    @Autowired
    private RoomDao roomDao;

    @Test
    public void testFindById() throws Exception{
        int roomId = 1;
        Room room = roomDao.findById(roomId);
        System.out.println(room);
    }

    @Test
    public void testFindAll(){
        List<Room> s = roomDao.findAll();
        System.out.println(s);
    }

}
