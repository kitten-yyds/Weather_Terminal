#ifndef _WEATHER_24_H_
#define _WEATHER_24_H_

#include <Arduino.h>
#include <ArduinoJson.h>



#define DEBUG // 调试用

class Weather24h {
  public:
    Weather24h();
    void config(String userKey, String location, String unit, String lang);
    bool get();
    String getServerCode();
    String getLastUpdate();
    String getFxDate(int index);
    int getTemp(int index);
    int getIconDay(int index);
    String getTextDay(int index);
    int getHumidity(int index);
    float getPrecip(int index);

  private:
    //const char* _host = "devapi.heweather.net"; // 服务器地址
    const int httpsPort = 443;
    String _url;

    // String _requserKey;  // 私钥
    // String _reqLocation; // 位置
    // String _reqUnit;     // 单位
    // String _reqLang;     // 语言

    //void _parseNowJson(String input); // 解析json信息
     void _parseNowJson(char *input, size_t inputLength); // 解析json信息


    String _response_code = "no_init";           // API状态码
    String _last_update_str = "no_init";         // API最近更新时间

     // 每小时温度
    int _hourly_item_temp_int[24] = {99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99};
    int _hourly_item_icon_int[24] = {99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99};
    String _hourly_item_text[24] = {"","","","","","","","","","","","","","","","","","","","","","","",""};
    int _hourly_item_humidity_int[24]={99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99};
     // 预测的日期
    String _hourly_item_fxTime[24] = {"","","","","","","","","","","","","","","","","","","","","","","",""};
    float _hourly_item_precip_float[24] = {99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99}; // 降水量，毫米
};
#endif
