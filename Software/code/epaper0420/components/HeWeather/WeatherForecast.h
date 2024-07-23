#ifndef _WEATHER_FORECAST_H_
#define _WEATHER_FORECAST_H_

#include <Arduino.h>
#include <ArduinoJson.h>



#define DEBUG // 调试用
 // 调试用
class WeatherForecast {
  public:
    WeatherForecast();
    void config(String userKey, String location, String unit, String lang);
    bool get();
    String getServerCode();
    String getLastUpdate();
    String getFxDate(int index);
    String getSunRise(int index);
    int getTempMax(int index);
    int getTempMin(int index);
    int getIconDay(int index);
    String getTextDay(int index);
    String getWindDirDay(int index);
    String getwindScaleDay(int index);
    int getHumidity(int index);
    float getPrecip(int index);
    int getUvIndex(int index);

  private:
    // const char* _host = "devapi.heweather.net"; // 服务器地址
    String _url;
    const int httpsPort = 443;

    // String _requserKey;  // 私钥
    // String _reqLocation; // 位置
    // String _reqUnit;     // 单位
    // String _reqLang;     // 语言

    void _parseNowJson(char* input, size_t inputLength); // 解析json信息
    // void _parseNowJson(String input);

    String _response_code = "no_init";           // API状态码
    String _last_update_str = "no_init";         // API最近更新时间
    String _daily_sunrise_str[7] = {"no_init", "no_init", "no_init","no_init", "no_init", "no_init", "no_init"}; // 日出时间
    String _daily_fxDate_str[7] = {"no_init", "no_init", "no_init","no_init", "no_init", "no_init", "no_init"}; // 预测的日期
    int _daily_tempMax_int[7] = {999, 999, 999,999, 999, 999,999}; // 最高气温
    int _daily_tempMin_int[7] = {999, 999, 999,999, 999, 999,999}; // 最低气温
    int _daily_iconDay_int[7] = {999, 999, 999,999, 999, 999,999}; // 白天天气状况的图标代码
    
    String _daily_textDay_str[7] = {"no_init", "no_init", "no_init","no_init", "no_init", "no_init", "no_init"};      // 天气状况的文字描述
    String _daily_windDirDay_str[7] = {"no_init", "no_init", "no_init","no_init", "no_init", "no_init", "no_init"};   // 白天风向
    String _daily_windScaleDay_str[7] = {"no_init", "no_init", "no_init","no_init", "no_init", "no_init", "no_init"}; // 白天风力等级
    int _daily_humidity_int[7] = {999, 999, 999,999, 999, 999,999};   // 相对湿度百分比数值
    float _daily_precip_float[7] = {999, 999, 999,999, 999, 999,999}; // 降水量，毫米
    int _daily_uvIndex_int[7] = {999, 999, 999,999, 999, 999,999};    // 紫外线强度指数

};
#endif
