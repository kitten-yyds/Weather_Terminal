#include "Weather24h.h"
#include "HttpsGetUtils.h"

Weather24h::Weather24h() {
}

void Weather24h::config(String userKey, String location, String unit, String lang) {
  // _requserKey = userKey;
  // _reqLocation = location;
  // _reqUnit = unit;
  // _reqLang = lang;
  // _uri = HttpsGetUtils::host +  "/v7/weather/24h?location=" + _reqLocation +
  //            "&key=" + _requserKey + "&unit=" + _reqUnit + "&lang=" + _reqLang;// + "&gzip=n";
  _url =  String(HttpsGetUtils::host) +  "/v7/weather/24h?location=" + location +
             "&key=" + userKey + "&unit=" + unit + "&lang=" + lang;// + "&gzip=n";
}



bool Weather24h::get() {
  // http请求
  #ifdef DEBUG
  Serial.print("[HTTP] begin...\n");
  #endif //DEBUG

  // String url = "http://192.168.2.144:8082/v7/weather/24h";
  //String str = HttpsGetUtils::get(url.c_str());
  //_parseNowJson(str);
  //return true;


//HTTPS请求+解压GZIP，内存不足                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               

  //const char *url="https://192.168.2.144:8082/v7/weather/24h";
  uint8_t *outbuf=NULL;
  size_t len=0;
  bool result = HttpsGetUtils::getString(_url.c_str(), outbuf, len);
  Serial.printf("result=%d, len=%d", result, len);
  if(outbuf && len){
    //Serial.printf("write to serial, buf=%x, len=%d\n", outbuf, len);
    // Serial.write(outbuf, len);
    Serial.println("parse json");
      _parseNowJson((char*)outbuf,len);
  } else {
    Serial.println("获取24小时天气失败了");
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




void Weather24h::_parseNowJson(char *input, size_t inputLength) {
  // void Weather24h::_parseNowJson(String input) {
  // std::string input;

// DynamicJsonDocument doc(8192);
// DeserializationError error = deserializeJson(doc, input);
DynamicJsonDocument doc(6144);
DeserializationError error = deserializeJson(doc, input, inputLength);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

_response_code = doc["code"].as<String>(); // "200"
_last_update_str  = doc["updateTime"].as<String>(); // "2022-12-05T15:35+08:00"
// const char* fxLink = doc["fxLink"]; // "http://hfx.link/1u0r1"

short i=0;
for (JsonObject hourly_item : doc["hourly"].as<JsonArray>()) {
 _hourly_item_fxTime[i] = hourly_item["fxTime"].as<String>(); // "2022-12-05T17:00+08:00", ...
 _hourly_item_temp_int[i] = hourly_item["temp"].as<int>(); // "15", "13", "13", "12", "11", "11", "10", "10", ...
  _hourly_item_icon_int[i] = hourly_item["icon"].as<int>(); // "100", "150", "150", "150", "150", "150", "150", ...
  _hourly_item_text[i] = hourly_item["text"].as<String>(); // "晴", "晴", "晴", "晴", "晴", "晴", "晴", "多云", "多云", ...
  //Serial.printf("%s,", _hourly_item_text[i]);
  // const char* hourly_item_wind360 = hourly_item["wind360"]; // "22", "24", "30", "33", "33", "31", "30", ...
  // const char* hourly_item_windDir = hourly_item["windDir"]; // "东北风", "东北风", "东北风", "东北风", "东北风", "东北风", ...
  // const char* hourly_item_windScale = hourly_item["windScale"]; // "3-4", "3-4", "3-4", "3-4", "3-4", ...
  // const char* hourly_item_windSpeed = hourly_item["windSpeed"]; // "16", "16", "16", "16", "14", "14", ...
  _hourly_item_humidity_int[i] = hourly_item["humidity"].as<int>(); // "57", "63", "63", "65", "66", "67", "68", ...
  // const char* hourly_item_pop = hourly_item["pop"]; // "1", "3", "6", "6", "6", "6", "6", "6", "7", "7", ...
  _hourly_item_precip_float[i] = hourly_item["precip"].as<float>(); // "0.0", "0.0", "0.0", "0.0", "0.0", "0.0", ...
  // const char* hourly_item_pressure = hourly_item["pressure"]; // "1013", "1013", "1012", "1012", "1012", ...
  // const char* hourly_item_cloud = hourly_item["cloud"]; // "5", "5", "4", "4", "7", "9", "11", "33", "54", ...
  // const char* hourly_item_dew = hourly_item["dew"]; // "7", "6", "6", "6", "5", "5", "5", "5", "5", "4", ...
    Serial.print(String(int(_hourly_item_humidity_int[i]))+",");
  i++;
}

// JsonArray refer_sources = doc["refer"]["sources"];
// const char* refer_sources_0 = refer_sources[0]; // "QWeather"
// const char* refer_sources_1 = refer_sources[1]; // "NMC"
// const char* refer_sources_2 = refer_sources[2]; // "ECMWF"

// const char* refer_license_0 = doc["refer"]["license"][0]; // "CC BY-SA 4.0"
Serial.println();
}

// 返回API状态码
String Weather24h::getServerCode() {
  return _response_code;
}

// 返回当前API最近更新时间
String Weather24h::getLastUpdate() {
  return _last_update_str;
}

String Weather24h::getFxDate(int index) {
  return _hourly_item_fxTime[index];
}

int Weather24h::getTemp(int index) {
  return _hourly_item_temp_int[index];
}
int Weather24h::getIconDay(int index){
  return _hourly_item_icon_int[index];
}
String Weather24h::getTextDay(int index) {
  return _hourly_item_text[index];
}
int Weather24h::getHumidity(int index) {
  return _hourly_item_humidity_int[index];
}
float Weather24h::getPrecip(int index) {
  return _hourly_item_precip_float[index];
}