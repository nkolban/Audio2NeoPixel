var express = require("express");
var app = express();
app.use(express.static("."));
app.listen(8080, function() {
  console.log("--- Hexapod WebServer ---");
  console.log("Serve files from the NeoPixel3D project folder.");
  console.log("Express app started on port 8080");
});
