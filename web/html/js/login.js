var http = require('http');

// 用于请求的选项
var options = {
    host: '39.105.65.209',
    port: '8080',
    path: '/login.html'
};

// 处理响应的回调函数
var callback = function (response) {
    // 不断更新数据
    var body = '';
    response.on('data',function (data) {
        body +=data;
    });

    response.on('end',function () {
        // 数据接收完成
        console.log(body);
    });
}

// 向服务器端发送请求
var request = http.request(options,callback);
request.end();