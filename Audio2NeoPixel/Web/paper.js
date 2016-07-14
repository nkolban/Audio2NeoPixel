/**
 * 
 */
$(function(){
  const LED_COUNT=11;
  const MAX_AUDIO=1000000;
  const BAR_HEIGHT=200;
  let width = $("#scene").width()-5;
  let led_width = Math.floor(width/LED_COUNT);
  let canvas=$("#scene").get(0);
  let rectangleArray = [];
  let ledsArray = [];
  paper.setup(canvas);
  drawScales();
  

  
  paper.view.draw();
  
  $("#reconnect").button().click(()=>{
    // Connect to the WS Server
    var ws = new WebSocket("ws://raspi4:3000");
    ws.onopen = function() {
      console.log("WebSocket formed!");
    };
    ws.onmessage = function(messageEvent) {
      console.log("Data: " + messageEvent.data);
      let data = JSON.parse(messageEvent.data);
      drawBars(data);
      drawLeds(data);
      paper.view.update();
    }
    ws.onerror = function(err) {
      console.log("Web socket error:" + err);
    }
    ws.onclose = function() {
      console.log("Socket closed!");
    }
  });
  
  function drawLeds(data) {
    for (let i=0; i<LED_COUNT; i++) {
      let x = i*led_width + led_width/2 + 5;
      let y= 100;
      let brightness = data[i] / MAX_AUDIO * 200/100;
      if (brightness > 1) {
        brightness = 1;
      }
      var circle1 = new paper.Shape.Circle(new paper.Point(x,y), led_width/2);
      circle1.set({
        strokeColor: 'black',
        fillColor: new paper.Color({
            hue: i/LED_COUNT*360,
            saturation: 1,
            brightness: brightness
        })
      });
      if (ledsArray[i] != null) {
        ledsArray[i].remove();
      }
      ledsArray[i] = circle1;
    }
  }
  
  function drawBars(barsData) {
    for (let i=0; i<LED_COUNT; i++) {
      // Draw a rectangle from x=i*ledWidth for width ledWidth
      let barHeight = BAR_HEIGHT * barsData[i] / MAX_AUDIO;
      if (barHeight > BAR_HEIGHT) {
        barHeight = BAR_HEIGHT;
      }
      var rectangle = new paper.Shape.Rectangle(new paper.Point(i*led_width, 400), new paper.Size(led_width, -barHeight));
      rectangle.set({
        strokeColor:'black',
        fillColor: "green"
      });
      if (rectangleArray[i] != null) {
        rectangleArray[i].remove();
      }
      rectangleArray[i] = rectangle;
    } // End of for loop
  }
  
  function drawScales() {
    for (let i=0; i<LED_COUNT; i++) {
      
      var text = new paper.PointText(new paper.Point((i+1)*led_width, 415));
      text.content = String(Math.round(logspace(50,22000, i, LED_COUNT))) + "Hz";
      text.characterStyle = {
          fontSize:8,
          fillColor:"#333333",
          font:"Arial"
      };
      text.rotate(90, text.bounds.topLeft);
      //text.rotation = 90;
    }
  }
  
  function logspace(start,  stop, n, N)
  {
      return start * Math.pow(stop/start, n/(N-1));
  }

});