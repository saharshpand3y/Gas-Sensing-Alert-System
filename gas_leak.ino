#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h>

#define green_led D8
#define red_led D2
const char* ssid = "Garv";
const char* password = "22102004";
SoftwareSerial mySerial(D1, D0);
WiFiServer server(80);
WiFiClient client;
String gasStatus = "No gas leak";

void setup() {
  mySerial.begin(9600);
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(green_led, OUTPUT);
  pinMode(red_led, OUTPUT);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  delay(5000);
}

void loop() {
  int gasSensor = analogRead(A0);
  Serial.print("Gas Reading: ");
  Serial.println(gasSensor);
  handleClient();
  if (gasSensor >= 150){
    digitalWrite(red_led, HIGH);
    digitalWrite(green_led, LOW);
    delay(100);
    MakeCall();
    SendMessage();
    updateGasStatus(true);
  } else {
    digitalWrite(red_led, LOW);
    digitalWrite(green_led, HIGH);
    updateGasStatus(false);
  }
  delay(100);
}

void MakeCall() {
  mySerial.println("ATD+9180927xxxxx;");
  Serial.println("Calling  ");
  delay(1000);
}

void SendMessage() {
  mySerial.println("AT+CMGF=1");
  delay(1000);
  mySerial.println("AT+CMGS=\"+9180927xxxxx\"\r");
  mySerial.println("Hi Yukti Gas Detected plz Open Windows And Check Your Gas Cylinder");
  delay(100);
  mySerial.println((char)26);
  delay(1000);
}
void updateGasStatus(bool hasLeak) {
  if (hasLeak) {
    Serial.println("Gas leak detected");
    gasStatus = "Gas leak detected";
  } else {
    Serial.println("No gas leak");
    gasStatus = "No gas leak";
  }
}

void handleClient() {
  client = server.available();
  if (client) {
    Serial.println("New client connected");
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Cache-Control: no-cache"); // Ensure response is not cached
    client.println();
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("<head><title>Gas Leak Status</title>");
    client.println("<style>");
    client.println("body {");
    client.println("  display: flex;");
    client.println("  justify-content: center;");
    client.println("  align-items: center;");
    client.println("  height: 100vh;");
    client.println("  margin: 0;");
    client.println("  background-color: white;");
    client.println("}");
    client.println(".box {");
    client.println("  width: 300px;");
    client.println("  height: 200px;");
    client.println("  background-color: black;");
    client.println("  color: white;");
    client.println("  text-align: center;");
    client.println("  border-radius: 10px;");
    client.println("  box-shadow: 0px 0px 10px rgba(0, 0, 0, 0.5);");
    client.println("}");
    client.println(".box p {");
    client.println("  font-size: 24px;");
    client.println("  font-style: italic;");
    client.println("}");
    client.println("</style>");
    client.println("</head>");
    client.println("<body>");
    client.println("<div class='box'>");
    client.println("<h1>Gas Leak Status:</h1>");
    client.println("<p>" + gasStatus + "</p>");
    client.println("</div>");
    client.println("</body>");
    client.println("</html>");
    Serial.println("Client response sent");
  }
}
