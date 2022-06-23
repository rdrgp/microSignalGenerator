#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

// Access Point default values
static const char *apSSID = "BciAP";
static const char *apPASSWORD = "123456789";
// This server can be accessed through: http://192.168.4.1

// Variables to configure access to an external ROUTER. Initially empty!
String stationSSID = "";
String stationPASSWORD = "";

WebServer server(80); // Creates a webserver object that listens for HTTP request on port 80
Preferences preferences;  // Creates a preferences object that allows to store data into ESP32's flash memory

// Variables storing HTML & CSS
String CSS_STYLE = "html {font-family: Montserrat;display: inline-block;margin: 0px auto;text-align: center;}h1 {color: #0F3376;font-family: Montserrat;font-size: 40px;padding: 2vh;}p {font-size: 1.4rem;}.values {display: inline-block;text-align: left;}.amp-value {color: #fff;background: #04AA6D;margin: 0.1em;padding: 0.1em;border-radius: 4px;font-size: 2rem;}.frq-value {color: #fff;background: #0300c7;margin: 0.1em;padding: 0.1em;border-radius: 4px;font-size: 2rem;}.spa-value{color: #fff; background: #c76700; margin: 0.1em; padding: 0.1em; border-radius: 4px; font-size: 2rem;}.button {font-family: \"Montserrat\";background-color: #008CBA;border: none;border-radius: 4px;color: white;padding: 16px 40px;text-decoration: none;font-size: 30px;margin: 2px;cursor: pointer;}.button_sm {font-size: 20px;padding: 8px 20px;margin: 1px;}.buttonred {background-color: #f44336;}.buttongrey {background-color: #989898;}.slider {-webkit-appearance: none;width: 100%;height: 15px;border-radius: 5px;background: #d3d3d3;outline: none;opacity: 0.7;-webkit-transition: .2s;transition: opacity .2s;}.slider::-webkit-slider-thumb {-webkit-appearance: none;appearance: none;width: 25px;height: 25px;border-radius: 50%;background: #04AA6D;cursor: pointer;}.slider2::-webkit-range-thumb {background: #0300c7;}.slider::-moz-range-thumb {width: 25px;height: 25px;border-radius: 50%;background: #04AA6D;cursor: pointer;}.slider2::-moz-range-thumb {background: #0300c7;}.slider-div {width:50%;margin: auto;display:table;}@media only screen and (max-width: 600px) {.slider-div {width:90%;margin: auto;display:table;}}";
String HTML_INDEX = "<!DOCTYPE html><html><head><title>BCI Signal Simulator</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><style>" + CSS_STYLE + "</style></head><body><h1>BCI Signal Simulator</h1><table style=\"display: flex; justify-content: center\"><tr><td><form style=\"display:flex; justify-content: right\" accept-charset=\"utf-8\" display=\"border-left\" name=\"sine button\" id=\"sine button\" method=\"GET\"action=\"/signal_mode\"><button name=\"mode\" type=\"submit\" class=\"button button_sm\" value=\"sine\">Sine Wave</button></form></td><td><form style=\"display:flex; justify-content: left\" accept-charset=\"utf-8\" name=\"sw button\" id=\"sw button\" method=\"GET\" action=\"/signal_mode\"><button name=\"mode\" type=\"submit\" class=\"button button_sm buttongrey\" value=\"sw\">Square Wave</button></form></td></tr><tr><td><form style=\"display:flex; justify-content: right\" accept-charset=\"utf-8\" display=\"border-left\" name=\"microv button\" id=\"microv button\" method=\"GET\"action=\"/magnitude\"><button name=\"mag\" type=\"submit\" class=\"button button_sm buttonred\" value=\"microv\">uV</button></form></td><td><form style=\"display:flex; justify-content: left\" accept-charset=\"utf-8\" name=\"microv button\" id=\"microv button\" method=\"GET\" action=\"/magnitude\"><button name=\"mag\" type=\"submit\" class=\"button button_sm buttongrey\" value=\"miliv\">mV</button></form></td></tr></table><br><form name=\"signal_form\" id = \"signal_form_1\" method=\"GET\" action=\"/signal\"><div class=\"slider-div\"><div ><label for=\"ref\">Reference:</label><br><div><input type=\"range\" min=\"0\" max=\"200\" step =\"10\" value=\"0\" class=\"slider\" id=\"ref\" name=\"ref\"><div class=\"values\"><p><nobr>Reference: <span class=\"amp-value\" id=\"refval\"></span>uV</nobr></p></div></div><br></div><hr><br><div ><label for=\"amp1\">Amplitude 1:</label><br><div><input type=\"range\" min=\"0\" max=\"200\" step =\"10\" value=\"0\" class=\"slider\" id=\"amp1\" name=\"amp1\"></div><label for=\"frq1\">Frequency 1:</label><br><div><input type=\"range\" min=\"1\" max=\"100\" value=\"1\" class=\"slider slider2\" id=\"frq1\" name=\"frq1\"><div class=\"values\"><p><nobr>Amplitude 1: <span class=\"amp-value\" id=\"amp1val\"></span>uV</nobr></p><p><nobr>Frequency 1: <span class=\"frq-value\" id=\"frq1val\"></span>Hz</nobr></p></div></div></div><hr><br><div><label for=\"amp2\">Amplitude 2:</label><br><div><input type=\"range\" min=\"0\" max=\"200\" step =\"10\" value=\"0\" class=\"slider\" id=\"amp2\" name=\"amp2\"></div><label for=\"frq2\">Frequency 2:</label><br><div><input type=\"range\" min=\"1\" max=\"100\" value=\"1\" class=\"slider slider2\" id=\"frq2\" name=\"frq2\"><div class=\"values\"><p><nobr>Amplitude 2: <span class=\"amp-value\" class=\"value\" id=\"amp2val\"></span>uV</nobr></p><p><nobr>Frequency 2: <span class=\"frq-value\" id=\"frq2val\"></span>Hz</nobr></p></div></div></div><hr><br><div><label for=\"amp3\">Amplitude 3:</label><br><div><input type=\"range\" min=\"0\" max=\"200\" step =\"10\" value=\"0\" class=\"slider\" id=\"amp3\" name=\"amp3\"></div><label for=\"frq3\">Frequency 3:</label><br><div><input type=\"range\" min=\"1\" max=\"100\" value=\"1\" class=\"slider slider2\" id=\"frq3\" name=\"frq3\">´<div class=\"values\"><p><nobr>Amplitude 3: <span class=\"amp-value\" id=\"amp3val\"></span>uV</nobr></p><p><nobr>Frequency 3: <span class=\"frq-value\" id=\"frq3val\"></span>Hz</nobr></p></div></div></div></div><div style=\"margin: auto;\"><input type=\"submit\" class=\"button buttonred\" value=\"Generate signal\"></div></form><br><script language = \"JavaScript\">var refslider = document.getElementById(\"ref\");var refoutput = document.getElementById(\"refval\");var amp1slider = document.getElementById(\"amp1\");var amp1output = document.getElementById(\"amp1val\");var amp2slider = document.getElementById(\"amp2\");var amp2output = document.getElementById(\"amp2val\");var amp3slider = document.getElementById(\"amp3\");var amp3output = document.getElementById(\"amp3val\");var frq1slider = document.getElementById(\"frq1\");var frq1output = document.getElementById(\"frq1val\");var frq2slider = document.getElementById(\"frq2\");var frq2output = document.getElementById(\"frq2val\");var frq3slider = document.getElementById(\"frq3\");var frq3output = document.getElementById(\"frq3val\");refoutput.innerHTML = refslider.value;refslider.oninput = function() {refoutput.innerHTML = this.value;};amp1output.innerHTML = amp1slider.value;amp1slider.oninput = function() {amp1output.innerHTML = this.value;};amp2output.innerHTML = amp2slider.value;amp2slider.oninput = function() {amp2output.innerHTML = this.value;};amp3output.innerHTML = amp3slider.value;amp3slider.oninput = function() {amp3output.innerHTML = this.value;};frq1output.innerHTML = frq1slider.value;frq1slider.oninput = function() {frq1output.innerHTML = this.value;};frq2output.innerHTML = frq2slider.value;frq2slider.oninput = function() {frq2output.innerHTML = this.value;};frq3output.innerHTML = frq3slider.value;frq3slider.oninput = function() {frq3output.innerHTML = this.value;};</script></body></html>";
String HTML_SCAN = "<!DOCTYPE html><html><head><title>ESP32 Web Server</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><style>" + CSS_STYLE + "</style></head><body><h1>ESP32 Web Server</h1><p><a href=\"/scan\"><button class=\"button\">SCAN NETWORKS</button></a></p><p><a href=\"/\"><button class=\"button2\">Return</button></a></p></body></html>";
String HTML_SIGNAL = "";
String HTML_ERROR = "<!DOCTYPE html><html><head><title>BCI Signal Simulator</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><style>" + CSS_STYLE + "</style></head><body><h1>The signal parameters are outside of the accepted range</h1><h2>Maximum peak-to-peak signal amplitude: 200 uV/mV</h2><br><div style=\"margin: auto;\"><form method=\"GET\" action=\"/\"><input type=\"submit\" class=\"button button2\" value=\"Generate new signal\"></form></div></body></html>";
String mili_button_off = "<button name=\"mag\" type=\"submit\" class=\"button button_sm buttongrey\" value=\"miliv\">";
String mili_button_on = "<button name=\"mag\" type=\"submit\" class=\"button button_sm buttonred\" value=\"miliv\">";
String micro_button_off = "<button name=\"mag\" type=\"submit\" class=\"button button_sm buttongrey\" value=\"microv\">";
String micro_button_on = "<button name=\"mag\" type=\"submit\" class=\"button button_sm buttonred\" value=\"microv\">";
String mili_text = "mV</nobr>";
String micro_text = "uV</nobr>";


String sw_button_off = "<button name=\"mode\" type=\"submit\" class=\"button button_sm buttongrey\" value=\"sw\">";
String sw_button_on = "<button name=\"mode\" type=\"submit\" class=\"button button_sm\" value=\"sw\">";
String sine_button_off = "<button name=\"mode\" type=\"submit\" class=\"button button_sm buttongrey\" value=\"sine\">";
String sine_button_on = "<button name=\"mode\" type=\"submit\" class=\"button button_sm\" value=\"sine\">";


// Declaring program variables
const int pinSIG = 25;
const int pinREF = 26;
const int pinLED = 13;
const int handleclock = 1000;
unsigned long temp1;
unsigned long temp2;
unsigned long temp3;
unsigned long temppulse;
unsigned long period1;
unsigned long period2;
unsigned long period3;
unsigned long periodp;
int t1;
int t2;
int t3;
int tp;
float sin1;
float sin2;
float sin3;
float sinp;
float pin25_max;
float pin26_max;
byte sig;
byte sig_aux;
byte ref;
int frq1;
int frq2;
int frq3;
byte amp1;
byte amp2;
byte amp3;
int pulseamp;
int pulse_duration;
int pulse_spacing;
int pulse_cycle;
float vd_coeff;
int smode;

// Re-maps a real voltage values from one range to another
byte mapsignal(float x, float in_min, float in_max, byte out_min, byte out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Access Point Handling function code
void WiFiAPEvent(WiFiEvent_t event, WiFiEventInfo_t info)
{
  if (event == SYSTEM_EVENT_AP_START) {
    Serial.println("AP Started");
  }
  else if (event == SYSTEM_EVENT_AP_STACONNECTED) {
    Serial.println("Client connected");
  }
  else if (event == SYSTEM_EVENT_AP_STADISCONNECTED) {
    Serial.println("Client disconnected");
  }
}

// WIFI Station Handling function code
void WiFiStationEvent(WiFiEvent_t event, WiFiEventInfo_t info)
{
  if (event == SYSTEM_EVENT_STA_START) {
    Serial.println("Client Started");
  }
  else if (event == SYSTEM_EVENT_STA_CONNECTED) {
    Serial.println("Connected to ROUTER EXTERNO");
  }
  else if (event == SYSTEM_EVENT_STA_DISCONNECTED) {
    Serial.println("Disconnected from ROUTER EXTERNO");
  }
}

void connectingRouterExterno()
{
  Serial.println("Connecting to WIFI EXTERNAL ROUTER...");
  // Connecting to Wifi
  WiFi.onEvent(WiFiStationEvent, SYSTEM_EVENT_STA_START);
  WiFi.onEvent(WiFiStationEvent, SYSTEM_EVENT_STA_CONNECTED);
  WiFi.onEvent(WiFiStationEvent, SYSTEM_EVENT_STA_DISCONNECTED);
  Serial.println(stationSSID.c_str());
  Serial.println(stationPASSWORD.c_str());
  WiFi.begin(stationSSID.c_str(), stationPASSWORD.c_str());
  // Waiting to be connected
  int retries = 0;
  while (retries < 25 && WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    retries++;
  }
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("ERROR: cannot connect to external WIFI router");
    WiFi.disconnect();
  }
  else
  {
    Serial.println("The URL of ESP32 Web Server (inside EXTERNAL ROUTER network) is: ");
    Serial.print("http://");
    Serial.println(WiFi.localIP());
  }
}

void scanWIFI ()
{
  int n = WiFi.scanNetworks();
  Serial.println("Scan done");
  if (n == 0) {
    Serial.println("No networks found");
  } else {
    Serial.print(n);
    Serial.println("Networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      delay(10);
    }
  }
}

void handleRoot()
{
  server.send(200, "text/html", HTML_INDEX);
}

void handleNotFound()
{
  server.send(404, "text/plain", "BCI Signal Simulator page not found");
}

void setup() {
  Serial.begin(115200);
  
  // Define output pins
  pinMode(pinSIG, OUTPUT);
  pinMode(pinREF, OUTPUT);
  pinMode(pinLED, OUTPUT);
  
  // Init Signal variables
  sig = 0;
  sig_aux = 0;
  ref = 0;
  temp1 = micros();
  temp2 = temp1;
  temp3 = temp1;
  temppulse = temp1;
  t1 = 1;
  t2 = 1;
  t3 = 1;
  sin1 = 0;
  sin2 = 0;
  sin3 = 0;
  sinp = 0;
  pin25_max = 3.2;
  pin26_max = 3.2;
  amp1 = 0;
  amp2 = 0;
  amp3 = 0;
  frq1 = 0;
  frq2 = 0;
  frq3 = 0;
  period1 = 1E12;
  period2 = 1E12;
  period3 = 1E12;
  periodp = 1E12;
  pulseamp = 0;
  pulse_duration = 0;
  pulse_spacing = 0;
  pulse_cycle = 4;
  vd_coeff = 1/0.88;
  smode = 1;
  
  Serial.println();

  Serial.println("Configuring access point...");
  // Attach WiFi events to callback function
  WiFi.onEvent(WiFiAPEvent, SYSTEM_EVENT_AP_START);
  WiFi.onEvent(WiFiAPEvent, SYSTEM_EVENT_AP_STACONNECTED);
  WiFi.onEvent(WiFiAPEvent, SYSTEM_EVENT_AP_STADISCONNECTED);

  // Remove the password parameter if you want the AP to be open
  WiFi.softAP(apSSID, apPASSWORD);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP SSID: ");
  Serial.println(apSSID);
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  preferences.begin("BCI", true);
  stationSSID = preferences.getString("router_ssid", "");
  stationPASSWORD = preferences.getString("router_pwd", "");
  Serial.println("Recovered SSID(external router):" + stationSSID);
  Serial.println("Recovered Password(external router):" + stationPASSWORD);
  preferences.end();

  if (stationSSID != "" && stationPASSWORD != "") connectingRouterExterno();

  Serial.println("Starting ESP32 Web Server...");
  server.begin(); /* Start the HTTP web Server */
  Serial.println("ESP32 Web Server Started");
  Serial.print("\n");
  Serial.print("The URL of ESP32 Web Server (en la red del Access Point) is: ");
  Serial.print("http://");
  Serial.println(myIP);

  Serial.print("\n");

  server.on("/", handleRoot);

  server.on("/scan", []() {
    scanWIFI();
    server.send(200, "text/html", HTML_SCAN);
  });
  server.on("/config", []() {
    String ssid = server.arg(String("ssid"));
    String pwd = server.arg(String("pwd"));
    if (ssid == "" || pwd == "")
      server.send(500, "text/plain", "ERROR: Parametros incorrecto/s (falta 'ssid' y/o' pwd')");
    else
    {
      stationSSID = ssid;
      stationPASSWORD = pwd;
      Serial.println("Save SSID y PWD en memoria Flash");
      preferences.begin("BCI", false);
      preferences.putString("router_ssid", stationSSID);
      preferences.putString("router_pwd", stationPASSWORD);
      preferences.end();

      connectingRouterExterno();
      server.send(200, "text/plain", "Configurando red del ROUTER EXTERNO\n    * SSID: " + stationSSID + "\n    * Password:" + stationPASSWORD + "\n");
    }
  });

  server.on("/signal", []() {
    String amp1_st = server.arg(String("amp1"));
    String frq1_st = server.arg(String("frq1"));
    String amp2_st = server.arg(String("amp2"));
    String frq2_st = server.arg(String("frq2"));
    String amp3_st = server.arg(String("amp3"));
    String frq3_st = server.arg(String("frq3"));
    String ref_st = server.arg(String("ref"));
    if (ref_st == "0")
      ref = 0;
    else {
      ref = mapsignal(ref_st.toFloat()/100, 0, pin26_max, 0, 255);
    }
    dacWrite(pinREF, ref);
    if (amp1_st == "0" || frq1_st == "0") {
      amp1 = 0;
      server.send(200, "text/html", HTML_INDEX);
    } else {
      amp1 = mapsignal(amp1_st.toFloat()/100, 0, pin25_max, 0, 255);
      frq1 = frq1_st.toFloat();
      period1 = 1000000 / frq1;
      if (amp2_st == "0" || frq2_st == "0") {
        amp2 = 0;
      } else {
        amp2 = mapsignal(amp2_st.toFloat()/100, 0, pin25_max, 0, 255);
        frq2 = frq2_st.toFloat();
        period2 = 1000000 / frq2;
      }
      if (amp3_st == "0" || frq3_st == "0") {
        amp3 = 0;
      } else {
        amp3 = mapsignal(amp3_st.toFloat()/100, 0, pin25_max, 0, 255);
        frq3 = frq3_st.toFloat();
        period3 = 1000000 / frq3;
      }
      if ((ref + vd_coeff*(amp1 + amp2 + amp3)/2 > 255) || (ref - vd_coeff*(amp1+ amp2 + amp3)/2 < 0)) {
      server.send(200, "text/html", HTML_ERROR);
      } else {
      HTML_SIGNAL = "<!DOCTYPE html><html><head><title>BCI Signal Simulator</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><style>" + CSS_STYLE + "</style></head><body><h1>BCI Signal Simulator</h1><h2>Signal Generated</h2><div style=\"width:50%; margin: auto; display:table;\"><div><p><nobr>Reference:<span class=\"spa-value\"> " + ref_st + " uV</span></nobr></p></div><hr><div><p><nobr>Amplitude 1:<span class=\"amp-value\">" + amp1_st + "</span> uV</nobr></p><p><nobr>Frequency 1:<span class=\"frq-value\">" + frq1_st + "</span> uV</nobr></p></div><hr><div><p><nobr>Amplitude 2:<span class=\"amp-value\">" + amp2_st + "</span> uV</nobr></p><p><nobr>Frequency 2:<span class=\"frq-value\">" + frq2_st + "</span> uV</nobr></p></div><hr><div><p><nobr>Amplitude 3:<span class=\"amp-value\">" + amp3_st + "</span> uV</nobr></p><p><nobr>Frequency 3:<span class=\"frq-value\">" + frq3_st + "</span> uV</nobr></p></div></div><br><div style=\"margin: auto;\"><form method=\"GET\" action=\"/\"><input type=\"submit\" class=\"button button2\" value=\"Generate new signal\"></form></div><br><hr><br><form name=\"pulse_form\" id=\"pulse_form_1\" method=\"GET\" action=\"/pulse\"><div class=\"slider-div\"><label for=\"pamp\">Amplitude of Impulse:</label><br><div><input type=\"range\" min=\"1\" max=\"150\" step =\"10\" value=\"1\" class=\"slider\" id=\"pamp\" name=\"pamp\"></div><label for=\"pdur\">Duration of Impulse:</label><br><div><input type=\"range\" min=\"0\" max=\"10\" value=\"0\" class=\"slider slider2\" id=\"pdur\" name=\"pdur\"></div><label for=\"pdur\">Spacing between of Impulses:</label><br><div><input type=\"range\" min=\"0\" max=\"100\" step =\"10\" value=\"0\" class=\"slider slider2\" id=\"pspa\" name=\"pspa\"></div><div class=\"values\"><p><nobr>Amplitude of Impulse: <span class=\"amp-value\" id=\"pampval\"></span>uV</nobr></p><p><nobr>Duration of Impulse: <span class=\"frq-value\" id=\"pdurval\"></span>ms</nobr></p><p><nobr>Spacing between of Impulses: <span class=\"spa-value\" id=\"pspaval\"></span>ms</nobr></p></div></div><br><div style=\"margin: auto\"><input type=\"submit\" class=\"button buttonred\" value=\"Generate pulse\"></div></form><script language=\"JavaScript\">var pampslider = document.getElementById(\"pamp\");var pampoutput = document.getElementById(\"pampval\");var pdurslider = document.getElementById(\"pdur\");var pduroutput = document.getElementById(\"pdurval\");var pspaslider = document.getElementById(\"pspa\");var pspaoutput = document.getElementById(\"pspaval\");pampoutput.innerHTML = pampslider.value;pampslider.oninput = function () {pampoutput.innerHTML = this.value;};pduroutput.innerHTML = pdurslider.value;pdurslider.oninput = function () {pduroutput.innerHTML = this.value;};pspaoutput.innerHTML = pspaslider.value;pspaslider.oninput = function () {pspaoutput.innerHTML = this.value;};</script></body></html>";
      server.send(200, "text/html", HTML_SIGNAL);
      }
    }
  });

  server.on("/pulse", []() {
    String pulsedur_st = server.arg(String("pdur"));
    String pulseamp_st = server.arg(String("pamp"));
    String pulsespa_st = server.arg(String("pspa"));
    if (pulsedur_st == "0" || pulseamp_st == "0" || pulsespa_st == "0") {
      pulse_cycle = 4;
    } else {
      pulse_cycle = 0;
      pulseamp =  mapsignal((pulseamp_st.toFloat()), 0, pin25_max, 0, 255);
      pulse_duration = pulsedur_st.toFloat(); // transform ms to micros
      pulse_spacing = pulsespa_st.toFloat()*1000; // transform ms to micros
    }    
    server.send(200, "text/html", HTML_SIGNAL);
    temppulse = micros(); 
  });

  server.on("/magnitude", []() { 
    String mag_st = server.arg(String("mag"));
    if (mag_st == "miliv") {
      vd_coeff = 1/0.9;
      HTML_INDEX.replace(mili_button_off,mili_button_on);
      HTML_INDEX.replace(micro_button_on,micro_button_off);
      HTML_INDEX.replace(micro_text,mili_text);
      HTML_SIGNAL.replace(mili_text,micro_text);
      server.send(200, "text/html", HTML_INDEX);
    }
    else if (mag_st == "microv"){
      vd_coeff = 1/1;
      HTML_INDEX.replace(micro_button_off,micro_button_on);
      HTML_INDEX.replace(mili_button_on,mili_button_off);
      HTML_INDEX.replace(mili_text,micro_text);
      HTML_SIGNAL.replace(mili_text,micro_text);     
      server.send(200, "text/html", HTML_INDEX);
    }
  });
  
   server.on("/signal_mode", []() { 
    String mode_st = server.arg(String("mode"));
    if (mode_st == "sw") {
      smode = 0;
      sin1 = 1;
      sin2 = 1;
      sin3 = 1;
      HTML_INDEX.replace(sw_button_off,sw_button_on);
      HTML_INDEX.replace(sine_button_on,sine_button_off);
      server.send(200, "text/html", HTML_INDEX);
    }
    else if (mode_st == "sine"){
      smode = 1;
      HTML_INDEX.replace(sine_button_off,sine_button_on);
      HTML_INDEX.replace(sw_button_on,sw_button_off);
      server.send(200, "text/html", HTML_INDEX);
    }
  });

  
  server.on("/ip", []() {
    server.send(200, "text/plain", "IP(Red Access Point):" + WiFi.softAPIP().toString() + " IP(Red Wifi Externo):" + WiFi.localIP().toString());
  });
  
  server.onNotFound(handleNotFound);

  digitalWrite (pinLED, HIGH);
  delay(500);
  digitalWrite (pinLED, LOW);
  delay(500);
  digitalWrite (pinLED, HIGH);
  delay(500);
  digitalWrite (pinLED, LOW);
  delay(500);
  digitalWrite (pinLED, HIGH);
  delay(500);
 
}
void loop() {
  // Web server
  if (millis()%handleclock == 0) {
  server.handleClient();
  }
  
  if (smode = 0) {
  // Square Wave Signal Generation
    if ((micros() > (temp1 + period1/2)) && amp1 != 0) {
    temp1 = micros();
    sin1 = -sin1;
    }
    if ((micros() > (temp2 + period2/2)) && amp2 != 0) {
    temp2 = micros();
    sin2 = -sin2;
    }
    if ((micros() > (temp3 + period3/2)) && amp3 != 0) {
    temp3 = micros();
    sin3 = -sin3;
    }

  } else {
  // Sinusoidal Wave Signal Generation
    if (amp1 != 0) {
      //temp1 = micros();
      sin1 = sin(2*PI*(micros()%period1)/period1);
    }
    if (amp2 != 0) {
      //temp2 = micros();
      sin2 = sin(2*PI*(micros()%period2)/period2);
    }
    if (amp3 != 0) {
     // temp3 = micros();
      sin3 = sin(2*PI*(micros()%period3)/period3);
    }
    
      sig = (ref + round(vd_coeff*(amp1 * sin1 + amp2 * sin2 + amp3 * sin3)/2));
    }
  
  /* Sinusoidal Pulse Generation
    if (micros() < (temppulse + pulse_train_duration)) {  
      sinp = sin(2*PI*(micros()%periodp)/periodp);
    } else {
      temppulse = 0;
    }
  }*/
      // Square Pulse Generation
      if ((micros() > (temppulse + pulse_spacing)) && pulse_cycle < 3){  
      temppulse = micros();
      sig = round(ref + pulseamp/2);
      if (sig > 255) { sig = 255; }
      dacWrite(pinSIG, sig);
      delay(pulse_duration/2);
      sig = round(ref - pulseamp/2);
      if (sig < 0) { sig = 0; }
      dacWrite(pinSIG, sig);
      delay(pulse_duration/2);
      pulse_cycle += 1;
    }

  // Only write a new voltage in pin 25 every time the value changes
  if (sig != sig_aux) {
    dacWrite(pinSIG, sig);  
    sig_aux = sig;
  }
     
}
