// 解析firstPage传过来的参数

                                    //search获得地址中的参数，内容为'?itemId=12'
var paras = location.search;			
                                    //match是字符串中符合的字段一个一个取出来，result中的值为['login=xx','table=admin']
var result = paras.match(/[^\?&]*=[^&]*/g); 	
                                    //让paras变成没有内容的json对象
paras = {};					
for(i in result){
                                    //split()将一个字符串分解成一个数组,两次遍历result中的值分别为['itemId','xx']
	var temp = result[i].split('=');	
	paras[temp[0]] = temp[1];
}
                                    //根据参数名"itemId"，获取参数值
var parentId1 = paras.parentId;     
console.log(parentId1);


setInterval(disp,1);
function disp(){

    $.ajax({
        url:'http://39.105.65.209:8080/parent/get/'+parentId1,
        type:'get',
        success:function (data) {
            $("#pName").html(data.parentName)
            $('#idCard').html(data.parentLongId)
            $('#roomName').html(data.parentRoom)
            $('#pGender').html(data.parentSex)
            $('#pBirth').html(data.parentBirth)
            $('#pAccount').html(data.parentAccount)
            $('#pPassword').html(data.parentPassword)
            $('#pPhone').html(data.parentPhone)
            $('#pHeight').html(data.parentHeight)
            $('#pWeight').html(data.parentWeight)

            $.ajax({
                url: 'http://39.105.65.209:8080/child_parent/parent/'+parentId1,
                type: 'get',
                success: function(data2){
                    $.each(data2,function(i,n){
                        $('#cName').html(n.childName)
                        $('#cGender').html(n.childSex)
                        $('#cPhone').html(n.childPhone)
                    })
                    
                    
                    // 步数折线图初始化
                    var walkTime = [];
                    var walkCount = [];
                    var cc = 0;
                    // 睡眠饼状图初始化
                    var date = new Date(); 
                    var year = date.getFullYear();
                    var month = date.getMonth()+1;
                    var day = date.getDate();
                    var hour = date.getHours();
                    var minute = date.getMinutes();
                    var second = date.getSeconds();
                    var timetime = hour+':'+minute+':'+second;
                    if(parseFloat(minute)>10){
                        var pastMinute = parseFloat(minute)-10;
                    }else{
                        var pastMinute = 60-10+parseFloat(minute)
                    }
                    var pastTime = hour+':'+pastMinute+':'+second;
                    
                    // console.log(timetime)
                    if (month >= 1 && month <= 9) {
                        month = "0" + month;
                    }
                    if (day >= 0 && day <= 9) {
                        day = "0" + day;
                    }
                    var today = year+'-'+month+'-'+day;
                    var lightSleep = [];
                    var deepSleep = [];
                    var awakeSleep = [];
                    var dd = 0;
                    // 心率折线图初始化
                    var heartTime = [];
                    var heartData = [];
                    var ee = 0;

                    // 房间实况图初始化
                    var roomTemp = [];
                    var roomWet = [];
                    var roomIn = [];
                    var roomTime = [];
                    var ff = 0;

                    // 血压折线图初始化
                    var bloodTime = [];
                    var bloodRate1 = [];
                    var bloodRate2 = [];
                    var bloodOxy = [];
                    var gg = 0;

                    // 轨迹图初始化
                    var inTimex = [];
                    var inTimey = [];
                    var hh = 0;
                    var names = [];
                    var inTime = [];
                    

                    

                        // console.log('刷刷刷')
                        // console.log(heartData)
                        
                        // 步数折线图
                        $.ajax({
                            url: 'http://39.105.65.209:8080/motion/get/'+parentId1,
                            type: 'get',
                            success: function(count){
                                $.each(count, function(i){
                                    cc++
                                    if(cc <= count.length ){
                                        if (count[i].motionDate == today){
                                            var dom = document.getElementById("walk");
                                            var myChart = echarts.init(dom);
                                            option = null;
                                            // newData = newData + count[i].motionCount
                                            walkTime.push(count[i].motionTime)                                      
                                            walkCount.push(count[i].motionCount)
                                            option = {
                                                title: {
                                                    text: '今日步数'
                                                    },
                                                    // hover效果
                                                tooltip: {
                                                    trigger: 'axis'
                                                    },
                                                xAxis: {
                                                    type: 'category',
                                                    data: walkTime
                                                },
                                                yAxis: {
                                                    type: 'value'
                                                },
                                                series: {
                                                    data: walkCount,
                                                    type: 'line'
                                                }
                                            };

                                            // console.log(count[i].motionDate)
                                            ;
                                            if (option && typeof option === "object") {
                                                myChart.setOption(option, true);
                                            }
                                        }
                                       
                                        
                                    }
                                    else {
                                        return;
                                    }
                                    
                                })
                                
                            }
                            
                        })
                        

                        // 睡眠饼状图
                        $.ajax({
                            url: 'http://39.105.65.209:8080/sleep/get/'+parentId1,
                            type: 'get',
                            success: function(sleep){
                                $.each(sleep, function(i){
                                    dd++
                                    if(dd <= sleep.length){
                                        // console.log(sleep[i].date)
                                        if(sleep[i].date == today){

                                            var dom = document.getElementById("sleep");
                                            var myChart = echarts.init(dom);
                                            var app = {};
                                            option = null;
                                            app.title = '环形图';


                                            lightSleep = parseFloat(sleep[i].lightTime) 
                                            deepSleep = parseFloat(sleep[i].deepTime) 
                                            awakeSleep = parseFloat(sleep[i].awakeTime) 
                                            

                                            // console.log(lightSleep)
                                            // console.log(deepSleep)
                                            // console.log(awakeSleep)
                                            // console.log(typeof(lightSleep))

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
                                                        data: [
                                                            {value: deepSleep, name: '深睡眠'},
                                                            {value: lightSleep, name: '浅睡眠'},
                                                            {value: awakeSleep, name: '清醒'}
                                                        ]
                                                    }
                                                ]
                                            };
                                            ;
                                            if (option && typeof option === "object") {
                                                myChart.setOption(option, true);
                                            }
                                        }
                                        
                                    }
                                })
                            
                            }
                        })

                        // 心率折线图
                        $.ajax({
                            url: 'http://39.105.65.209:8080/heartrate/get/'+parentId1,
                            type: 'get',
                            success: function(heart){
                                $.each(heart, function(i){
                                    ee++
                                    if(ee <= heart.length){
                                       s = heart[i].time;
                                       newS = s.split(" ");
                                        if(newS[0] == today){
                                            // console.log('aaaaaaaaaaaaa')
                                            var dom = document.getElementById("heart");
                                            var myChart = echarts.init(dom);
                                            var app = {};
                                            option = null;
                                            heartTime.push(newS[1])
                                            heartData.push(heart[i].rate)
                                            // console.log(newS[1])
                                            // console.log(heart[i].rate)
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
                                                    data: heartTime
                                                },
                                                yAxis: {
                                                    type: 'value'
                                                },
                                                series: [{
                                                    data: heartData,
                                                    type: 'line'
                                                }]
                                            };
                                            ;
                                            if (option && typeof option === "object") {
                                                myChart.setOption(option, true);
                                            }
                                        }
                                        
                                    }
                                    
                                })
                              
                            }
                        })


                        // 房间实时状态折线图
                        $.ajax({
                            url: 'http://39.105.65.209:8080/rstate/get/1?start='+today+' '+pastTime+'&end='+today+' '+timetime,
                            type: 'get',
                            success: function(roomS){
                                $.each(roomS, function(i){
                                    // console.log('aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa')

                                    ff++
                                    if(ff <= roomS.length){
                                        str = roomS[i].time;
                                       newStr = str.split(" ");
                                    //    console.log(newStr[1])
                                       if(newStr[0] == today){
                                            var dom = document.getElementById("roomState");
                                            var myChart = echarts.init(dom);
                                            var app = {};
                                            roomTime.push(newStr[1])
                                            roomTemp.push(parseFloat(roomS[i].temperature) )
                                            roomWet.push(parseFloat(roomS[i].humidity) )
                                            roomIn.push(parseFloat(roomS[i].isin) )

                                            // console.log(newStr[1])
                                            option = null;
                                            option = {
                                                title: {
                                                    text: '房间实况'
                                                },
                                                tooltip: {
                                                    trigger: 'axis'
                                                },
                                                legend: {
                                                    data:['温度','湿度','是否有人']
                                                },
                                                xAxis: {
                                                    type: 'category',
                                                    boundaryGap: false,
                                                    data: roomTime
                                                },
                                                yAxis: {
                                                    type: 'value'
                                                },
                                                series: [
                                                    {
                                                        name:'温度',
                                                        type:'line',
                                                        data: roomTemp
                                                    },
                                                    {
                                                        name:'湿度',
                                                        type:'line',
                                                        data: roomWet
                                                    },
                                                    {
                                                        name:'是否有人',
                                                        type:'line',
                                                        data: roomIn
                                                    }
                                                ]
                                            };
                                            ;
                                            if (option && typeof option === "object") {
                                                myChart.setOption(option, true);
                                            }
                                       }
                                    }
                                })
                            }
                        })
                        

                        // 血压折线图
                        $.ajax({
                            url: 'http://39.105.65.209:8080/heartrate/get/'+parentId1,
                            type: 'get',
                            success: function(bloodData){
                                $.each(bloodData, function(i){
                                    gg++
                                    if(gg <= bloodData.length){
                                        strb = bloodData[i].time;
                                       newStrb = strb.split(" ");
                                    //    console.log(newStr[1])
                                       if(newStrb[0] == today){
                                            var dom = document.getElementById("blood");
                                            var myChart = echarts.init(dom);
                                            var app = {};
                                            bloodTime.push(newStrb[1])
                                            bloodRate1.push(parseFloat(bloodData[i].rate1) )
                                            bloodRate2.push(parseFloat(bloodData[i].rate2) )
                                            bloodOxy.push(parseFloat(bloodData[i].oxy) )

                                            // console.log(newStr[1])
                                            option = null;
                                            option = {
                                                title: {
                                                    text: '今日血压'
                                                },
                                                tooltip: {
                                                    trigger: 'axis'
                                                },
                                                legend: {
                                                    data:['收缩压','舒张压','血液含氧量']
                                                },
                                                xAxis: {
                                                    type: 'category',
                                                    boundaryGap: false,
                                                    data: bloodTime
                                                },
                                                yAxis: {
                                                    type: 'value'
                                                },
                                                series: [
                                                    {
                                                        name:'收缩压',
                                                        type:'line',
                                                        data: bloodRate1
                                                    },
                                                    {
                                                        name:'舒张压',
                                                        type:'line',
                                                        data: bloodRate2
                                                    },
                                                    {
                                                        name:'血液含氧量',
                                                        type:'line',
                                                        data: bloodOxy
                                                    }
                                                ]
                                            };
                                            ;
                                            if (option && typeof option === "object") {
                                                myChart.setOption(option, true);
                                            }
                                       }
                                    }
                                })
                            }
                        })
                    
                    
                        // 轨迹图

                        $.ajax({
                        url: 'http://39.105.65.209:8080/location/get/'+parentId1,
                        type: 'get',
                        success: function(trackData){
                            $.each(trackData, function(i){
                                hh++
                                names = hh
                                //  console.log(names)
                                if(hh <= trackData.length){
                                    
                                            strt = trackData[i].time;
                                            newStrt = strt.split(" ");
                                            if(newStrt[0] == today){
                                                var map = new BMap.Map("track");          // 创建地图实例  
                                                var point = new BMap.Point(112.44183, 38.01419);  // 创建点坐标  
                                                map.centerAndZoom(point, 15);                 // 初始化地图，设置中心点坐标和地图级别  

                                                trackData[i].longitude
                                                var polyline = new BMap.Polyline([
                                            new BMap.Point(parseFloat(trackData[i].longitude) , parseFloat( trackData[i].latitude)),
                                            // new BMap.Point(112.452, 38.014)
                                            ],
                                        {strokeColor:"blue", strokeWeight:6, strokeOpacity:0.5}
                                        );
                                                var polyline = new BMap.Polyline([
                                                    new BMap.Point(112.444, 38.0116),
                                                    new BMap.Point(112.455, 38.0107),
                                                    new BMap.Point(112.455, 38.0107),
                                                    new BMap.Point(112.455, 38.0107),
                                                    new BMap.Point(112.452,38.0128),
                                                    new BMap.Point(112.444, 38.0116)
                                                    ],
                                                    
                                                    {strokeColor:"blue", strokeWeight:6, strokeOpacity:0.5}
                                                    );
                                                map.addOverlay(polyline);
                                                map.enableScrollWheelZoom(true);     //开启鼠标滚轮缩放
                                       
                                    }
                                }
                            })
                        }
                    })    
                            
                        
                  

                    
                }
            });    
        },
        error: function(){
            alert('遇到错误了啊喂')
        }
    });
}


// 修改资料卡
function modify(){
    // $('<input>').replaceAll("#pNme")
    // alert('修改修改')
    // $.ajax({
    //     url: 'http://39.105.65.209:8080/parent/update/'+parentId1,
    //     type: 'post',
    //     data: {
    //         // parentName: $("#pName").val()
    //     },
    // })
}