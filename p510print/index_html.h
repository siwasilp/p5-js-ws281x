// This contains the HTML and JavaScript for the client browser.
// Note p5.js setup() and draw() are here. PROGMEM is a way
// in Arduino speak to save RAM. PROGMEM puts this string in
// program memory which means FLASH. The R"=====(...)====="
// construct is called a C++ raw liternal and is used so
// characters such as double quotes do not have to be escaped.
const char INDEX_HTML[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <head>
    <title>p5.js LED grid demo</title>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
    <script src="https://cdnjs.cloudflare.com/ajax/libs/p5.js/0.5.14/p5.min.js"></script>
    <script>
var websock;
var WebSockOpen=0;  //0=close,1=opening,2=open

var SCALER;
var SPACING=2;
var GREEN;
var row = 0;
var col = 0;

function start() {
  websock = new WebSocket("ws://" + window.location.hostname + ":81/");
  WebSockOpen=1;
  websock.onopen = function(evt) {
    console.log('websock open');
    WebSockOpen=2;
    var e = document.getElementById('webSockStatus');
    e.style.backgroundColor = 'green';
  };
  websock.onclose = function(evt) {
    console.log('websock close');
    WebSockOpen=0;
    var e = document.getElementById('webSockStatus');
    e.style.backgroundColor = 'red';
  };
  websock.onerror = function(evt) { console.log(evt); };
  websock.onmessage = function(evt) {
    // Messages from the client browser arrive here.
    console.log('websock message ' + evt.data);
    Buttons = JSON.parse(evt.data);
  };
}

function setup() {
  // Create LED pixel wall (OK, it is a small wall) 8 LEDs wide by 16 LEDs tall
  ledgrid = createGraphics(8, 16);
  ledgrid.background(0);
  ledgrid.stroke(0, 255, 0);
  ledgrid.strokeWeight(1);
  ledgrid.pixelDensity(1);
  SCALER = windowHeight/ledgrid.height;
  // Show the LED pixel wall scaled up so it is visible
  createCanvas(ledgrid.width*SCALER, ledgrid.height*SCALER);
  background(0);
  frameRate(10);
  GREEN = color(0, 255, 0);
}

function draw() {
  //console.log('col', col, 'row', row);
  if (random(1) < 0.5) {
    // * -
    // - *
    ledgrid.set(col, row, GREEN);
    ledgrid.set(col+1, row+1, GREEN);
  }
  else {
    // - *
    // * -
    ledgrid.set(col, row+1, GREEN);
    ledgrid.set(col+1, row, GREEN);
  }
  ledgrid.updatePixels();
  // Advance to next 2x2 LED subgrid
  col += SPACING;
  if (col >= ledgrid.width) {
    col = 0;
    row += SPACING;
    if (row >= ledgrid.height) {
      row = 0;
      ledgrid.clear();
      ledgrid.background(0);
      clear();
      background(0);
    }
  }
  // Show the LED wall. Is the blurring is a side-effect of the bitmap
  // stretching?
  image(ledgrid, 0, 0, width, height);

  if (WebSockOpen === 2) {
    ledgrid.loadPixels();
    var d = ledgrid.pixelDensity();
    //console.log("pixelDensity", d);
    var fullImage = 4 * (ledgrid.width * d) * (ledgrid.height * d);
    //console.log("fullImage", fullImage);
    var ledobj = {};
    ledobj.nLEDs = ledgrid.width * ledgrid.height;
    ledobj.rgb = new Array(ledobj.nLEDs*3);
    for (var i = 0, j = 0; i < fullImage; i += 4, j += 3) {
      //console.log(ledgrid.pixels[i], ledgrid.pixels[i+1], ledgrid.pixels[i+2]);
      ledobj.rgb[j]   = ledgrid.pixels[i]/8;
      ledobj.rgb[j+1] = ledgrid.pixels[i+1]/8;
      ledobj.rgb[j+2] = ledgrid.pixels[i+2]/8;
    }
    websock.send(JSON.stringify(ledobj));
  }
}
    </script>
  </head>
    <body onload="javascript:start();">
    <h3>p5.js LED grid demo</h3>
    <button id="webSockStatus" type="button" onclick="window.location.reload();">Connect</button>
    <p>
    </body>
</html>
)=====";
