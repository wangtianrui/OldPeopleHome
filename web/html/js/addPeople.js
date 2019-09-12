function changeDiv() {
        register_1 = document.getElementById("register_1");
        register_2 = document.getElementById("register_2");
        if(register_1.style.display == "block"&&register_2.style.display == "none"){
          register_1.style.display = "none";
          register_2.style.display = "block";
          }
        }

function over(){        
    $.ajax({
        type: "POST",
        url: "http://39.105.65.209:8080/parent/add",
        data: {
            parentAccount: $("#pAccount").val(),
            parentPassword: $('#pPassword').val(),
            parentName: $('#name').val(),
            parentLongId: $('#cardId').val(),
            parentSex: $('input:radio:checked').val(),
            parentBirth: $('#date').val(),
            parentRoomId: $('#roomId').val()
        },
        success: function(data){
          alert('桥豆麻袋！')
          window.location.href="firstPage.html";
        },
        error:function(){
            alert('请求失败，请重试')
            if(register_1.style.display == "none"&&register_2.style.display == "block"){
              register_1.style.display = "block";
              register_2.style.display = "none";
              }
        }
    }); 
}