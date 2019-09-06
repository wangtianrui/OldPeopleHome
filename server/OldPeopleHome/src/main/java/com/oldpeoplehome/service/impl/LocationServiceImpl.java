package com.oldpeoplehome.service.impl;

import com.oldpeoplehome.dao.LocationDao;
import com.oldpeoplehome.dto.LocationFilter;
import com.oldpeoplehome.entity.Location;
import com.oldpeoplehome.service.LocationService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

/**
 * Created By Jiangyuwei on 2019/9/6 11:36
 * Description:
 */
@Service
public class LocationServiceImpl implements LocationService {

    @Autowired
    private LocationDao locationDao;

    @Override
    public List<Location> findByParent(LocationFilter locationFilter) {
        return locationDao.findByParent(locationFilter);
    }

    @Override
    public List<Location> findByTime(LocationFilter locationFilter) {
        return locationDao.findByTime(locationFilter);
    }

    @Override
    public List<Location> findByParentAndTime(LocationFilter locationFilter) {
        return locationDao.findByParentAndTime(locationFilter);
    }

    @Override
    public void insert(Location location) {
        locationDao.insert(location);
    }
}
