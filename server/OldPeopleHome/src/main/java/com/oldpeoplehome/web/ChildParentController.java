package com.oldpeoplehome.web;

import com.oldpeoplehome.entity.Child;
import com.oldpeoplehome.entity.ChildParent;
import com.oldpeoplehome.entity.Parent;
import com.oldpeoplehome.entity.Room;
import com.oldpeoplehome.service.ChildParentService;
import com.oldpeoplehome.service.ParentService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.util.List;
import java.util.Map;

/**
 * Created By Jiangyuwei on 2019/8/21 15:45
 * Description:
 */
@RestController
@RequestMapping("/child_parent")
public class ChildParentController {

    @Autowired
    private ChildParentService childParentService;
    @Autowired
    private ParentService parentService;

    @RequestMapping("/child/{id}")
    @ResponseBody
    public List findByChild(@PathVariable("id") String id) {
        List<Parent> list = childParentService.findByChild(Long.valueOf(id));
        System.out.println(list);
        return list;
    }

    @RequestMapping("/parent/{id}")
    @ResponseBody
    public List findByParent(@PathVariable("id") String id) {
        List<Child> list = childParentService.findByParent(Long.valueOf(id));
        System.out.println(list);
        return list;
    }

    @RequestMapping(value = "/add", method = RequestMethod.POST)
    @ResponseBody
    public Parent add(@RequestParam Map<String, Object> params) {

        long child = Long.valueOf(String.valueOf(params.get("child")));
        long parent = Long.valueOf(String.valueOf(params.get("parent")));
        String relation = String.valueOf(params.get("relation"));
        ChildParent childParent = new ChildParent(parent, child, relation);
        childParentService.combine(childParent);
        return parentService.findByID(parent);
    }
}
