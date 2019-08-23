package com.oldpeoplehome.web;

import com.oldpeoplehome.dto.SleepFilter;
import com.oldpeoplehome.entity.Sleep;
import com.oldpeoplehome.service.SleepService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import javax.servlet.http.HttpServletRequest;
import java.sql.Date;
import java.util.List;
import java.util.Map;

/**
 * Created By Jiangyuwei on 2019/8/22 17:06
 * Description:
 */
@RestController
@RequestMapping("/sleep")
public class SleepController {
    @Autowired
    private SleepService sleepService;

    @RequestMapping("/get/{id}")
    @ResponseBody
    public List findByParentId(HttpServletRequest request, @PathVariable("id") String id) {
        List<Sleep> list;
        String startDate = request.getParameter("start");
        String endDate = request.getParameter("end");
        if (endDate == null){
            if (startDate == null) {
                SleepFilter sleepFilter = new SleepFilter(Long.valueOf(id));
                list = sleepService.findByParentId(sleepFilter);
            }
            else {
                SleepFilter sleepFilter = new SleepFilter(Date.valueOf(startDate), Long.valueOf(id));
                list = sleepService.findByParentIdAndDate(sleepFilter);
            }
        }
        else{
            SleepFilter sleepFilter = new SleepFilter(Date.valueOf(startDate), Date.valueOf(endDate), Long.valueOf(id));
            list = sleepService.findByDate(sleepFilter);
        }
        return list;
    }

    @RequestMapping(value = "/add", method = RequestMethod.POST)
    @ResponseBody
    public void add(@RequestParam Map<String, Object> params){
        long parentId = Long.valueOf(String.valueOf(params.get("parent")));
        String date = String.valueOf(params.get("date"));
        String deep = String.valueOf(params.get("deep"));
        String light = String.valueOf(params.get("light"));
        String awake = String.valueOf(params.get("awake"));
        Sleep sleep = new Sleep(parentId, date, deep, light, awake);
        sleepService.insert(sleep);
    }
}
