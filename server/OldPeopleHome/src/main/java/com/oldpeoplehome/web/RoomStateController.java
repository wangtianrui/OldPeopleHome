package com.oldpeoplehome.web;

import com.oldpeoplehome.dto.RoomStateFilter;
import com.oldpeoplehome.entity.RoomState;
import com.oldpeoplehome.service.RoomStateService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import javax.servlet.http.HttpServletRequest;
import java.sql.Timestamp;
import java.util.List;
import java.util.Map;

/**
 * Created By Jiangyuwei on 2019/8/22 21:23
 * Description:
 */

@RestController
@RequestMapping("/rstate")
public class RoomStateController {

    @Autowired
    private RoomStateService roomStateService;

    @RequestMapping("/get/{id}")
    @ResponseBody
    public List findByParentId(HttpServletRequest request, @PathVariable("id") String id) {
        List<RoomState> list;
        String startDate = request.getParameter("start");
        String endDate = request.getParameter("end");
        if (endDate == null){
            if (startDate == null) {
                RoomStateFilter roomStateFilter = new RoomStateFilter(Integer.valueOf(id));
                list = roomStateService.findByRoom(roomStateFilter);
            }
            else {
                RoomStateFilter roomStateFilter = new RoomStateFilter(Timestamp.valueOf(startDate), Integer.valueOf(id));
                list = roomStateService.findByRoomAndTime(roomStateFilter);
            }
        }
        else{
            RoomStateFilter roomStateFilter = new RoomStateFilter(Timestamp.valueOf(startDate), Timestamp.valueOf(endDate), Integer.valueOf(id));
            list = roomStateService.findByTime(roomStateFilter);
        }
        return list;
    }

    @RequestMapping(value = "/add", method = RequestMethod.POST)
    @ResponseBody
    public void add(@RequestParam Map<String, Object> params){
        int roomId = Integer.valueOf(String.valueOf(params.get("roomId")));
        String time = String.valueOf(params.get("time"));
        Double temperature = Double.valueOf(String.valueOf(params.get("temperature")));
        double humidity = Double.valueOf(String.valueOf(params.get("humidity")));
        int isin = Integer.valueOf(String.valueOf(params.get("isin")));
        RoomState roomState = new RoomState(roomId, time, temperature, humidity, isin);
        roomStateService.insert(roomState);
    }

}
