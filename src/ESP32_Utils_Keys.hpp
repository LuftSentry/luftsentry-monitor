void InitKeys(){
  String Read_rootca;
  String Read_cert;
  String Read_privatekey;

  if (!SPIFFS.begin(true)) {
    Serial.println("Se ha producido un error al montar SPIFFS");
    return;
  }
//**********************
  //Root CA leer archivo.
  File file2 = SPIFFS.open("/AmazonRootCA1.pem", "r");
  if (!file2) {
    Serial.println("No se pudo abrir el archivo para leerlo");
    return;
  }
  Serial.println("Reading file 2");
  while (file2.available()) {
    Read_rootca = file2.readString();
  }
  //*****************************
  // Cert leer archivo
  File file4 = SPIFFS.open("/api_server.cert.pem", "r");
  if (!file4) {
    Serial.println("No se pudo abrir el archivo para leerlo");
    return;
  }
  Serial.println("Reading file 4");
  while (file4.available()) {
    Read_cert = file4.readString();
  }
  //***************************************
  //Privatekey leer archivo
  File file6 = SPIFFS.open("/api_server.private.key", "r");
  if (!file6) {
    Serial.println("No se pudo abrir el archivo para leerlo");
    return;
  }
  Serial.println("Reading file 6");
  while (file6.available()) {
    Read_privatekey = file6.readString();
  }


  pRead_rootca = (char *)malloc(sizeof(char) * (Read_rootca.length() + 1));
  strcpy(pRead_rootca, Read_rootca.c_str());


  pRead_cert = (char *)malloc(sizeof(char) * (Read_cert.length() + 1));
  strcpy(pRead_cert, Read_cert.c_str());

  pRead_privatekey = (char *)malloc(sizeof(char) * (Read_privatekey.length() + 1));
  strcpy(pRead_privatekey, Read_privatekey.c_str());

  delay(2000);
}