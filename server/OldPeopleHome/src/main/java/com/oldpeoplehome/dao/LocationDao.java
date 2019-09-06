package com.oldpeoplehome.dao;

import com.oldpeoplehome.dto.LocationFilter;
import com.oldpeoplehome.entity.Location;

import java.util.List;

/**
 * Created By Jiangyuwei on 2019/9/6 11:30
 * Description:
 */
public interface LocationDao {

    List<Location> findByParent(LocationFilter locationFilter);
    List<Location> findByTime(LocationFilter locationFilter);
    List<Location> findByParentAndTime(LocationFilter locationFilter);

    void insert(Location location);

}
