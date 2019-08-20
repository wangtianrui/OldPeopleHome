package com.oldpeoplehome.utils;

/**
 * Created By Jiangyuwei on 2019/8/20 16:31
 * Description:
 */
public class StringUtil {

    public static String capital(String s){
        s = s.substring(0, 1).toUpperCase() + s.substring(1);
        return  s;
    }

}
