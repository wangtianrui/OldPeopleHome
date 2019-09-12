package com.oldpeoplehome.web;

import com.oldpeoplehome.entity.Room;
import com.oldpeoplehome.service.RoomService;

import com.oldpeoplehome.utils.MethodUtil;
import com.oldpeoplehome.utils.Result;
import com.oldpeoplehome.utils.StringUtil;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.io.UnsupportedEncodingException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.List;
import java.util.Map;

/**
 * Created By Jiangyuwei on 2019/8/19 21:46
 * Description:
 */
@RestController
@RequestMapping("/room")
public class RoomController {
    @Autowired
    private RoomService roomService;

    @RequestMapping("/get/{id}")
    @ResponseBody
    public Room get(@PathVariable("id") String id) {
        Room room = roomService.findById(Integer.valueOf(id));
        System.out.println(room);
        return room;
    }

    @RequestMapping("/get/mixMac/{mixMac}")
    @ResponseBody
    public Room getByMixMac(@PathVariable("mixMac") String mixMac) {
        Room room = roomService.findByMixMac(mixMac);
        System.out.println(room);
        return room;
    }

    @RequestMapping("/list")
    @ResponseBody
    public List list() {
        List<Room> list = roomService.findAll();
        System.out.println(list);
        return list;
    }
    @RequestMapping("/list/empty")
    @ResponseBody
    public List listEmpty() {
        List<Room> list = roomService.findEmpty();
        System.out.println(list);
        return list;
    }

    @RequestMapping(value = "/add", method = RequestMethod.POST)
    @ResponseBody
    public Room add(@RequestParam Map<String, Object> param) {
        Room room = new Room(String.valueOf(param.get("location")), String.valueOf(param.get("name")));
        roomService.insert(room);
        return room;
    }

    @RequestMapping(value = "/delete/{id}", method = RequestMethod.DELETE)
    @ResponseBody
    public List delete(@PathVariable("id") String id) {
        roomService.delete(Integer.valueOf(id));
        List<Room> list = roomService.findAll();
        System.out.println(list);
        return list;
    }

    @RequestMapping(value = "/update/{id}", method = RequestMethod.POST)
    @ResponseBody
    public Room update(@RequestParam Map<String, Object> params, @PathVariable("id") String id) throws IllegalAccessException, NoSuchMethodException, InvocationTargetException {
        Room room = roomService.findById(Integer.valueOf(id));
        MethodUtil.updateFields(room, params);
        roomService.update(room);
        return room;
    }


}
