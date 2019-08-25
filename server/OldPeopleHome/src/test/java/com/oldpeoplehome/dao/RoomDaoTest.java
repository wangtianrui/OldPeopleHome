package com.oldpeoplehome.dao;

import com.oldpeoplehome.BaseTest;
import com.oldpeoplehome.entity.Parent;
import com.oldpeoplehome.entity.Room;
import com.oldpeoplehome.utils.StringUtil;
import org.junit.Test;
import org.springframework.beans.factory.annotation.Autowired;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.sql.Timestamp;
import java.util.*;

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

    @Test
    public void testInsert(){
        Room room = new Room("惠子楼", "241");
        int s = roomDao.insertRoom(room);
        System.out.println("*******" + s);
    }

    @Test
    public void testDelete(){
        int id = 3;
        int s = roomDao.deleteRoom(id);
        System.out.println("*******" + s);
    }

    @Test
    public void testUpdate() throws IllegalAccessException, NoSuchMethodException, InvocationTargetException {
        int roomId = 4;
        Map<String, Object> params = new HashMap<>();
        params.put("roomLocation", "钉子楼");
        params.put("roomName", "9128");
        Room room = roomDao.findById(roomId);
        Class<?> pClass = room.getClass();
        for(Map.Entry<String, Object> entry: params.entrySet()){
            Method method = pClass.getMethod("set"+StringUtil.capital(entry.getKey()), entry.getValue().getClass());
            System.out.println("***" + method);
            method.invoke(room, entry.getValue());
        }
        System.out.println(room);
        roomDao.updateRoom(room);
    }

    @Test
    public void testFindEmpty(){
        System.out.println(roomDao.findEmptyRoom());
    }

    @Test
    public void testA(){
        Parent parent = new Parent();
        System.out.println(parent.getClass());
        Object obj = parent;
        System.out.println(obj.getClass());
        System.out.println(Timestamp.valueOf("2018-2-2 1:1:1"));
    }

}
