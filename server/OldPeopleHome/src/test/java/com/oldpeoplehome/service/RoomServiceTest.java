package com.oldpeoplehome.service;

import com.oldpeoplehome.BaseTest;
import com.oldpeoplehome.entity.Room;
import org.junit.Test;
import org.springframework.beans.factory.annotation.Autowired;

/**
 * Created By Jiangyuwei on 2019/9/7 11:10
 * Description:
 */
public class RoomServiceTest extends BaseTest {

    @Autowired
    private RoomService roomService;

    @Test
    public void test(){
        Room room = roomService.findById(1);
        room.setTemp(22.2f);
        roomService.update(room);
    }

}
