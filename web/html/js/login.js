function login(){
    
    var account1 = $("#account").val()
    var password2 =  $('#password').val()
   
    $.ajax({
        type: "POST",
        url: "http://39.105.65.209:8080/admin/login",
        data: {
            account: account1,
            password: password2
        },
        success: function(datas){
             
            if(datas!=null){
                window.location.href="html/firstPage.html";
            }
        },
        error:function(){
            alert('请求失败，请重试')            
        }
    }); 
}