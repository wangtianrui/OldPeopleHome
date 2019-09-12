count = [
    document.getElementById('1'),
    document.getElementById('2'),
    document.getElementById('3'),
    document.getElementById('4'),
    document.getElementById('5'),
    document.getElementById('6'),
    document.getElementById('7'),
    document.getElementById('8'),
    document.getElementById('9'),
    document.getElementById('10')]
arrays = [
    document.getElementById('s1'),
    document.getElementById('s2'),
    document.getElementById('s3'),
    document.getElementById('s4'),
    document.getElementById('s5'),
    document.getElementById('s6'),
    document.getElementById('s7'),
    document.getElementById('s8'),
    document.getElementById('s9'),
    document.getElementById('s10')
]
infos = [
    document.getElementById('div1'),
    document.getElementById('div2'),
    document.getElementById('div3'),
    document.getElementById('div4'),
    document.getElementById('div5'),
    document.getElementById('div6'),
    document.getElementById('div7'),
    document.getElementById('div8'),
    document.getElementById('div9'),
    document.getElementById('div10')
]
$.ajax({
    url:'http://39.105.65.209:8080/parent/list',
    type:'get',
    success:function (data) {
        $.each(data,function(i,n){
            count[i].style.display = "block";
            $(arrays[i]).html(n.parentName);
            $(infos[i]).html(n.parentId).hide();
        });
    },
    error: function(){
        alert('error')
    }    
});

function info1(){
    location.replace('views.html?parentId='+$('#div1').text());
    // alert($('#div1').text());
}
function info2(){
    location.replace('views.html?parentId='+$('#div2').text());
    // alert($('#div2').text());
}
function info3(){
    location.replace('views.html?parentId='+$('#div3').text());
    // alert($('#div3').text());
}
function info4(){
    location.replace('views.html?parentId='+$('#div4').text());
    // alert($('#div4').text());
}
function info5(){
    location.replace('views.html?parentId='+$('#div5').text());
    // alert($('#div5').text());
}
function info6(){
    location.replace('views.html?parentId='+$('#div6').text());
    // alert($('#div6').text());
}
function info7(){
    location.replace('views.html?parentId='+$('#div7').text());
    // alert($('#div7').text());
}
function info8(){
    location.replace('views.html?parentId='+$('#div8').text());
    // alert($('#div8').text());
}
function info9(){
    location.replace('views.html?parentId='+$('#div9').text());
    // alert($('#div9').text());
}
function info10(){
    location.replace('views.html?parentId='+$('#div10').text());
    // alert($('#div10').text());
}