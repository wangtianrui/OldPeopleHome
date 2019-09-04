package com.scorpiomiku.oldpeoplehome.utils;

import java.util.HashMap;
import java.util.Set;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.MediaType;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;

import static com.scorpiomiku.oldpeoplehome.utils.StaticUtils.webHost;

/**
 * Created by ScorpioMiku on 2019/9/4.
 */

public class WebUtils {
    private static WebUtils instance = new WebUtils();
    private static OkHttpClient mClient = new OkHttpClient();

    private WebUtils() {
    }

    public static WebUtils getInstance() {
        return instance;
    }


    private static RequestBody postRequestBody(HashMap<String, String> data) {
        String body = "";
        MediaType mediaType = MediaType.parse("application/x-www-form-urlencoded");
        Set<String> keys = data.keySet();
        for (String key : keys) {
            body += key + "=" + data.get(key) + "&";
        }
        body = body.substring(0, body.length() - 1);
        LogUtils.logd(body);
        RequestBody requestBody = RequestBody.create(mediaType, body);
        return requestBody;
    }

    private static RequestBody getRequestBody(HashMap<String, String> data) {
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

    /**
     * ~上传运动信息
     *
     * @param data
     * @param callback
     */
    public void UpSport(HashMap<String, String> data, Callback callback) {
        Request request = new Request.Builder().post(postRequestBody(data))
                .url(webHost + "/motion/add/").build();
        Call call = mClient.newCall(request);
        call.enqueue(callback);
    }
}
