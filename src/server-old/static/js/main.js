$(document).ready(function() {
  setInterval(function() {
    $.getJSON("/sensors", function(data) {
      $("#temperature-reading").text(data.temp)
      $("#humidity-reading").text(data.humid)
      $("#moisture-reading").text(data.moist)
    })
  }, 1000)
})