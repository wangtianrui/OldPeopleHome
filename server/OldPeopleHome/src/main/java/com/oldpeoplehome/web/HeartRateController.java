package com.oldpeoplehome.web;

import com.oldpeoplehome.dto.HeartRateFilter;
import com.oldpeoplehome.dto.RoomStateFilter;
import com.oldpeoplehome.entity.HeartRate;
import com.oldpeoplehome.entity.RoomState;
import com.oldpeoplehome.service.HeartRateService;
import com.oldpeoplehome.service.RoomStateService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import javax.servlet.http.HttpServletRequest;
import java.sql.Timestamp;
import java.util.List;
import java.util.Map;

/**
 * Created By Jiangyuwei on 2019/8/22 21:58
 * Description:
 */

@RestController
@RequestMapping("/heartrate")
public class HeartRateController {

    @Autowired
    private HeartRateService heartRateService;

    @RequestMapping("/get/{id}")
    @ResponseBody
    public List findByParentId(HttpServletRequest request, @PathVariable("id") String id) {
        List<HeartRate> list;
        String startDate = request.getParameter("start");
        String endDate = request.getParameter("end");
        if (endDate == null) {
            if (startDate == null) {
                HeartRateFilter heartRateFilter = new HeartRateFilter(Long.valueOf(id));
                list = heartRateService.findByParent(heartRateFilter);
            } else {
                HeartRateFilter heartRateFilter = new HeartRateFilter(Timestamp.valueOf(startDate), Long.valueOf(id));
                list = heartRateService.findByParentAndTime(heartRateFilter);
            }
        } else {
            HeartRateFilter heartRateFilter = new HeartRateFilter(Timestamp.valueOf(startDate), Timestamp.valueOf(endDate), Long.valueOf(id));
            list = heartRateService.findByTime(heartRateFilter);
        }
        return list;
    }

    @RequestMapping(value = "/add", method = RequestMethod.POST)
    @ResponseBody
    public void add(@RequestParam Map<String, Object> params) {
        long parentId = Long.valueOf(String.valueOf(params.get("parentId")));
        String time = String.valueOf(params.get("time"));
        double rate = Double.valueOf(String.valueOf(params.get("rate")));
        double rate1 = Double.valueOf(String.valueOf(params.get("rate1")));
        double rate2 = Double.valueOf(String.valueOf(params.get("rate2")));
        double oxy = Double.valueOf(String.valueOf(params.get("oxy")));
        HeartRate heartRate = new HeartRate(parentId, time, rate, rate1, rate2, oxy);
        heartRateService.insert(heartRate);
    }

}
