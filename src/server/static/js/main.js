$(document).ready(function() {
  // setup all the button listener events
  $("#pump-on-btn").on("click", function() {
    $.post("http://608dev.net/sandbox/sc/mattfeng/finalproject/server/control/override/pump.py", {"override": "ON"})
  });
  $("#pump-off-btn").on("click", function() {
    $.post("http://608dev.net/sandbox/sc/mattfeng/finalproject/server/control/override/pump.py", {"override": "OFF"})
  });
  $("#pump-none-btn").on("click", function() {
    $.post("http://608dev.net/sandbox/sc/mattfeng/finalproject/server/control/override/pump.py", {"override": "NONE"})
  });
  $("#lamp-on-btn").on("click", function() {
    $.post("http://608dev.net/sandbox/sc/mattfeng/finalproject/server/control/override/lamp.py", {"override": "ON"})
  });
  $("#lamp-off-btn").on("click", function() {
    $.post("http://608dev.net/sandbox/sc/mattfeng/finalproject/server/control/override/lamp.py", {"override": "OFF"})
  });
  $("#lamp-none-btn").on("click", function() {
    $.post("http://608dev.net/sandbox/sc/mattfeng/finalproject/server/control/override/lamp.py", {"override": "NONE"})
  });

  // get sensor readings
  setInterval(function() {
    $.getJSON("http://608dev.net/sandbox/sc/mattfeng/finalproject/server/sensors/read.py", function(data) {
      $("#temperature-reading").text(data.temp)
      $("#humidity-reading").text(data.humid)
      $("#moisture-reading").text(data.moist)
    })
  }, 500)

  // get current state of pump
  setInterval(function() {
    $.getJSON("http://608dev.net/sandbox/sc/mattfeng/finalproject/server/status/pump.py", function(data) {
      $("#pump-status").text(data.status)
    })
  }, 500)

  // get current state of lamp
  setInterval(function() {
    $.getJSON("http://608dev.net/sandbox/sc/mattfeng/finalproject/server/status/lamp.py", function(data) {
      $("#lamp-status").text(data.status)
    })
  }, 500)

  // handle lamp override
  setInterval(function() {
    $.getJSON("http://608dev.net/sandbox/sc/mattfeng/finalproject/server/control/override/current/lamp.py", function(data) {
      if (data.override == "ON") {
        $("#lamp-on").attr("checked", "checked")
        $("#lamp-off").removeAttr("checked")
        $("#lamp-none").removeAttr("checked")
        $("#lamp-on-btn").addClass("active")
        $("#lamp-off-btn").removeClass("active")
        $("#lamp-none-btn").removeClass("active")
      } else if (data.override == "OFF") {
        $("#lamp-off").attr("checked", "checked")
        $("#lamp-on").removeAttr("checked")
        $("#lamp-none").removeAttr("checked")
        $("#lamp-off-btn").addClass("active")
        $("#lamp-on-btn").removeClass("active")
        $("#lamp-none-btn").removeClass("active")
      } else {
        $("#lamp-none").attr("checked", "checked")
        $("#lamp-off").removeAttr("checked")
        $("#lamp-on").removeAttr("checked")
        $("#lamp-none-btn").addClass("active")
        $("#lamp-off-btn").removeClass("active")
        $("#lamp-on-btn").removeClass("active")
      }
    })
  }, 500)

  // handle pump override
  setInterval(function() {
    $.getJSON("http://608dev.net/sandbox/sc/mattfeng/finalproject/server/control/override/current/pump.py", function(data) {
      if (data.override == "ON") {
        $("#pump-on").attr("checked", "checked")
        $("#pump-off").removeAttr("checked")
        $("#pump-none").removeAttr("checked")
        $("#pump-on-btn").addClass("active")
        $("#pump-off-btn").removeClass("active")
        $("#pump-none-btn").removeClass("active")
      } else if (data.override == "OFF") {
        $("#pump-off").attr("checked", "checked")
        $("#pump-on").removeAttr("checked")
        $("#pump-none").removeAttr("checked")
        $("#pump-off-btn").addClass("active")
        $("#pump-on-btn").removeClass("active")
        $("#pump-none-btn").removeClass("active")
      } else {
        $("#pump-none").attr("checked", "checked")
        $("#pump-off").removeAttr("checked")
        $("#pump-on").removeAttr("checked")
        $("#pump-none-btn").addClass("active")
        $("#pump-off-btn").removeClass("active")
        $("#pump-on-btn").removeClass("active")
      }
    })

  }, 500)

  // handle intent for lamp
  setInterval(function() {
    $.getJSON("http://608dev.net/sandbox/sc/mattfeng/finalproject/server/control/lamp.py", function(data) {
      $("#lamp-intent").text((data.intent == "TRUE") ? "ON" : "OFF")
    })
  }, 500)


  // handle intent for pump
  setInterval(function() {
    $.getJSON("http://608dev.net/sandbox/sc/mattfeng/finalproject/server/control/pump.py", function(data) {
      $("#pump-intent").text((data.intent == "TRUE") ? "ON" : "OFF")
    })
  }, 500)

  // handle camera stream
  $.getJSON("http://608dev.net/sandbox/sc/mattfeng/finalproject/server/camera/read.py", function(data) {
    if (data.success) {
      $("#camera").html("<img src='http://" + data.address + "/stream' />")
    } else {
      $("#camera").html("<p>No camera source found...</p>")
    }
  })
  
})