package com.oldpeoplehome.web;

import com.oldpeoplehome.entity.Admin;
import com.oldpeoplehome.entity.Parent;
import com.oldpeoplehome.service.ParentService;
import com.oldpeoplehome.utils.MethodUtil;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.lang.reflect.InvocationTargetException;
import java.util.List;
import java.util.Map;

/**
 * Created By Jiangyuwei on 2019/8/20 20:04
 * Description:
 */
@RestController
@RequestMapping("/parent")
public class ParentController {

    @Autowired
    private ParentService parentService;

    @RequestMapping("/get/{id}")
    @ResponseBody
    public Parent get(@PathVariable("id") String id){
        return parentService.findByID(Long.valueOf(id));
    }
    @RequestMapping("/get_longid/{longid}")
    @ResponseBody
    public Parent getLong(@PathVariable("longid") String id){
        return parentService.findByLongId(id);
    }
    @RequestMapping("/get_name/{name}")
    @ResponseBody
    public Parent getName(@PathVariable("name") String name){
        return parentService.findByName(name);
    }
    @RequestMapping("/get_room/{room}")
    @ResponseBody
    public Parent getRoom(@PathVariable("room") String room){
        return parentService.findByID(Integer.valueOf(room));
    }
    @RequestMapping("/list")
    @ResponseBody
    public List list() {
        return parentService.findAll();
    }
    @RequestMapping(value = "/add", method = RequestMethod.POST)
    @ResponseBody
    public Parent add(@RequestParam Map<String, Object> params){
        Parent parent = new Parent();
        System.out.println(params);
        parent.setParentSex(String.valueOf(params.get("parentSex")));
        parent.setParentPassword(String.valueOf(params.get("parentPassword")));
        parent.setParentAccount(String.valueOf(params.get("parentAccount")));
        parent.setParentName(String.valueOf(params.get("parentName")));
        parent.mySetParentBirth(String.valueOf(params.get("parentBirth")));
        parent.setParentLongId(String.valueOf(params.get("parentLongId")));
        parentService.insert(parent);
        return parentService.findByLongId(parent.getParentLongId());
    }
    @RequestMapping(value = "/update/{id}", method = RequestMethod.POST)
    @ResponseBody
    public Parent update(@RequestParam Map<String, Object> params, @PathVariable("id") String id) throws NoSuchMethodException, IllegalAccessException, InvocationTargetException {
        Parent parent = parentService.findByID(Long.valueOf(id));
        System.out.println(params);
        MethodUtil.updateFields(parent, params);
        parentService.update(parent);
        return parent;
    }

    @RequestMapping(value = "/delete/{id}", method = RequestMethod.DELETE)
    @ResponseBody
    public List<Parent> delete(@PathVariable("id") String id){
        parentService.delete(Long.valueOf(id));
        return parentService.findAll();
    }

    @RequestMapping(value = "/login", method = RequestMethod.POST)
    @ResponseBody
    public Parent login(@RequestParam Map<String, Object> params){
        Parent res = parentService.login(params.get("account").toString());
        return res.getParentPassword().equals(params.get("password").toString()) ? res:null;
    }
}
