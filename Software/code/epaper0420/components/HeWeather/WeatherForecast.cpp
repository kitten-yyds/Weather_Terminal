#include "WeatherForecast.h"
#include "HttpsGetUtils.h"
WeatherForecast::WeatherForecast() {
}

void WeatherForecast::config(String userKey, String location, String unit, String lang) {
  // _requserKey = userKey;
  // _reqLocation = location;
  // _reqUnit = unit;
  // _reqLang = lang;
    _url =  String(HttpsGetUtils::host) +  "/v7/weather/7d?location=" + location +
             "&key=" + userKey + "&unit=" + unit + "&lang=" + lang;// + "&gzip=n";
}

bool WeatherForecast::get() {
  /*
  // String url = "http://192.168.2.144:8082/v7/weather/3d";
  String str = HttpsGetUtils::get(url.c_str());
  _parseNowJson(str);
  return true;
  */

  
  //const char* url="https://192.168.2.144:8082/v7/weather/3d";
  uint8_t *outbuf=NULL;
  size_t len=0;
  bool result = HttpsGetUtils::getString(_url.c_str(), outbuf, len);
  Serial.printf("result=%d, len=%d", result, len);
  if(outbuf && len){
    //Serial.printf("write to serial, buf=%x, len=%d\n", outbuf, len);
    //Serial.write(outbuf, len);
    Serial.println("parse json");
    _parseNowJson((char*)outbuf,len);
  } else {
    Serial.println("获取7天天气失败了");
  }
  //一定要记得释放内存
  Serial.println("clean outbuf");
  if(outbuf!=NULL) {
    free(outbuf);
    outbuf=NULL;
    //Serial.printf("After clean, outbuf addr now=%x", outbuf);
  }
  return result;
  
}

// char*
void WeatherForecast::_parseNowJson(char *input, size_t inputLength) {
DynamicJsonDocument doc(6144);//1536
DeserializationError error = deserializeJson(doc, input, inputLength);

// String
// void WeatherForecast::_parseNowJson(String input) {
// DynamicJsonDocument doc(3072);
// DeserializationError error = deserializeJson(doc, input);


  if (error) {
    #ifdef DEBUG
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    #endif //DEBUG
    return;
  }

  const char* code = doc["code"];
  const char* updateTime = doc["updateTime"];

  _response_code = doc["code"].as<String>();
  _last_update_str = doc["updateTime"].as<String>();

  short i=0;
  for (JsonObject daily : doc["daily"].as<JsonArray>()){
    _daily_sunrise_str[i] = daily["sunrise"].as<String>();
    _daily_fxDate_str[i] = daily["fxDate"].as<String>();
    _daily_tempMax_int[i] = daily["tempMax"].as<int>();
    _daily_tempMin_int[i] = daily["tempMin"].as<int>();
    _daily_iconDay_int[i] = daily["iconDay"].as<int>();
    _daily_textDay_str[i] = daily["textDay"].as<String>();
    _daily_windDirDay_str[i] = daily["windDirDay"].as<String>();
    _daily_windScaleDay_str[i] = daily["windScaleDay"].as<String>();
    _daily_humidity_int[i] = daily["humidity"].as<int>();
    _daily_precip_float[i] = daily["precip"].as<float>();
    _daily_uvIndex_int[i] = daily["uvIndex"].as<int>();
    i++;
  }
  /*
  JsonArray daily = doc["daily"];
  JsonObject daily_0 = daily[0];
  JsonObject daily_1 = daily[1];
  JsonObject daily_2 = daily[2];

  _response_code = doc["code"].as<String>();
  _last_update_str = doc["updateTime"].as<String>();
  _daily_sunrise_str[0] = daily_0["sunrise"].as<String>();
  _daily_sunrise_str[1] = daily_1["sunrise"].as<String>();
  _daily_sunrise_str[2] = daily_2["sunrise"].as<String>();

  _daily_fxDate_str[0] = daily_0["fxDate"].as<String>();
  _daily_fxDate_str[1] = daily_1["fxDate"].as<String>();
  _daily_fxDate_str[2] = daily_2["fxDate"].as<String>();

  _daily_tempMax_int[0] = daily_0["tempMax"].as<int>();
  _daily_tempMax_int[1] = daily_1["tempMax"].as<int>();
  _daily_tempMax_int[2] = daily_2["tempMax"].as<int>();

  _daily_tempMin_int[0] = daily_0["tempMin"].as<int>();
  _daily_tempMin_int[1] = daily_1["tempMin"].as<int>();
  _daily_tempMin_int[2] = daily_2["tempMin"].as<int>();

  _daily_iconDay_int[0] = daily_0["iconDay"].as<int>();
  _daily_iconDay_int[1] = daily_1["iconDay"].as<int>();
  _daily_iconDay_int[2] = daily_2["iconDay"].as<int>();

  _daily_textDay_str[0] = daily_0["textDay"].as<String>();
  _daily_textDay_str[1] = daily_1["textDay"].as<String>();
  _daily_textDay_str[2] = daily_2["textDay"].as<String>();

  _daily_windDirDay_str[0] = daily_0["windDirDay"].as<String>();
  _daily_windDirDay_str[1] = daily_1["windDirDay"].as<String>();
  _daily_windDirDay_str[2] = daily_2["windDirDay"].as<String>();
  _daily_windScaleDay_str[0] = daily_0["windScaleDay"].as<String>();
  _daily_windScaleDay_str[1] = daily_1["windScaleDay"].as<String>();
  _daily_windScaleDay_str[2] = daily_2["windScaleDay"].as<String>();
  _daily_humidity_int[0] = daily_0["humidity"].as<int>();
  _daily_humidity_int[1] = daily_1["humidity"].as<int>();
  _daily_humidity_int[2] = daily_2["humidity"].as<int>();
  _daily_precip_float[0] = daily_0["precip"].as<float>();
  _daily_precip_float[1] = daily_1["precip"].as<float>();
  _daily_precip_float[2] = daily_2["precip"].as<float>();
  _daily_uvIndex_int[0] = daily_0["uvIndex"].as<int>();
  _daily_uvIndex_int[1] = daily_1["uvIndex"].as<int>();
  _daily_uvIndex_int[2] = daily_2["uvIndex"].as<int>();
  */
}

// 返回API状态码
String WeatherForecast::getServerCode() {
  return _response_code;
}

// 返回当前API最近更新时间
String WeatherForecast::getLastUpdate() {
  return _last_update_str;
}

String WeatherForecast::getFxDate(int index) {
  return _daily_fxDate_str[index];
}

// 返回日出时间
String WeatherForecast::getSunRise(int index) {
  return _daily_sunrise_str[index];
}

// 返回最高气温
int WeatherForecast::getTempMax(int index) {
  return _daily_tempMax_int[index];
}

// 返回最低气温
int WeatherForecast::getTempMin(int index) {
  return _daily_tempMin_int[index];
}

// 返回白天天气状况的图标代码
int WeatherForecast::getIconDay(int index) {
  return _daily_iconDay_int[index];
}

// 返回天气状况的文字描述
String WeatherForecast::getTextDay(int index) {
  return _daily_textDay_str[index];
}


// 返回白天风向
String WeatherForecast::getWindDirDay(int index) {
  return _daily_windDirDay_str[index];
}

// 返回白天风力等级
String WeatherForecast::getwindScaleDay(int index) {
  return _daily_windScaleDay_str[index];
}

// 返回当天相对湿度百分比数值
int WeatherForecast::getHumidity(int index) {
  return _daily_humidity_int[index];
}

// 返回当天降水量，毫米
float WeatherForecast::getPrecip(int index) {
  return _daily_precip_float[index];
}

// 返回当天紫外线强度指数
int WeatherForecast::getUvIndex(int index) {
  return _daily_uvIndex_int[index];
}

