function login(){
    $.ajax({
        url:'http://39.105.65.209:8080/admin/login',
        type:'POST',
        data:[{"account": "root"},{"password": "root"}],
        success:function (data) {
            // if(data){
            //     var jsonArray =typeof data=='string' ?JSON.parse(data):data;
            // }
            // console.log(jsonArray);
            // $.each(jsonArray, function(i, n){
                
            //      console.log(jsonArray[i])
            // });
           
            // $('body').html(jsonArray[0]);
            if(account=='root'&&password=='root')
            $.open('html/firstPage.html');
        }
    });
}