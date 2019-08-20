package com.oldpeoplehome.utils;

/**
 * Created By Jiangyuwei on 2019/8/20 10:53
 * Description:
 */
public class Result<T> {

    private static String NULL = "[]";

    private boolean success;
    private T data;
    private String error;
    public Result(){}

    public Result(boolean success, T data) {
        this.success = success;
        this.data = data;
        this.error = NULL;
    }

    public Result(boolean success, String error) {
        this.success = success;
        this.error = error;
    }

    @Override
    public String toString() {
        return "{" +
                "success=" + success +
                ", data=" + data +
                ", error='" + error + '\'' +
                '}';
    }
}
