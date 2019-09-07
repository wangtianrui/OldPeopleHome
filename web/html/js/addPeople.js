function changeDiv() {
  register_1 = document.getElementById("register_1")
  register_2 = document.getElementById("register_2")
  if(register_1.style.display === "block"&&register_2.style.display === "none"){
    register_1.style.display = "none"
    register_2.style.display = "block"
    console.log(register_1.style.display)
    }
    else if(register_1.style.display === "none"&&register_2.style.display === "block") {
      register_1.style.display = "block"
      register_2.style.display = "none"
    }
}