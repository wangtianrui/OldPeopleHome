// 步数折线图
var dom = document.getElementById("walk");
var myChart = echarts.init(dom);
var app = {};
option = null;
option = {
    title: {
        text: '本周步数'
        },
        // hover效果
    tooltip: {
        trigger: 'axis'
        },
    xAxis: {
        type: 'category',
        data: ['周一', '周二', '周三', '周四', '周五', '周六', '周日']
    },
    yAxis: {
        type: 'value'
    },
    series: [{
        data: [1820, 2932, 1901, 3934, 3290, 1330, 2320],
        type: 'line'
    }]
};
;
if (option && typeof option === "object") {
    myChart.setOption(option, true);
}

// 睡眠饼状图
var dom = document.getElementById("sleep");
var myChart = echarts.init(dom);
var app = {};
option = null;
app.title = '环形图';

option = {
    title: {
        text: '睡眠情况'
    },
    // hover效果
    tooltip: {
        trigger: 'item',
        formatter: "{a} <br/>{b}: {c} ({d}%)"
    },
    legend: {
        orient: 'vertical',
        x: 'right',
        data:['深睡眠','浅睡眠','清醒']
    },
    series: [
        {
            name:'睡眠情况',
            type:'pie',
            radius: ['50%', '70%'],
            avoidLabelOverlap: false,
            label: {
                normal: {
                    show: false,
                    position: 'center'
                },
                emphasis: {
                    show: true,
                    textStyle: {
                        fontSize: '30',
                        fontWeight: 'bold'
                    }
                }
            },
            labelLine: {
                normal: {
                    show: false
                }
            },
            data:[
                {value:5, name:'深睡眠'},
                {value:3, name:'浅睡眠'},
                {value:1, name:'清醒'},
            ]
        }
    ]
};
;
if (option && typeof option === "object") {
    myChart.setOption(option, true);
}

// 心率折线图
var dom = document.getElementById("heart");
var myChart = echarts.init(dom);
var app = {};
option = null;
option = {
    title: {
        text: '心率折线图'
        },
        // hover效果
    tooltip: {
        trigger: 'axis'
        },
    xAxis: {
        type: 'category',
        data: ['06:00', '07:00', '08:00', '09:00', '10:00', '11:00', '12:00', '13:00', '14:00', '15:00', '16:00', '17:00', '18:00', '19:00', '20:00', '21:00', '22:00', '23:00', '24:00', '01:00', '02:00', '03:00', '04:00', '05:00']
    },
    yAxis: {
        type: 'value'
    },
    series: [{
        data: [76, 56, 68, 79, 67, 76, 57, 68, 86, 68, 78, 76, 58, 97, 57, 76, 78, 87, 67, 77, 58, 95, 59, 86],
        type: 'line'
    }]
};
;
if (option && typeof option === "object") {
    myChart.setOption(option, true);
}

// 行动轨迹
var map = new BMap.Map("track");          // 创建地图实例  
var point = new BMap.Point(112.44183, 38.01419);  // 创建点坐标  
map.centerAndZoom(point, 15);                 // 初始化地图，设置中心点坐标和地图级别  
map.enableScrollWheelZoom(true);     //开启鼠标滚轮缩放

var geolocation = new BMap.Geolocation();
// 开启SDK辅助定位
geolocation.enableSDKLocation();
geolocation.getCurrentPosition(function(r){
	if(this.getStatus() == BMAP_STATUS_SUCCESS){
		var mk = new BMap.Marker(r.point);
		map.addOverlay(mk);
		map.panTo(r.point);
		alert('您的位置：'+r.point.lng+','+r.point.lat);
	}
	else {
		alert('failed'+this.getStatus());
	}        
});