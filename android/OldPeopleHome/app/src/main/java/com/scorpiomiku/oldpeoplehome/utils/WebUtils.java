package com.scorpiomiku.oldpeoplehome.utils;

import com.google.gson.JsonArray;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;

import java.io.IOException;
import java.util.HashMap;
import java.util.Set;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.MediaType;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;

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

    public JsonObject getJsonObj(Response response) throws IOException {
        String result = response.body().string();
        JsonParser jsonParser = new JsonParser();
        JsonObject jsonObject = (JsonObject) jsonParser.parse(result);
        return jsonObject;
    }

    public JsonObject getJsonObj(String json) throws IOException {
        JsonParser jsonParser = new JsonParser();
        JsonObject jsonObject = (JsonObject) jsonParser.parse(json);
        return jsonObject;
    }

    public JsonArray getJsonArray(Response response) throws IOException {
        String result = response.body().string();
        JsonParser jsonParser = new JsonParser();
        JsonArray jsonObject = (JsonArray) jsonParser.parse(result);
        return jsonObject;
    }

    /**
     * ~上传运动信息
     *
     * @param data
     * @param callback
     */
    public void upSport(HashMap<String, String> data, Callback callback) {
        Request request = new Request.Builder().post(postRequestBody(data))
                .url(webHost + "/motion/add/").build();
        Call call = mClient.newCall(request);
        call.enqueue(callback);
    }

    /**
     * 获取运动数据
     *
     * @param parentId
     * @param callback
     */
    public void getSport(String parentId, Callback callback) {
        Request request = new Request.Builder()
                .url(webHost + "/motion/get/" + parentId).build();
        Call call = mClient.newCall(request);
        call.enqueue(callback);
    }

    /**
     * 上传心率等信息
     *
     * @param data
     * @param callback
     */
    public void upHeartRates(HashMap<String, String> data, Callback callback) {
        Request request = new Request.Builder().post(postRequestBody(data))
                .url(webHost + "/heartrate/add/").build();
        Call call = mClient.newCall(request);
        call.enqueue(callback);
    }

    /**
     * 获取房间数据
     *
     * @param roomId
     * @param callback
     */
    public void getRoomData(String roomId, Callback callback) {
        Request request = new Request.Builder()
                .url(webHost + "/rstate/get/" + roomId)
                .build();
        Call call = mClient.newCall(request);
        call.enqueue(callback);
    }

    /**
     * 获取天气信息
     *
     * @param callback
     */
    public void getWeather(Callback callback) {
        Request request = new Request.Builder()
                .url("https://api.caiyunapp.com/v2/TAkhjf8d1nlSlspN/112.4450,38.0134/realtime.jsonp")
                .build();
        Call call = mClient.newCall(request);
        call.enqueue(callback);
    }

    /**
     * 上传房间环境数据
     *
     * @param hashMap
     * @param callback
     */
    public void upRoomState(HashMap<String, String> hashMap, Callback callback) {
        Request request = new Request.Builder().post(getRequestBody(hashMap))
                .url(webHost + "/rstate/add/").build();
        Call call = mClient.newCall(request);
        call.enqueue(callback);
    }

    /**
     * 注册child
     *
     * @param hashMap
     * @param callback
     */
    public void registerChild(HashMap<String, String> hashMap, Callback callback) {
        Request request = new Request.Builder().post(getRequestBody(hashMap))
                .url(webHost + "/child/add/").build();
        Call call = mClient.newCall(request);
        call.enqueue(callback);
    }

    /**
     * child login
     *
     * @param hashMap
     * @param callback
     */
    public void loginChild(HashMap<String, String> hashMap, Callback callback) {
        Request request = new Request.Builder().post(getRequestBody(hashMap))
                .url(webHost + "/child/login/").build();
        Call call = mClient.newCall(request);
        call.enqueue(callback);
    }

    /**
     * 孩子绑定老人
     *
     * @param hashMap
     * @param callback
     */
    public void childBindOldPeople(HashMap<String, String> hashMap, Callback callback) {
        Request request = new Request.Builder().post(getRequestBody(hashMap))
                .url(webHost + "/child_parent/add/").build();
        Call call = mClient.newCall(request);
        call.enqueue(callback);
    }

    /**
     * 获得已绑定了的老人
     *
     * @param childId
     * @param callback
     */
    public void getBindOldPeople(String childId, Callback callback) {
//        LogUtils.loge(webHost + "/child_parent/child/" + childId);
        Request request = new Request.Builder()
                .url(webHost + "/child_parent/child/" + childId).build();
        Call call = mClient.newCall(request);
        call.enqueue(callback);
    }
}
