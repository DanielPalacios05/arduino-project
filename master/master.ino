#include <ESP8266WebServer.h>
#include <Ticker.h>
#include <WebSocketsServer.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#define sensor A0

const String ssid =  "experienciaeafit3";
const String mac  = "experiencia";

Ticker timer;
ESP8266WebServer server;
WebSocketsServer socket = WebSocketsServer(81);
char* html = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Medidor de temperatura</title>
</head>
<body onload="init()">
    <h1>Medidor de temperatura</h1>

    <h2 id="tempLog" style="text-align: center"></h2>
    <canvas class="c">

    </canvas>
</body>

<script>
    function init(){
        websocket = new WebSocket('ws://' + window.location.hostname + ':81/') 
        websocket.onmessage = function(event){
            var data = JSON.parse(event.data)

            document.getElementById("tempLog").innerHTML = "Temperatura:"+ data.temp
        }
    }
</script>
</html>
)=====";

// Adding a websocket to the server



void setup() {

  Serial.begin(115200);

  WiFi.begin(ssid,mac);

  socket.begin();

  Serial.print("IP Address:");
  Serial.println(WiFi.localIP());


  timer.attach(1,sendTemp);

  Wire.begin();
  pinMode(sensor,INPUT);

  server.on("/",[](){
    server.send_P(200, "text/html", html);
  });
  server.begin();

}

void loop() {

  socket.loop();


  Wire.beginTransmission(1);

  
  Wire.write(getTemp());

  Wire.endTransmission();

    server.handleClient();

    delay(100);
}

int getTemp(){

  int val = analogRead(sensor);

  int temp = val/3.1;

  return temp;
  }

void sendTemp(){
    String json = "{\"temp\":";
    json += getTemp();
    json += "}";
    socket.broadcastTXT(json.c_str(), json.length());
}
