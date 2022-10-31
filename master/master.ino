#include <ESP8266WebServer.h>
#include <Ticker.h>
#include <WebSocketsServer.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#define sensor A0


const char* html = R"=====(
<!DOCTYPE html>
<html lang="en">
  <head>
        
    <script src='https://cdn.jsdelivr.net/npm/chart.js'></script>
    <meta charset="UTF-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Medidor de temperatura</title>
  </head>
  <body onload="init()">
    <h1>Medidor de temperatura</h1>

    <div style="text-align: center">
      <h2 id="tempLog" style="text-align: center"></h2>

      <button id="toggleSensorButton" onclick="toggleTemp()" style="text-align: center">
        Apagar sensor
      </button>
    </div>

    <div style="text-align: center; width:50em;height:50em">
        <canvas id="tempChart" width="100" height="100" ></canvas>
  </div>




    <script>
      var dataPlot;
      var maxDataPoints = 20;
      function removeData() {
        dataPlot.data.labels.shift();
        dataPlot.data.datasets[0].data.shift();
      }
      function addData(label, data) {
        if (dataPlot.data.labels.length > maxDataPoints) removeData();
        dataPlot.data.labels.push(label);
        dataPlot.data.datasets[0].data.push(data);
        dataPlot.update();
      }

      var tempOn = true;
      var websocket = null;

      dataPlot = new Chart(document.getElementById('tempChart'), {
        type: 'line',
        data: {
          labels: [],
          datasets: [
            {
              data: [],
              label: 'Temperatura',
              borderColor:'#3e95cd',
              fill: false,
            },
          ],
        },

        options: {
          scales: {
                    yAxes: [{
                            display: true,
                            ticks: {
                                beginAtZero: true,
                                steps: 10,
                                stepValue: 5,
                                max: 150,
                            }
                        }]
          }
        },
      });

      function init() {
        websocket = new WebSocket("ws://" + window.location.hostname + ":81/");
        websocket.onmessage = function (event) {
          var data = JSON.parse(event.data);

          document.getElementById("tempLog").innerHTML =
            "Temperatura:" + data.temp;
          var today = new Date();
          var t = today.getHours() + ":" + today.getMinutes() + ":" + today.getSeconds();
          addData(t, data.temp);
        };
      }

      function toggleTemp() {
        if (tempOn) {
          websocket.close();
          tempOn = false;

          document.getElementById("tempLog").innerHTML = "Sensor apagado";
          document.getElementById("toggleSensorButton").innerHTML = "Prender sensor";
        } else {
          init();
          tempOn = true;
          document.getElementById("toggleSensorButton").innerHTML ="Apagar sensor";
        }
      }
    </script>
  </body>
</html>

)=====";

const String ssid =  "experienciaeafit3";
const String mac  = "experiencia";

Ticker timer;
ESP8266WebServer server;
WebSocketsServer socket = WebSocketsServer(81);
uint8_t clientNum = 0;



int getTemp(){

  int val = analogRead(sensor);

  int temp = val/3.1;

  return temp;
  }

// Adding a websocket to the server

void sendTemp(){
    String json = "{\"temp\":";
    json += getTemp();
    json += "}";
    socket.broadcastTXT(json.c_str(), json.length());
}


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

  
  server.handleClient();

  socket.loop();



  if(socket.clientIsConnected(clientNum)){


    
      Wire.beginTransmission(1);

      Wire.write(getTemp());

      Wire.endTransmission();

  }else{

        
      Wire.beginTransmission(1);

      Wire.write(0);

      Wire.endTransmission();

  }



    delay(100);
}


