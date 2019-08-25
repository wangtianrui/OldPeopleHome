package com.oldpeoplehome.dao;

import com.oldpeoplehome.BaseTest;
import com.oldpeoplehome.entity.Parent;
import com.oldpeoplehome.entity.Room;
import com.oldpeoplehome.utils.StringUtil;
import org.junit.Test;
import org.springframework.beans.factory.annotation.Autowired;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.sql.Date;
import java.util.HashMap;
import java.util.Map;

/**
 * Created By Jiangyuwei on 2019/8/20 13:53
 * Description:
 */
public class ParentDaoTest extends BaseTest {
    @Autowired
    private ParentDao parentDao;
    @Autowired
    private RoomDao roomDao;

    @Test
    public void testFindById(){
        System.out.println( parentDao.findByID(1) );
    }
    @Test
    public void testFindByName(){
        System.out.println( parentDao.findByName("jiang") );
    }
    @Test
    public void testFindByLong(){
        System.out.println( parentDao.findByLongId("510703199706330018") );
    }
    @Test
    public void testFindByRoom(){
        System.out.println( parentDao.findByRoomId(1) );
    }
    @Test
    public void testFindAll(){
        System.out.println( parentDao.findAll() );
    }
    @Test
    public void testInsert(){
        Parent parent = new Parent();
        parent.setParentLongId("510703199701213123");
        parent.setParentBirth(Date.valueOf("1927-4-5"));
        parent.setParentName("当当娃");
        parent.setParentAccount("dang");
        parent.setParentHeight(12.1);
        parent.setParentWeight(3112.1);
        parent.setParentPassword("123");
        parent.setParentSex("女");
        parent.setParentPhone("12312312312");
        Room room = roomDao.findById(1);
        System.out.println(room);
        parentDao.insert(parent);
        parent = parentDao.findByLongId(parent.getParentLongId());
        room.setParentId(parent.getParentId());
        roomDao.updateRoom(room);
        System.out.println(parent);
    }

    @Test
    public void testUpdate()throws IllegalAccessException, NoSuchMethodException, InvocationTargetException {
        Map<String, Object> params = new HashMap<>();
        params.put("parentName", "咪咪娃");
        params.put("parentBirth", "2017-2-23");
        Parent parent = parentDao.findByID(6);
        Class<?> pClass = parent.getClass();
        for (Map.Entry<String, Object> entry: params.entrySet()){
            Method method = pClass.getMethod("set"+ StringUtil.capital(entry.getKey()), entry.getValue().getClass());
            System.out.println("***" + method);
            method.invoke(parent, entry.getValue());
        }
        System.out.println(parent);
        parentDao.update(parent);
    }

}
