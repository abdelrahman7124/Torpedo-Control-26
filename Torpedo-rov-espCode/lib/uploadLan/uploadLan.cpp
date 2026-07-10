#include "uploadLan.h"

void sendIndexPage(EthernetClient& client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  
  client.print(
    "<!DOCTYPE html><html><head><title>ESP32 OTA</title>"
    "<style>body{font-family:sans-serif;text-align:center;background:#1e1e1e;color:#fff;padding-top:50px;} "
    ".box{display:inline-block;padding:30px;border:1px solid #444;border-radius:10px;background:#2d2d2d;}"
    ".navBar{position:fixed;top:0;left:0;width:100%;background:#333;color:#fff;padding:10px 0;display:flex;justify-content:center;box-shadow:0 2px 5px rgba(0,0,0,0.5);align-items:center;}"
    ".textLogo{font-size:30px;font-weight:bold;margin-right:20px;color:#ff0000;}"
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
    "       document.getElementById('status').innerText = 'Success! Device Rebooting...';"
    "     } else {"
    "       document.getElementById('status').innerText = 'Error: ' + xhr.responseText;"
    "       document.getElementById('btn').disabled = false;"
    "     }"
    "  };"
    "  xhr.send(file);"
    "}"
    "</script></head>"
    "<body>"
    "<div class='navBar'>"
    "<div class='textLogo'>Torpedo OTA</div>"
    "</div>"
    "<div class='box'><h2>Firmware Update</h2>"
    "<input type='file' id='file' style='margin-bottom:20px;' accept='.bin'><br>"
    "<p>uploaded<p>"
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
    if (Update.end()) {
      Serial.println("Firmware Verified. Rebooting!");
      client.println("HTTP/1.1 200 OK\n\nUpdate Success!");
      client.flush();
      delay(1000);
      ESP.restart();
    } else {
      Serial.printf("Flash Error: %s\n", Update.errorString());
      client.println("HTTP/1.1 500 Internal Error\n\nFlash Failed.");
    }
  } else {
    Serial.println("\nUpdate Aborted due to missing data.");
    client.println("HTTP/1.1 500 Internal Error\n\nData stream broken.");
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
          else if (header.startsWith("POST /update")) {
            handleOTA(client, header);
          } 
          else {
            client.println("HTTP/1.1 404 Not Found\nConnection: close\n");
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
}