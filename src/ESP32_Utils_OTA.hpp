String getHeaderValue(String header, String headerName) {
  return header.substring(strlen(headerName.c_str()));
}

void firmwareUpdate() {
  WiFiClient client;

  long contentLength = 0;
  bool isValidContentType = false;
  Serial.println("Connecting to: " + String(host));

  if (client.connect(host.c_str(), port)) {
    Serial.println("Fetching Bin: " + String(bin));

    client.print(String("GET ") + bin + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Cache-Control: no-cache\r\n" +
                 "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println("Client Timeout !");
        client.stop();
        return;
      }
    }

    while (client.available()) {

      String line = client.readStringUntil('\n');
      line.trim();

      if (!line.length()) {
        break;
      }

      if (line.startsWith("HTTP/1.1")) {
        if (line.indexOf("200") < 0) {
          Serial.println("Got a non 200 status code from server. Exiting OTA Update.");
          break;
        }
      }

      if (line.startsWith("Content-Length: ")) {
        contentLength = atol((getHeaderValue(line, "Content-Length: ")).c_str());
        Serial.println("Got " + String(contentLength) + " bytes from server");
      }

      if (line.startsWith("Content-Type: ")) {
        String contentType = getHeaderValue(line, "Content-Type: ");
        Serial.println("Got " + contentType + " payload.");
        if (contentType == "application/octet-stream") {
          isValidContentType = true;
        }
      }
    }
  } else {

    Serial.println("Connection to " + String(host) + " failed. Please check your setup");
    // firmwareUpdate();
  }

  Serial.println("contentLength : " + String(contentLength) + ", isValidContentType : " + String(isValidContentType));

  if (contentLength && isValidContentType) {
    bool canBegin = Update.begin(contentLength);

    if (canBegin) {
      Serial.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");
      size_t written = Update.writeStream(client);

      if (written == contentLength) {
        Serial.println("Written : " + String(written) + " successfully");
      } else {
        Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?" );
        // firmwareUpdate();
      }

      if (Update.end()) {
        Serial.println("OTA done!");
        if (Update.isFinished()) {
          Serial.println("Update successfully completed. Rebooting.");
          ESP.restart();
        } else {
          Serial.println("Update not finished? Something went wrong!");
        }
      } else {
        Serial.println("Error Occurred. Error #: " + String(Update.getError()));
      }
    } else {
      Serial.println("Not enough space to begin OTA");
      client.flush();
    }
  } else {
    Serial.println("There was no content in the response");
    client.flush();
  }
}

void downloadDataFiles() {
  if (!SPIFFS.exists("/data")) {
    SPIFFS.mkdir("/data");
  }

  for (int i = 0; i < num_files; i++) {
    Serial.println("Check file "+String(filenames[i]));
    if (SPIFFS.exists(filenames[i])) {
      continue;
    }

    HTTPClient http;
    http.begin(urls[i]);
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      File file = SPIFFS.open(filenames[i], "w");
      if (!file) {
        Serial.println("Error al abrir el archivo " + String(filenames[i]) + " para escritura.");
        continue;
      }
      file.write(http.getStream());
      file.close();
      Serial.println("El archivo " + String(filenames[i]) + " se ha guardado correctamente.");
    } else {
      Serial.println("Error al descargar el archivo " + String(filenames[i]) + ". Código HTTP: " + String(httpCode));
    }
    http.end();
  }
}

bool checkFirmwareUpdate() {
  HTTPClient http;
  http.begin(FIRMWARE_URL);
  int httpResponseCode = http.GET();
  
  if (httpResponseCode == 200) {
    String response = http.getString();
    DynamicJsonDocument json(1024);
    deserializeJson(json, response);
    String latestVersion = json["version"];

    if (latestVersion > FIRMWARE_VERSION) {
      return true;
    }
  }
  return false;
}

void checkDataFolder(){
  if(!SPIFFS.begin(true)){
    Serial.println("Error al montar SPIFFS");
    return;
  }
  Serial.println(!SPIFFS.exists("/data"));
  if(!SPIFFS.exists("/data")){
    downloadDataFiles();
  } else {
    //updateDataFiles();
  }
}

void InitOTA(){
  checkDataFolder();
}
