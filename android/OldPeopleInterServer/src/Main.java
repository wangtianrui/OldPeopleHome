import com.google.gson.Gson;
import com.google.gson.JsonArray;
import com.google.gson.JsonParser;
import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.Response;

import java.io.IOException;

public class Main {
    private static String isin;
    private static String temporature;
    private static String humidity;
    private static WebUtils webUtils;

    private static float slice;

    public static void main(String[] args) {
        webUtils = WebUtils.getInstance();
        System.out.println("开始转发任务");
        getAndUpData();
    }

    private static void fengshan() {
        webUtils.fengshan(8, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                System.out.println(e.getMessage());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {

            }
        });
    }

    private static void getAndUpData() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    Thread.sleep(2000);
                    webUtils.getRoomData(new Callback() {
                        @Override
                        public void onFailure(Call call, IOException e) {
                            System.out.println(e.getMessage());
                        }

                        @Override
                        public void onResponse(Call call, Response response) throws IOException {
                            JsonArray jsonElements = getJsonArray(response);
                            Gson gson = new Gson();
                            Node[] nodes = gson.fromJson(jsonElements, Node[].class);
                            for (Node node : nodes) {
                                System.out.println("接收到数据:" + node.toString());
                                funcList[] funcLists = node.getFuncList();
                                for (funcList funcList : funcLists) {
                                    if (funcList.getType().equals("安防")) {
                                        isin = String.valueOf((int) funcList.getData());
                                    } else if (funcList.getType().equals("温度")) {
                                        temporature = String.valueOf(funcList.getData());

                                    } else if (funcList.getType().equals("湿度")) {
                                        humidity = String.valueOf(funcList.getData());
                                    }
                                }
                            }
                            RoomData roomData = new RoomData(isin, temporature, humidity, TimeUtils.getTime(), "1");
                            System.out.println("解析出房间信息:" + roomData);
                            webUtils.upRoomState(roomData.getMap(), new Callback() {
                                @Override
                                public void onFailure(Call call, IOException e) {
                                    System.out.println(e.getMessage());
                                }

                                @Override
                                public void onResponse(Call call, Response response) throws IOException {
                                    System.out.println("房间信息上传成功");
                                    getAndUpData();
                                }
                            });
                        }
                    });
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }

    private static JsonArray getJsonArray(Response response) throws IOException {
        String result = response.body().string();
        JsonParser jsonParser = new JsonParser();
        JsonArray jsonObject = (JsonArray) jsonParser.parse(result);
        return jsonObject;
    }

}
