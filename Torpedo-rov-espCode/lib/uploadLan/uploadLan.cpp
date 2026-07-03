#include "uploadLan.h"
float kp = 0.0, ki = 0.0, kd = 0.0;
bool shouldReboot = false;

static float getQueryValue(const String& header, const String& key, float fallback) {
  int keyIndex = header.indexOf(key + "=");
  if (keyIndex == -1) {
    return fallback;
  }

  keyIndex += key.length() + 1;
  int endIndex = header.indexOf('&', keyIndex);
  String value = (endIndex == -1) ? header.substring(keyIndex) : header.substring(keyIndex, endIndex);
  return value.toFloat();
}

void sendIndexPage(EthernetClient& client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  
  client.print(
    "<!DOCTYPE html><html><head><title>Torpedo Control</title>"
    "<style>body{font-family:sans-serif;text-align:center;background:#1e1e1e;color:#fff;padding-top:50px;} "
    ".box{display:inline-block;padding:30px;border:1px solid #444;border-radius:10px;background:#2d2d2d; margin: 10px;}"
    ".navBar{position:fixed;top:0;left:0;width:100%;background:#333;color:#fff;padding:10px 0;display:flex;justify-content:center;box-shadow:0 2px 5px rgba(0,0,0,0.5);align-items:center;}"
    ".textLogo{font-size:30px;font-weight:bold;margin-right:20px;color:#ff0000;}"
    ".data-text{font-size: 24px; color: #00ffcc; font-weight: bold;}"
    "</style>"
    "<script>"
    "function upload() {"
    "  var file = document.getElementById('file').files[0];"
    "  if(!file) return alert('Select a file!');"
    "  document.getElementById('btn').disabled = true;"
    "  document.getElementById('status').innerText = 'Uploading... Please wait (Do not refresh)';\n"
    "  var xhr = new XMLHttpRequest();"
    "  xhr.open('POST', '/update', true);"
    "  xhr.onload = function() {"
    "     if(xhr.status == 200) {"
    "       document.getElementById('status').innerText = 'Success! Rebooting... Page will refresh shortly.';"
    "       setTimeout(function(){ window.location.reload(); }, 8000);"
    "     } else {"
    "       document.getElementById('status').innerText = 'Error: ' + xhr.responseText;"
    "       document.getElementById('btn').disabled = false;"
    "     }"
    "  };"
    "  xhr.onerror = function() { document.getElementById('status').innerText = 'Error: upload failed'; document.getElementById('btn').disabled = false; };"
    "  xhr.send(file);"
    "}"
    ""
    "function sendPID() {"
    "  var p = document.getElementById('input_kp').value;"
    "  var i = document.getElementById('input_ki').value;"
    "  var d = document.getElementById('input_kd').value;"
    "  document.getElementById('set_status').innerText = 'Sending...';"
    "  var xhr = new XMLHttpRequest();"
    "  xhr.open('POST', '/values?kp=' + p + '&ki=' + i + '&kd=' + d, true);"
    "  xhr.onload = function() {"
    "    if(xhr.status == 200) {"
    "      document.getElementById('set_status').innerText = 'Updated successfully!';"
    "      setTimeout(function(){ document.getElementById('set_status').innerText = ''; }, 2000);"
    "    } else {"
    "      document.getElementById('set_status').innerText = 'Failed to update.';"
    "    }"
    "  };"
    "  xhr.send();"
    "}"

    ""
    "function fetchLivePID() {"
    "  var xhr = new XMLHttpRequest();"
    "  xhr.open('GET', '/data', true);"
    "  xhr.onload = function() {"
    "    if(xhr.status == 200) {"
    "      var data = JSON.parse(xhr.responseText);"
    "      document.getElementById('kp_val').innerText = data.kp.toFixed(2);"
    "      document.getElementById('ki_val').innerText = data.ki.toFixed(2);"
    "      document.getElementById('kd_val').innerText = data.kd.toFixed(2);"
    "    } else {"
    "      document.getElementById('kp_val').innerText = '--';"
    "      document.getElementById('ki_val').innerText = '--';"
    "      document.getElementById('kd_val').innerText = '--';"
    "    }"
    "  };"
    "  xhr.onerror = function() {"
    "    document.getElementById('kp_val').innerText = '--';"
    "    document.getElementById('ki_val').innerText = '--';"
    "    document.getElementById('kd_val').innerText = '--';"
    "  };"
    "  xhr.send();"
    "}"
    "window.addEventListener('DOMContentLoaded', function() {"
    "  fetchLivePID();"
    "  setInterval(fetchLivePID, 1000);"
    "});"
    "</script></head>"
    "<body>"
    "<div class='navBar'><div class='textLogo'>Torpedo Control</div></div>"
    
    ""
    "<div class='box'><h2>Adaptive PID Live Stats</h2>"
    "<p>KP: <span id='kp_val' class='data-text'>0.00</span></p>"
    "<p>KI: <span id='ki_val' class='data-text'>0.00</span></p>"
    "<p>KD: <span id='kd_val' class='data-text'>0.00</span></p>"
    "</div>"

    "<div class='box'><h2>Update PID Values</h2>"
    "<p>KP: <input type='number' id='input_kp' step='0.01' style='width:80px; text-align:center; padding:5px;'></p>"
    "<p>KI: <input type='number' id='input_ki' step='0.01' style='width:80px; text-align:center; padding:5px;'></p>"
    "<p>KD: <input type='number' id='input_kd' step='0.01' style='width:80px; text-align:center; padding:5px;'></p>"
    "<button onclick='sendPID()' style='padding:10px 20px; cursor:pointer;'>Push to ROV</button>"
    "<p id='set_status' style='margin-top:20px; color:#aaa;'></p>"
    "</div>"

    ""
    "<div class='box'><h2>Firmware Update</h2>"
    "<input type='file' id='file' style='margin-bottom:20px;' accept='.bin'><br>"
    "<button id='btn' onclick='upload()' style='padding:10px 20px;cursor:pointer;'>Flash Device</button>"
    "<p id='status' style='margin-top:20px; color:#aaa;'></p>"
    "</div></body></html>"
  );
}

void handleOTA(EthernetClient& client, String header) {
  int lenIndex = header.indexOf("Content-Length: ");
  if (lenIndex == -1) {
    client.println("HTTP/1.1 411 Length Required\n\n");
    return;
  }
  
  uint32_t contentLength = header.substring(lenIndex + 16).toInt();
  Serial.printf("\nIncoming Firmware! Size: %u bytes\n", contentLength);

  if (!Update.begin(contentLength)) {
    Serial.println("ERROR: Not enough flash space.");
    client.println("HTTP/1.1 500 Internal Error\n\nNot Enough Space.");
    return;
  }

  Serial.println("Flashing...");
  
  uint32_t written = 0;
  uint8_t buffer[512];
  unsigned long lastDataTime = millis();
  while (written < contentLength) {
    if (client.available()) {
      int bytesRead = client.read(buffer, min((uint32_t)sizeof(buffer), contentLength - written));
      Update.write(buffer, bytesRead);
      written += bytesRead;
      lastDataTime = millis();
      yield();
      if (written % 51200 == 0 || written == contentLength) {
          Serial.printf("[OTA] Progress: %u / %u bytes\n", written, contentLength);
      }
    } else {
      if (millis() - lastDataTime > 5000) {
        Serial.println("\n[OTA] FATAL: Network Timeout! Connection dropped by router/chip.");
        break;
      }
    }
  }
  if (written == contentLength) {
    Serial.println("\n100% Data Received!");
    Serial.flush();
    yield();
    if (Update.end()) {
      Serial.println("Firmware Verified. Flagging for reboot...");
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/plain");
      client.println("Content-Length: 15");
      client.println("Connection: close");
      client.println();
      client.print("Update Success!");
      
      client.flush();
      shouldReboot = true;
    } else {
      Serial.printf("Flash Error: %s\n", Update.errorString());
      client.println("HTTP/1.1 500 Internal Error");
      client.println("Content-Type: text/plain");
      client.println("Connection: close");
      client.println();
      client.println("Flash Failed.");
    }
  } else {
    Serial.println("\nUpdate Aborted due to missing data.");
    client.println("HTTP/1.1 500 Internal Error");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();
    client.println("Data stream broken.");
    Update.end();
  }
}

void handleEthernetOTA(EthernetServer& server) {
  EthernetClient client = server.available();
  if (client) {
    Serial.println("\n[WEB] Browser Connected!");
    String header = "";
    boolean currentLineIsBlank = true;
    unsigned long timeoutTime = millis(); 

    while (client.connected() && (millis() - timeoutTime < 3000)) {
      if (client.available()) {
        char c = client.read();
        header += c;
        timeoutTime = millis(); 

        if (c == '\n' && currentLineIsBlank) {
          Serial.print("Request: ");
          Serial.println(header.substring(0, header.indexOf('\n')));

          if (header.startsWith("GET / ")) {
            sendIndexPage(client);
          } 
          else if (header.startsWith("GET /data")) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: application/json");
            client.println("Connection: close");
            client.println();
            client.print("{\"kp\":");
            client.print(kp, 2);
            client.print(",\"ki\":");
            client.print(ki, 2);
            client.print(",\"kd\":");
            client.print(kd, 2);
            client.println("}");
          }
          else if (header.startsWith("POST /values")) {
            float newKp = getQueryValue(header, "kp", kp);
            float newKi = getQueryValue(header, "ki", ki);
            float newKd = getQueryValue(header, "kd", kd);
            setValues(newKp, newKi, newKd);
            sendValues(newKp, newKi, newKd);
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/plain");
            client.println("Connection: close");
            client.println();
            client.print("PID Values Updated");
          }
          else if (header.startsWith("POST /update")) {
            handleOTA(client, header);
          }
          else {
            client.println("HTTP/1.1 404 Not Found");
            client.println("Connection: close");
            client.println();
          }
          break;
        }
        if (c == '\n') currentLineIsBlank = true;
        else if (c != '\r') currentLineIsBlank = false;
      }
    }
    while (client.available()) client.read();
    delay(10);
    client.stop();
  }
  if (shouldReboot) {
    Serial.println("Closing network connections and rebooting in 1 second...");
    delay(2000);
    ESP.restart();
  }
}
void sendValues(float kp_1, float ki_1, float kd_1) {
  kp = kp_1;
  ki = ki_1;
  kd = kd_1;
}
