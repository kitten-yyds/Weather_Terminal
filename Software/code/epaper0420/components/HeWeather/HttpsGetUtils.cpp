#include "HttpsGetUtils.h"
// #include "ArduinoZlib.h" // gzip库
#include "ArduinoUZlib.h" // gzip库

HttpsGetUtils::HttpsGetUtils() {
}
void heap(){
  Serial.print("FREE_HEAP[");
  Serial.print(ESP.getFreeHeap());
  Serial.print("]\n");
}

void log(const char *str) {
  Serial.println(str);
}
/*
// 尝试获取信息，成功返回true，失败返回false
String HttpsGetUtils::get(String url) {
  // https请求
    WiFiClient client;
    HTTPClient http;

  #ifdef DEBUG
  Serial.print("[HTTP] begin...\n");
  #endif //DEBUG
  // String api = "http://192.168.2.180:8081";
  // String url = api + "/v7/weather/now?location=" + _reqLocation + "&key=" + _requserKey + "&unit=" + _reqUnit + "&lang=" + _reqLang;// + "&gzip=n";

  if (http.begin(client, url)) {  // HTTP连接成功
    #ifdef DEBUG
    Serial.print("[HTTP] GET...\n");
    #endif //DEBUG
    int httpCode = http.GET(); // 请求

    if (httpCode > 0) { // 错误返回负值
      #ifdef DEBUG
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      #endif //DEBUG
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) { // 服务器响应
        String payload = http.getString();
        #ifdef DEBUG
        Serial.printf("payload size=%d\n", payload.length());
        #endif //DEBUG
        return payload;
      }
    } else { // 错误返回负值
      #ifdef DEBUG
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      #endif //DEBUG
      return "";
    }
    http.end();
  } else { // HTTP连接失败
    #ifdef DEBUG
    Serial.printf("[HTTP] Unable to connect\n");
    #endif //DEBUG
    return "";
  }
}
*/
// 配置请求信息，私钥、位置、单位、语言
// void HttpsGetUtils::config(String userKey, String location, String unit, String lang) {
//   _requserKey = userKey;
//   _reqLocation = location;
//   _reqUnit = unit;
//   _reqLang = lang;
// }
bool HttpsGetUtils::fetchBuffer(const char *url) {
    _bufferSize=0;
    std::unique_ptr<WiFiClientSecure> client(new WiFiClientSecure);
    client->setInsecure();
    Serial.print("[HTTPS] begin...\n");
    HTTPClient https;
    if (https.begin(*client, url)) {
      https.addHeader("Accept-Encoding", "gzip");
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK) {

          // get length of document (is -1 when Server sends no Content-Length header)
          int len = https.getSize();

          // create buffer for read
          static uint8_t buff[256] = { 0 };//128

          // read all data from server
          int offset=0;
          //  为什么这里分配内存会报错？
          // if(inbuf==NULL) inbuf=(uint8_t*)malloc(sizeof(uint8_t)*128);
          while (https.connected() && (len > 0 || len == -1)) {
            // get available data size
            size_t size = client->available();
            
            if (size) {
              // read up to 128 byte
              int c = client->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
              // int c = client->readBytes(buff, size);
              // Serial.println("memcpy");
              memcpy(_buffer+offset, buff, sizeof(uint8_t)*c);
              offset+=c;
              // write it to Serial
              // Serial.write(buff, c);
              if (len > 0) {
                len -= c;
              } 
            }
            delay(1);
          }
          _bufferSize=offset;
          Serial.printf("offset=%d\n", offset);
          // Serial.write(_buffer, offset);
          Serial.print("[HTTPS] connection closed or file end.\n");
          
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
      https.end();

    } else {
      Serial.printf("Unable to connect\n");
    }
    return _bufferSize>0;
}

uint8_t HttpsGetUtils::_buffer[1280];//1280
const char* HttpsGetUtils::host = "https://devapi.qweather.com"; // 代理服务器地址
// const char* HttpsGetUtils::host = "https://192.168.2.144:8081"; // 代理服务器地址
size_t HttpsGetUtils::_bufferSize=0;
bool HttpsGetUtils::getString(const char* url, uint8_t *& outbuf, size_t &outlen) {
  fetchBuffer(url);
  Serial.printf("\nAfter fetch, buffer size=%d\n", _bufferSize);
  if(_bufferSize) {
    // write it to Serial 
    Serial.write(_buffer,_bufferSize);
    //outbuf=(uint8_t*)malloc(sizeof(uint8_t)*outlen);
    //if(outbuf==NULL) log("outbuf allocate failed!");
    ArduinoUZlib::decompress(_buffer, _bufferSize,outbuf, outlen);

    Serial.printf("outsize=%d\n", outlen);
    
    // Serial.write(outbuf,outlen);
    // Serial.printf("outlen :%d\n", outlen);
    _bufferSize=0;
    return true;
  }else {
    Serial.println("no avali size!");
  }
  return false;
  // if (outbuf!=NULL){
  //   free(outbuf);
  //   outbuf=NULL;
  // }
}



