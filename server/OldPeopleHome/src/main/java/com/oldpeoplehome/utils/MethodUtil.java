package com.oldpeoplehome.utils;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Map;

/**
 * Created By Jiangyuwei on 2019/8/20 20:59
 * Description:
 */
public class MethodUtil {

    public static void updateFields(Object obj, Map<String, Object> params)throws NoSuchMethodException, IllegalAccessException, InvocationTargetException {
        Class<?> pClass = obj.getClass();
        for (Map.Entry<String, Object> entry: params.entrySet()){
            Method method = pClass.getMethod("mySet" + StringUtil.capital(entry.getKey()), entry.getValue().getClass());
            method.invoke(obj, entry.getValue());
        }
    }


}
