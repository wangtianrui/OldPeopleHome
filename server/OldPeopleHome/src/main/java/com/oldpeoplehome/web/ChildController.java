package com.oldpeoplehome.web;

import com.oldpeoplehome.entity.Child;
import com.oldpeoplehome.entity.Parent;
import com.oldpeoplehome.service.ChildService;
import com.oldpeoplehome.utils.MethodUtil;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.lang.reflect.InvocationTargetException;
import java.util.List;
import java.util.Map;

/**
 * Created By Jiangyuwei on 2019/8/21 11:18
 * Description:
 */
@RestController
@RequestMapping("/child")
public class ChildController {

    @Autowired
    private ChildService childService;

    @RequestMapping("/get/{id}")
    @ResponseBody
    public Child get(@PathVariable("id") String id){
        return childService.findById(Long.valueOf(id));
    }
    @RequestMapping("/get_longid/{longid}")
    @ResponseBody
    public Child getLong(@PathVariable("longid") String id){
        return childService.findByLongId(id);
    }
    @RequestMapping("/get_name/{name}")
    @ResponseBody
    public Child getName(@PathVariable("name") String name){
        return childService.findByName(name);
    }
    @RequestMapping("/list")
    @ResponseBody
    public List list() {
        return childService.findAll();
    }

    @RequestMapping(value = "/add", method = RequestMethod.POST)
    @ResponseBody
    public Child add(@RequestParam Map<String, Object> params){
        Child child = new Child();
        child.setChildSex(String.valueOf(params.get("childSex")));
        child.setChildPassword(String.valueOf(params.get("childPassword")));
        child.setChildAccount(String.valueOf(params.get("childAccount")));
        child.setChildName(String.valueOf(params.get("childName")));
        child.setChildLongId(String.valueOf(params.get("childLongId")));
        child.setChildPhone(String.valueOf(params.get("childPhone")));
        childService.insert(child);
        return childService.findByLongId(child.getChildLongId());
    }

    @RequestMapping(value = "/update/{id}", method = RequestMethod.POST)
    @ResponseBody
    public Child update(@RequestParam Map<String, Object> params, @PathVariable("id") String id) throws NoSuchMethodException, IllegalAccessException, InvocationTargetException {
        Child child = childService.findById(Long.valueOf(id));
        MethodUtil.updateFields(child, params);
        childService.update(child);
        return child;
    }

    @RequestMapping(value = "/delete/{id}", method = RequestMethod.DELETE)
    @ResponseBody
    public List<Child> delete(@PathVariable("id") String id){
        childService.delete(Long.valueOf(id));
        return childService.findAll();
    }
}
