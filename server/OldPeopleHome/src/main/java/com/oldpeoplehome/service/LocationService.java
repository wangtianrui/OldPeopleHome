package com.oldpeoplehome.service;

import com.oldpeoplehome.dto.LocationFilter;
import com.oldpeoplehome.entity.Location;

import java.util.List;

/**
 * Created By Jiangyuwei on 2019/9/6 11:35
 * Description:
 */
public interface LocationService {


    List<Location> findByParent(LocationFilter locationFilter);
    List<Location> findByTime(LocationFilter locationFilter);
    List<Location> findByParentAndTime(LocationFilter locationFilter);

    void insert(Location location);
}
