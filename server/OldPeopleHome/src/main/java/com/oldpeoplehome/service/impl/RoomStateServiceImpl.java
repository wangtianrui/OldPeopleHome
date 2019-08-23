package com.oldpeoplehome.service.impl;

import com.oldpeoplehome.dao.RoomStateDao;
import com.oldpeoplehome.dto.RoomStateFilter;
import com.oldpeoplehome.entity.RoomState;
import com.oldpeoplehome.service.RoomStateService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

/**
 * Created By Jiangyuwei on 2019/8/22 21:20
 * Description:
 */
@Service
public class RoomStateServiceImpl implements RoomStateService {

    @Autowired
    private RoomStateDao roomStateDao;

    @Override
    public List<RoomState> findByRoom(RoomStateFilter roomStateFilter) {
        return roomStateDao.findByRoom(roomStateFilter);
    }

    @Override
    public List<RoomState> findByTime(RoomStateFilter roomStateFilter) {
        return roomStateDao.findByTime(roomStateFilter);
    }

    @Override
    public List<RoomState> findByRoomAndTime(RoomStateFilter roomStateFilter) {
        return roomStateDao.findByRoomAndTime(roomStateFilter);
    }

    @Override
    public void insert(RoomState roomState) {
        roomStateDao.insert(roomState);
    }
}
