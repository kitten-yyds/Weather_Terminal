#ifndef _AIRQUALITY_H
#define _AIRQUALITY_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClient.h>


// #define DEBUG   // 调试用

class AirQuality {
  public:
    AirQuality();

    void config(String userKey, String location, String unit, String lang);
    bool get();
    String getLastUpdate();
    String getServerCode();
    int getAqi();
    String getCategory();
    String getPrimary();
    String getpm10();
    String getpm2p5();
    String getno2();
    String getso2();
    String getco();
    String geto3();

  private:
    const char* _host = "api.heweather.local"; // 服务器地址
    const int httpsPort = 2053;

    String _requserKey;  // 私钥
    String _reqLocation; // 位置
    String _reqUnit;     // 单位
    String _reqLang;     // 语言
    String _url;

    void _parseNowJson(char* input, size_t inputLength);

    String _response_code =  "no_init";   // API状态码
    String _last_update_str = "no_init";  // 当前API最近更新时间
    int _now_aqi_int = 999;               // 实时空气质量指数
    String _now_category_str = "no_init"; // 实时空气质量指数级别
    String _now_primary_str = "no_init";  // 实时空气质量的主要污染物，优时返回 NA
    String _pm10 = "no_init";
    String _pm2p5 = "no_init";
    String _no2 = "no_init";
    String _so2 = "no_init";
    String _co = "no_init";
    String _o3 = "no_init";

};
#endif
