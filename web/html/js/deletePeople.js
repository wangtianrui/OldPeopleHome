function deleteData(){
    if(confirm('确定要注销ID为'+$('#pId').val()+'的老人吗？')){
        $.ajax({
            url: 'http://39.105.65.209:8080/parent/delete/'+$('#pId').val(),
            type: 'delete',
            success: function(data){
                alert('删除成功');
                window.location.href="firstPage.html";
            },
            error: function(){
                alert('请求失败，请重试')
            }
        })
    }
    else{

    }
}