/*Firstly install the package for NodeMCU from Board Manager
to do that, go to File > Preferences, and copy paste this : 
https://dl.espressif.com/dl/package_esp32_index.json, 
http://arduino.esp8266.com/stable/package_esp8266com_index.json
in additional boards manager URLs
Then, go to Tools > Board > Board Manager, and search esp8266, 
Install the first package you see 
again go to Boards and choose NodeMCU 1.0 ESP 12E Modulde and compile this code */

#include <ESP8266WiFi.h>
//#include <Servo.h>
//ultrasonic
const int trigPin = 5;//D1
const int echoPin = 4;//D2
//Servo s;
//defines variables
long duration;
int distance;
String webStringUS;
String webStringGas;
String webStringMoisture;
int gas_value;


//moisture
int sensor_pin = A0; 
int output_value ;

//gas sensor
const int gasPin = 2; //Gas sensor output pin to arduino analog A0 pin


String condition;
const char* ssid = "<Your Wifi/Hotspot SSID>";
const char* password = "<Your Wifi/Hotspot Password>";

WiFiServer server(80);
/*int servo()
{
  // your code for servo here
}*/
int ultrasonic(){
   //Clear the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  //Sets the trigPin on the HIGH state for 10 microseconds
  digitalWrite (trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  //Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  //Calculating the distance 
  distance = duration*0.034/2;
//  Prints the distance on the serial monitor
  Serial.print("Distance: ");
  Serial.println(distance);
  delay(400);
   if(distance>=18)
  {
    condition="Empty";
    Serial.println(condition);
  }

  else if(distance>=9 && distance <=17)
  {
    condition="Half filled";
    Serial.println(condition);
  }
   
  else if (distance<=5) 
  {
    condition="Full";
    Serial.println(condition);
  }

  else{
    condition="Almost Full";
    Serial.println(condition);
  }
}

void moisture(){
  output_value= analogRead(sensor_pin);
  output_value = map(output_value,550,0,0,100);
  Serial.print("Mositure : ");
  Serial.print(output_value);
  Serial.println("%");
  delay(1000);  
}

void gas(){
  gas_value = digitalRead(gasPin);
  Serial.print("Gas Content: ");
  Serial.println(analogRead(gasPin));
  delay(1000); //Print value every 1 sec.  
}



void setup()
{
  pinMode(trigPin, OUTPUT); //set the trigPin as an output
  pinMode(echoPin, INPUT); 
  Serial.begin(115200);
  Serial.println();

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  server.begin();
  Serial.printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());
}


// prepare a web page to be send to a client (web browser)
String prepareHtmlPage()
{
  String htmlPage =
     String("HTTP/1.1 200 OK\r\n") +
            "Content-Type: text/html\r\n" +
            "Connection: close\r\n" +  // the connection will be closed after completion of the response
            "Refresh: 5\r\n" +  // refresh the page automatically every 5 sec
            "\r\n" +
            "<!DOCTYPE HTML>" +
            "<html>" +
            "<title>"+
            "Garbage management system"+
            "</title>"+
            "<body>"+
            "<p>"+
            "Garbage amount:  " + String(distance) +
            "\r\n"+
            "</p>"+
            "Gas content:" + String(gas_value) +
            "\r\n"+
            "Moisture content:" + String(output_value)+
            "<p>"+
            "The dustbin is"+ String(condition)+
            "</p>"+
            "</body>"+
            "</html>" +
            "\r\n";
  return htmlPage;
}


void loop()
{
  Serial.println("Hello");
  WiFiClient client = server.available();
  // wait for a client (web browser) to connect
  if (client)
  {
    Serial.println("\n[Client connected]");
    while (client.connected())
    {
      // read line by line what the client (web browser) is requesting
      if (client.available())
      {
        String line = client.readStringUntil('\r');
        Serial.print(line);
        // wait for end of client's request, that is marked with an empty line
        if (line.length() == 1 && line[0] == '\n')
        {
          client.println(prepareHtmlPage());
          break;
        }
      }
    }
    delay(1); // give the web browser time to receive the data

    // close the connection:
    client.stop();
    Serial.println("[Client disonnected]");

    ultrasonic();
    moisture();
    gas();
  }
}
