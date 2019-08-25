package com.oldpeoplehome.service;

import com.oldpeoplehome.dto.RoomStateFilter;
import com.oldpeoplehome.entity.RoomState;

import java.util.List;

/**
 * Created By Jiangyuwei on 2019/8/22 21:20
 * Description:
 */
public interface RoomStateService {

    List<RoomState> findByRoom(RoomStateFilter roomStateFilter);
    List<RoomState> findByTime(RoomStateFilter roomStateFilter);
    List<RoomState> findByRoomAndTime(RoomStateFilter roomStateFilter);

    void insert(RoomState roomState);

}
