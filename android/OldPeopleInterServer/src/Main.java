import com.google.gson.Gson;
import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.Response;

import java.io.IOException;

public class Main {
    private static String isin;
    private static String temporature;
    private static String humidity;
    private static WebUtils webUtils;

    public static void main(String[] args) {
        webUtils = WebUtils.getInstance();
        System.out.println("开始转发任务");
        getAndUpData();
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
                            String body = response.body().string();
                            Gson gson = new Gson();
                            Node[] nodes = gson.fromJson(body, Node[].class);
                            for (Node node : nodes) {
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
                            webUtils.upRoomState(roomData.getMap(), new Callback() {
                                @Override
                                public void onFailure(Call call, IOException e) {
                                    System.out.println(e.getMessage());
                                }

                                @Override
                                public void onResponse(Call call, Response response) throws IOException {
                                    System.out.println("上传成功:" + roomData.toString());
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
}
