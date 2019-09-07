package com.oldpeoplehome.web;

import com.oldpeoplehome.dto.LocationFilter;
import com.oldpeoplehome.entity.Location;
import com.oldpeoplehome.service.LocationService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import javax.servlet.http.HttpServletRequest;
import java.sql.Time;
import java.sql.Timestamp;
import java.util.List;
import java.util.Map;

/**
 * Created By Jiangyuwei on 2019/9/6 13:51
 * Description:
 */
@RestController
@RequestMapping("/location")
public class LocationController {

    @Autowired
    private LocationService locationService;

    @RequestMapping("/get/{id}")
    @ResponseBody
    public List findByParentId(HttpServletRequest request, @PathVariable("id") String id) {
        List<Location> list;
        String startDate = request.getParameter("start");
        String endDate = request.getParameter("end");
        if (endDate == null){
            if (startDate == null) {
                LocationFilter locationFilter = new LocationFilter(Long.valueOf(id));
                list = locationService.findByParent(locationFilter);
            }
            else {
                LocationFilter locationFilter = new LocationFilter(startDate, Long.valueOf(id));
                list = locationService.findByParentAndTime(locationFilter);
            }
        }
        else{
            LocationFilter locationFilter = new LocationFilter(startDate, endDate, Long.valueOf(id));
            list = locationService.findByTime(locationFilter);
        }
        return list;
    }

    @RequestMapping(value = "/add", method = RequestMethod.POST)
    @ResponseBody
    public Location add(@RequestParam Map<String, Object> params){
        long parentId = Long.valueOf(String.valueOf(params.get("parent")));
        float longitude = Float.valueOf(String.valueOf(params.get("longitude")));
        float latitude = Float.valueOf(String.valueOf(params.get("latitude")));
        String time = String.valueOf(params.get("time"));
        Location location = new Location(parentId,longitude,latitude,time);
        locationService.insert(location);
        return location;
    }

}
