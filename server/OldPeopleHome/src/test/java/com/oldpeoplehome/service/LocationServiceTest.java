package com.oldpeoplehome.service;

import com.oldpeoplehome.BaseTest;
import com.oldpeoplehome.dto.LocationFilter;
import com.oldpeoplehome.entity.Location;
import org.junit.Test;
import org.springframework.beans.factory.annotation.Autowired;

/**
 * Created By Jiangyuwei on 2019/9/6 11:38
 * Description:
 */
public class LocationServiceTest extends BaseTest {

    @Autowired
    private LocationService locationService;

    @Test
    public void test(){
//        Location location = new Location(new Long(1),13.2323f,123.45f,"2019-4-5 19:2:2");
//        locationService.insert(location);
        LocationFilter locationFilter = new LocationFilter("2019-4-5 19:2:7","2019-4-6 19:2:2",1);
        System.out.println(locationService.findByTime(locationFilter));
    }

}
