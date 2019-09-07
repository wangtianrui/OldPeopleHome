package com.scorpiomiku.wifitrick;

import android.util.Log;

import com.google.gson.JsonObject;
import com.google.gson.JsonParser;

import java.io.IOException;
import java.util.HashMap;
import java.util.Set;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.FormBody;
import okhttp3.MediaType;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;

/**
 * Created by ScorpioMiku on 2019/9/3.
 */

public class WebUtils {
    private static final String TAG = "WebUtils";
    private static WebUtils instance = new WebUtils();
    private static OkHttpClient mClient = new OkHttpClient();
    private static final String webHost = "http://39.105.65.209:8080";

    private WebUtils() {
    }

    public static WebUtils getInstance() {
        return instance;
    }

    public static RequestBody getRequestBody(HashMap<String, String> data) {
        String body = "";
        MediaType mediaType = MediaType.parse("application/x-www-form-urlencoded");
        Set<String> keys = data.keySet();
        for (String key : keys) {
            body += key + "=" + data.get(key) + "&";
        }
        body = body.substring(0, body.length() - 1);
        RequestBody requestBody = RequestBody.create(mediaType, body);
        return requestBody;
    }

    public static void upRoomState(HashMap<String, String> hashMap, Callback callback) {
        Request request = new Request.Builder().post(getRequestBody(hashMap))
                .url(webHost + "/rstate/add/").build();
        Call call = mClient.newCall(request);
        call.enqueue(callback);
    }

    public static void getTest(Callback callback) {
        Request request = new Request.Builder()
                .url("http://192.168.0.1/cgi-bin/node.cgi")
                .build();
        Call call = mClient.newCall(request);
        call.enqueue(callback);
    }


}
