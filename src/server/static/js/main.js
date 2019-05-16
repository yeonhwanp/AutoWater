var tempChart
var moistureChart
var humidityChart
var MOISTURE_FG_COLOR = "#DEEFB7"
var MOISTURE_BG_COLOR = "#DEEFB710"
var TEMPERATURE_FG_COLOR = "#98DFAF"
var TEMPERATURE_BG_COLOR = "#98DFAF10"
var HUMIDITY_FG_COLOR = "#5FB49C"
var HUMIDITY_BG_COLOR = "#5FB49C10"
var PUMP_SCHEDULE_UPDATE_URL = "http://608dev.net/sandbox/sc/mattfeng/finalproject/server/schedule/update/pump.py"
var LAMP_SCHEDULE_UPDATE_URL = "http://608dev.net/sandbox/sc/mattfeng/finalproject/server/schedule/update/lamp.py"
var MOISTURE_MIN_URL = "http://608dev.net/sandbox/sc/mattfeng/finalproject/server/control/override/moisture/min.py"
var MOISTURE_MAX_URL = "http://608dev.net/sandbox/sc/mattfeng/finalproject/server/control/override/moisture/max.py"
var MOISTURE_BOUNDS_URL = "http://608dev.net/sandbox/sc/mattfeng/finalproject/server/control/override/moisture/read.py"

var pumpSchedule
var lampSchedule

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
      setData(tempChart, data.temps, "Temperature", TEMPERATURE_BG_COLOR, TEMPERATURE_FG_COLOR)
      setData(moistureChart, data.moists, "Soil Moisture", MOISTURE_BG_COLOR, MOISTURE_FG_COLOR)
      setData(humidityChart, data.humids, "Humidity", HUMIDITY_BG_COLOR, HUMIDITY_FG_COLOR)
    })
  }, 3000)

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


  // handle updating the schedule for the pump
  setInterval(function() {
    $.getJSON("http://608dev.net/sandbox/sc/mattfeng/finalproject/server/schedule/pump.py", function(data) {
      pumpSchedule = data.schedule
      updatePumpClient()
    })
  }, 1000)

  // handle updating the schedule for the lamp
  setInterval(function() {
    $.getJSON("http://608dev.net/sandbox/sc/mattfeng/finalproject/server/schedule/lamp.py", function(data) {
      lampSchedule = data.schedule
      updateLampClient()
    })
  }, 1000)

  // handle camera stream
  $.getJSON("http://608dev.net/sandbox/sc/mattfeng/finalproject/server/camera/read.py", function(data) {
    if (data.success) {
      $("#camera").html("<img src='http://" + data.address + "/stream' />")
    } else {
      $("#camera").html("<p>No camera source found...</p>")
    }
  })


  function setData(chart, data, labelText, bg, fg) {
    var newData = data.map((point) => {
      return {
        t: Date.parse(point.t),
        y: point.y
      }
    })

    chart.data.datasets = [{
      label: labelText,
      data: newData,
      backgroundColor: [bg],
      borderColor: [fg],
      pointBackgroundColor: "rgba(0, 0, 0, 0)",
      pointBorderColor: "rgba(0, 0, 0, 0)"
    }]

    chart.update()
  }

  function setupChart(id, bg, fg) {
    // setup charts
    var ctx = $(id)
    ctx.height = 300
    var chart = new Chart(ctx, {
      type: "line",
      data: {
        datasets: [{
          data: []
        }]
      },
      options: {
        scales: {
          xAxes: [{
            type: "time",
            distribution: "linear"
          }],
          yAxes: [{
            ticks: {
              max: 100,
              min: 0,
              stepSize: 10
            }
          }]
        },
        animation: {
          duration: 0
        },
        maintainAspectRatio: false,
      }
    })

    return chart
  }

  function scheduleToString(schedule) {
    let ret = []
    for (let i = 0; i < schedule.length; i++) {
      ret.push(schedule[i].start + ":" + schedule[i].end)
    }
    ret = ret.join(",")
    console.log(ret)
    return ret
  }

  function timeToSeconds(time) {
    let timeSplit = time.split(":")
    return parseInt(timeSplit[0], 10) * 3600 +  parseInt(timeSplit[1], 10) * 60
  }

  function secondsToTime(seconds) {
    let hours = Math.floor(seconds / 3600)
    let minutes = Math.floor(seconds % 3600 / 60)
    return `${hours}`.padStart(2, "0") + ":" +`${minutes}`.padStart(2, "0")
  }

  function addPumpScheduleItem() {
    let startTime = timeToSeconds($("#pump-time-start").val())
    let endTime = startTime +  parseInt($("#pump-time-duration").val(), 10)

    pumpSchedule.push({
      start: startTime,
      end: endTime
    })

    updateScheduleServer(PUMP_SCHEDULE_UPDATE_URL, scheduleToString(pumpSchedule))
    updatePumpClient()

    $("#add-pump-time-modal").modal("hide")
  }

  function addLampScheduleItem() {
    let startTime = timeToSeconds($("#lamp-time-start").val())
    let endTime = timeToSeconds($("#lamp-time-end").val())

    lampSchedule.push({
      start: startTime,
      end: endTime
    })

    updateScheduleServer(LAMP_SCHEDULE_UPDATE_URL, scheduleToString(lampSchedule))
    updateLampClient()

    $("#add-lamp-time-modal").modal("hide")
  }

  function removePumpScheduleItem(start, end) {
    pumpSchedule = pumpSchedule.filter(item => item.start != start || item.end != end)
    updateScheduleServer(PUMP_SCHEDULE_UPDATE_URL, scheduleToString(pumpSchedule))
    updatePumpClient()
  }

  function removeLampScheduleItem(start, end) {
    lampSchedule = lampSchedule.filter(item => item.start != start || item.end != end)
    updateScheduleServer(LAMP_SCHEDULE_UPDATE_URL, scheduleToString(lampSchedule))
    updateLampClient()
  }

  function schedulePumpItem(start, end) {
    let schedItem = $(`<li class="schedule-item"></li>`)
    let infoParagraph = $(`
      <p>
        Start at <b>${secondsToTime(start)}</b> and run for <b>${end - start} seconds</b>
      </p>
    `)
    let deleteBtn = $(`
      <button type="button" class="btn btn-outline-danger">
        <i class="far fa-trash-alt"></i>
        Delete
      </button>
    `)
    deleteBtn.on("click", function() {
      removePumpScheduleItem(start, end)
    })
    schedItem.append(infoParagraph)
    schedItem.append(deleteBtn)
    return schedItem
  }

  function scheduleLampItem(start, end) {
    let schedItem = $(`<li class="schedule-item"></li>`)
    let infoParagraph = $(`
      <p>
        Start at <b>${secondsToTime(start)}</b> and end at <b>${secondsToTime(end)}</b>
      </p>
    `)
    let deleteBtn = $(`
      <button type="button" class="btn btn-outline-danger">
        <i class="far fa-trash-alt"></i>
        Delete
      </button>
    `)
    deleteBtn.on("click", function() {
      removeLampScheduleItem(start, end)
    })
    schedItem.append(infoParagraph)
    schedItem.append(deleteBtn)
    return schedItem
  }

  function updatePumpClient() {
    let pumpScheduleList = $("#pump-schedule-list")
    pumpScheduleList.empty()

    pumpSchedule.map((item) => {
      pumpScheduleList.append(schedulePumpItem(item.start, item.end))
    })
  }

  function updateLampClient() {
    let lampScheduleList = $("#lamp-schedule-list")
    lampScheduleList.empty()

    lampSchedule.map((item) => {
      lampScheduleList.append(scheduleLampItem(item.start, item.end))
    })
  }

  function updateScheduleServer(url, schedule) {
    $.post(url, {"schedule": schedule})
  }

  // initialize charts and grab chart data
  tempChart = setupChart("#temperature-reading")
  moistureChart = setupChart("#moisture-reading")
  humidityChart = setupChart("#humidity-reading")

  $.getJSON("http://608dev.net/sandbox/sc/mattfeng/finalproject/server/sensors/read.py", function(data) {
    setData(tempChart, data.temps, "Temperature", TEMPERATURE_BG_COLOR, TEMPERATURE_FG_COLOR)
    setData(moistureChart, data.moists, "Soil Moisture", MOISTURE_BG_COLOR, MOISTURE_FG_COLOR)
    setData(humidityChart, data.humids, "Humidity", HUMIDITY_BG_COLOR, HUMIDITY_FG_COLOR)
  })


  // initialize clock/time pickers
  $(".clockpicker").clockpicker({
    donetext: "Done",
    autoclose: true
  })

  // finish setup for modals
  $("#add-pump-time-modal").on("shown.bs.modal", function() {
    $('#add-pump-time-modal').trigger("focus")
  })

  $("#add-lamp-time-modal").on("shown.bs.modal", function() {
    $('#add-lamp-time-modal').trigger("focus")
  })

  $("#pump-schedule-finish").on("click", addPumpScheduleItem)
  $("#lamp-schedule-finish").on("click", addLampScheduleItem)

  // setup schedules
  $.getJSON("http://608dev.net/sandbox/sc/mattfeng/finalproject/server/schedule/pump.py", function(data) {
    pumpSchedule = data.schedule
    updatePumpClient()
  })

  $.getJSON("http://608dev.net/sandbox/sc/mattfeng/finalproject/server/schedule/lamp.py", function(data) {
    lampSchedule = data.schedule
    updateLampClient()
  })

  $("#moisture-min").on("blur", function() {
    $.post(MOISTURE_MIN_URL, {"value": $("#moisture-min").val()})
  })

  $("#moisture-max").on("blur", function() {
    $.post(MOISTURE_MAX_URL, {"value": $("#moisture-max").val()})
  })

  $.getJSON(MOISTURE_BOUNDS_URL, function(data) {
    $("#moisture-max").val(data.max)
    $("#moisture-min").val(data.min)
  })

})