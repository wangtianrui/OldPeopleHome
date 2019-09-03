package com.oldpeoplehome.web;

import com.oldpeoplehome.entity.Admin;
import com.oldpeoplehome.service.AdminService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.util.Map;

/**
 * Created By Jiangyuwei on 2019/8/30 11:57
 * Description:
 */
@RestController
@RequestMapping("/admin")
public class AdminController {

    @Autowired
    private AdminService adminService;

    @RequestMapping(value = "/login",method = RequestMethod.POST)
    @ResponseBody
    public Admin login(@RequestParam Map<String, Object> params){
        Admin res = adminService.login(params.get("account").toString());
        return res.getAdminPassword().equals(params.get("password").toString()) ? res:null;
    }

}
