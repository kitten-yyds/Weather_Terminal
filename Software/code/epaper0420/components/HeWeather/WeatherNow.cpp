#include "WeatherNow.h"
#include "HttpsGetUtils.h"
WeatherNow::WeatherNow() {
}

// 配置请求信息，私钥、位置、单位、语言
void WeatherNow::config(String userKey, String location, String unit, String lang) {
  // _requserKey = userKey;
  // _reqLocation = location;
  // _reqUnit = unit;
  // _reqLang = lang;

    _url =  String(HttpsGetUtils::host) +  "/v7/weather/now?location=" + location +
             "&key=" + userKey + "&unit=" + unit + "&lang=" + lang;// + "&gzip=n";
}

// 尝试获取信息，成功返回true，失败返回false
bool WeatherNow::get() {
  // #ifdef DEBUG
  // Serial.print("[HTTP] begin...\n");
  // #endif DEBUG
  
  // String api = "https://devapi.qweather.com";
  // String url = api + "/v7/weather/now?location=" + _reqLocation +
  //             "&key=" + _requserKey + "&unit=" + _reqUnit + "&lang=" + _reqLang;// + "&gzip=n";
  /*
  // String url = "http://192.168.2.144:8082/v7/weather/now";
  String str = HttpsGetUtils::get(url.c_str());
  _parseNowJson(str);
  return true;
  */

  
  // const char *url = "https://192.168.2.144:8082/v7/weather/now";
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
    Serial.println("获取当前天气失败了");
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

// 解析Json信息
void WeatherNow::_parseNowJson(char* input, size_t inputLength) {
StaticJsonDocument<512> doc;
DeserializationError error = deserializeJson(doc, input, inputLength);

// void WeatherNow::_parseNowJson(String input) {
// StaticJsonDocument<768> doc;
// DeserializationError error = deserializeJson(doc, input);


  JsonObject now = doc["now"];

  _response_code = doc["code"].as<String>();            // API状态码
  _last_update_str = doc["updateTime"].as<String>();    // 当前API最近更新时间
  _now_temp_int = now["temp"].as<int>();                // 实况温度
  _now_feelsLike_int = now["feelsLike"].as<int>();      // 实况体感温度
  _now_icon_int = now["icon"].as<int>();                // 当前天气状况和图标的代码
  _now_text_str = now["text"].as<String>();             // 实况天气状况的文字描述
  _now_windDir_str = now["windDir"].as<String>();       // 实况风向
  _now_windScale_int = now["windSpeed"].as<int>();      // 实况风力等级
  _now_humidity_float = now["humidity"].as<float>();    // 实况相对湿度百分比数值
  _now_precip_float = now["precip"].as<float>();        // 实况降水量,毫米
}

// API状态码
String WeatherNow::getServerCode() {
  return _response_code;
}

// 当前API最近更新时间
String WeatherNow::getLastUpdate() {
  return _last_update_str;
}

// 实况温度
int WeatherNow::getTemp() {
  return _now_temp_int;
}

// 实况体感温度
int WeatherNow::getFeelLike() {
  return _now_feelsLike_int;
}

// 当前天气状况和图标的代码
int WeatherNow::getIcon() {
  return _now_icon_int;
}

// 实况天气状况的文字描述
String WeatherNow::getWeatherText() {
  return _now_text_str;
}

// 实况风向
String WeatherNow::getWindDir() {
  return _now_windDir_str;
}

// 实况风力等级
int WeatherNow::getWindScale() {
  return _now_windScale_int;
}

// 实况相对湿度百分比数值
float WeatherNow::getHumidity() {
  return _now_humidity_float;
}
// 实况降水量,毫米
float WeatherNow::getPrecip() {
  return _now_precip_float;
}