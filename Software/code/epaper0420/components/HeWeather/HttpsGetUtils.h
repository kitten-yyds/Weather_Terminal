#ifndef _HTTPS_GET_UTILS_H_
#define _HTTPS_GET_UTILS_H_

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>//BearSSL.h>


#define DEBUG // 调试用

class HttpsGetUtils {

  
  public:
    HttpsGetUtils();

    static bool getString(const char* url, uint8_t*& outbuf, size_t &len);
    //static String get(String str);
    static const char  *host;

  private:
    static bool fetchBuffer(const char* url);
    static const char* _host; // 服务器地址
    
    const int httpsPort = 2053;

    static uint8_t _buffer[1280]; //gzip流最大缓冲区1280
    static size_t _bufferSize;

};

#endif
