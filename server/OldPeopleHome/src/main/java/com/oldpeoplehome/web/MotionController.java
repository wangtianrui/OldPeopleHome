package com.oldpeoplehome.web;

import com.oldpeoplehome.dto.MotionFilter;
import com.oldpeoplehome.entity.Motion;
import com.oldpeoplehome.entity.Parent;
import com.oldpeoplehome.service.MotionService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import javax.servlet.http.HttpServletRequest;
import java.sql.Date;
import java.util.List;
import java.util.Map;

/*
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

/**
 * Created By Jiangyuwei on 2019/8/22 14:34
 * Description:
 */
@RestController
@RequestMapping("/motion")
public class MotionController {


    @Autowired
    private MotionService motionService;

    @RequestMapping("/get/{id}")
    @ResponseBody
    public List findByParentId(HttpServletRequest request, @PathVariable("id") String id) {
        List<Motion> list;
        String startDate = request.getParameter("start");
        String endDate = request.getParameter("end");
        if (endDate == null){
            if (startDate == null) {
                MotionFilter motionFilter = new MotionFilter(Long.valueOf(id));
                list = motionService.findByParentId(motionFilter);
            }
            else {
                MotionFilter motionFilter = new MotionFilter(Date.valueOf(startDate), Long.valueOf(id));
                list = motionService.findByParentIdAndDate(motionFilter);
            }
        }
        else{
            MotionFilter motionFilter = new MotionFilter(Date.valueOf(startDate), Date.valueOf(endDate), Long.valueOf(id));
            list = motionService.findByDate(motionFilter);
        }
        return list;
    }

    @RequestMapping(value = "/add", method = RequestMethod.POST)
    @ResponseBody
    public void add(@RequestParam Map<String, Object> params){
        long parentId = Long.valueOf(String.valueOf(params.get("parent")));
        String date = String.valueOf(params.get("date"));
        long count = Long.valueOf(String.valueOf(params.get("count")));
        double distance = Long.valueOf(String.valueOf(params.get("distance")));
        double energy = Long.valueOf(String.valueOf(params.get("energy")));
        String time = String.valueOf(params.get("time"));
        Motion motion = new Motion(parentId, date, count, distance,energy, time);
        motionService.insert(motion);
    }

}
