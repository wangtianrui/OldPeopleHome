package com.oldpeoplehome.service.impl;

import com.oldpeoplehome.dao.ParentDao;
import com.oldpeoplehome.entity.Parent;
import com.oldpeoplehome.service.ParentService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

/**
 * Created By Jiangyuwei on 2019/8/20 18:08
 * Description:
 */
@Service
public class ParentServiceImpl implements ParentService {



    @Autowired
    private ParentDao parentDao;

    @Override
    public Parent findByID(long id) {
        return parentDao.findByID(id);
    }

    @Override
    public Parent findByLongId(String longId) {
        return parentDao.findByLongId(longId);
    }

    @Override
    public Parent findByRoomId(int roomId) {
        return parentDao.findByRoomId(roomId);
    }

    @Override
    public Parent changeRoom(Parent parent, int RoomId) {
        return null;
    }

    @Override
    public Parent findByName(String name) {
        return parentDao.findByName(name);
    }

    @Override
    public List<Parent> findAll() {
        return parentDao.findAll();
    }

    @Override
    public void insert(Parent parent) {
        parentDao.insert(parent);
    }

    @Override
    public void update(Parent parent) {
        parentDao.update(parent);
    }

    @Override
    public void delete(long id) {
        parentDao.delete(id);
    }
    @Override
    public Parent login(String account) {
        return parentDao.findByAccount(account);
    }
}
