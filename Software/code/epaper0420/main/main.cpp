/*导入库*/
#include "Arduino.h"//Arduino组件
#include "logo_550W.h" //550W logo
#include "weather_icon.h"//天气图片库，图片来自于和风天气
#include "time.h" //ESP32 RTC时钟库
#include "GxEPD2_3C.h" //墨水屏驱动库，扩展了SSD1683 IC
#include "U8g2_for_Adafruit_GFX.h" //图像库
#include <Preferences.h> //ESP32 NVS储存
#include <WiFi.h> //ESP32 WiFi
#include <WebServer.h> //服务器库，用于配网
#include "Heweather.h" //和风天气库，带gzip
#include "axp20x.h"//电源芯片库
#include "esp_log.h"//串口打印
#include "NTPClient.h"//NTP时间获取
#include "WiFiUdp.h"//Udp库，用于NTP时间获取

/*用户配置 config*/
#define HEAD_TEXT "550W"
#define AP_NAME "Epaper"
#define NTP_URL "ntp.aliyun.com"
#define TIME_ZONE 8
#define PMU_SDA 21
#define PMU_SCL 22
#define DISPLAY_CS 5
#define DISPLAY_DC 19
#define DISPLAY_RST 13
#define DISPLAY_BUSY 4
#define WOKE_GOIO GPIO_NUM_36

/*实例化对象*/
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;
GxEPD2_3C<GxEPD2_420c_1680, GxEPD2_420c_1680::HEIGHT> display(GxEPD2_420c_1680(DISPLAY_CS,DISPLAY_DC,DISPLAY_RST,DISPLAY_BUSY));  //显示屏实例化
WebServer server(80);
Preferences nvs;
WeatherNow weatherNow; 
WeatherForecast weatherforecast;
AirQuality airquality;
Weather24h weather24h;
AXP20X_Class axp202;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_URL , TIME_ZONE*3600, 60000);

/*导入库*/
#include "html.h"//网页库
#include "display_weather_icon.h"//天气代码转换天气图片库

/*创建变量*/
const String WDAY_NAMES[] = {"星期天", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"};//星期
RTC_DATA_ATTR bool is_System_has_on = 0;//用于保存系统是否已经启动过，用于开机界面
const uint8_t slave_address = AXP202_SLAVE_ADDRESS;//AXP202 IIC地址

/*提前声明函数*/
void time_update();//时间更新函数
void AXP202_init();//AXP202初始化函数
void display_setup();//开机界面UI函数
void setting_mode(int why);//进入设置模式[配网]函数
void time_wokeup();//设置整点唤醒定时唤醒函数
void gpio_wokeup();//设置GPIO唤醒函数
void weather_update();//天气数据更新函数
void display_main();//主页面显示函数
void display_at_middle(int x,int y,String text);//坐标在中间显示字符串
void check_bat();//检测电池是否有电
int check_wokeup_reason();//检查唤醒原因
int get_display_long(String text);//查看文字显示长度
String plus_0(int num); //补零程序
String fxtime24_handle(String fxtime);//24h天气时间提取函数
String fxtimeforecast_handle(String fxtime);//未来天气时间提取函数
int get_max(int num1,int num2,int num3,int num4,int num5);//获取最大值
int get_min(int num1,int num2,int num3,int num4,int num5);//获取最小值
void app_init();//各种开机[唤醒]初始化函数

struct tm timeinfo;

void setup(){
  app_init();//各种开机[唤醒]初始化
  ESP_LOGI("weather","get...");
  weather_update();//天气更新
  ESP_LOGI("weather","get...done");
  ESP_LOGI("time","updating...");
  time_update();//时间更新同步
  ESP_LOGI("time","updating...done");
  WiFi.disconnect();//断开WiFi
  ESP_LOGI("wifi","disconnect");
  ESP_LOGI("display","display the main paper");
  display_main();//显示主页面
  ESP_LOGI("woke","setting gpio wokeup...");
  gpio_wokeup();//设置gpio唤醒
  ESP_LOGI("woke","setting gpio wokeup...done");
  ESP_LOGI("woke","setting timee wokeup...");
  time_wokeup();//设置时间唤醒
  ESP_LOGI("woke","setting time wokeup...done");
  ESP_LOGI("sleep","sleep...zzz");
  esp_deep_sleep_start();//开始进入深睡眠
}

void loop(){
  
}

/*--------------------------------------------------------------------------------------------------*/

void display_main(){
  display.init();
  display.setFullWindow();
  display.firstPage();
  do{
    /*主框架辅助线*/
    //display.drawLine(100,0,100,300,GxEPD_BLACK);
    //display.drawLine(200,0,200,300,GxEPD_BLACK);
    //display.drawLine(300,0,300,300,GxEPD_BLACK);
    //display.drawLine(0,100,400,100,GxEPD_BLACK);
    //display.drawLine(0,200,400,200,GxEPD_BLACK);
    /*框架线*/
    display.drawLine((display.width()/3)+1,18,(display.width()/3)+1,279,GxEPD_RED);
    display.drawLine(0,206,(display.width()/3),206,GxEPD_RED);
    display.drawLine(0,280,display.width(),280,GxEPD_RED);
    display.drawLine((display.width()/3)+1,149,display.width(),149,GxEPD_RED);
    /*上栏*/
    u8g2Fonts.setForegroundColor(GxEPD_WHITE);   // 设置前景色
    u8g2Fonts.setBackgroundColor(GxEPD_RED);   // 设置背景色
    display.fillRect(2,2,display.width(),14,GxEPD_RED);
    u8g2Fonts.setFont(u8g2_font_wqy12_t_gb2312);
    u8g2Fonts.setCursor(4,13);
    u8g2Fonts.print("更新时间 "+plus_0(timeinfo.tm_hour)+":"+plus_0(timeinfo.tm_min));
    u8g2Fonts.setCursor(335,13);
    if (axp202.isBatteryConnect()){
      u8g2Fonts.print("电量 " + String(axp202.getBattPercentage()) + " %");
    }else{
      u8g2Fonts.print(String("电量 -- %"));
    }
    display_at_middle(200,13,HEAD_TEXT);
    /*日期*/
    display.fillRect(2,18,display.width()/3,90,GxEPD_RED);
    u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312);
    u8g2Fonts.setCursor(4,36);
    u8g2Fonts.print(String(timeinfo.tm_year+1900)+"年"+plus_0(timeinfo.tm_mon+1)+"月");
    u8g2Fonts.setCursor(85,104);
    u8g2Fonts.print(WDAY_NAMES[timeinfo.tm_wday]);
    u8g2Fonts.setCursor(110,85);
    u8g2Fonts.print("日");
    u8g2Fonts.setFont(u8g2_font_fub42_tn);
    u8g2Fonts.setCursor(35,85);
    u8g2Fonts.print(plus_0(timeinfo.tm_mday));
    /*现在天气*/
    display_weather_icon(2,110,weatherNow.getIcon());
    u8g2Fonts.setForegroundColor(GxEPD_BLACK);   // 设置前景色
    u8g2Fonts.setBackgroundColor(GxEPD_WHITE);   // 设置背景色
    u8g2Fonts.setFont(u8g2_font_fub20_tn);
    display_at_middle(100-6,140,String(weatherNow.getTemp()));
    u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312);
    display_at_middle(100-6,158,weatherNow.getWeatherText());
    u8g2Fonts.setCursor(4,183);
    u8g2Fonts.print("湿度 " + String(int(weatherNow.getHumidity())));
    u8g2Fonts.setCursor(70,183);
    u8g2Fonts.print("降水 " + String(int(weatherNow.getPrecip())));
    u8g2Fonts.setCursor(4,201);
    u8g2Fonts.print(weatherNow.getWindDir() + " " + String(weatherNow.getWindScale()) + " km/h");
    /*空气质量*/
    if(get_display_long("空气指数 " + String(airquality.getAqi())+" "+airquality.getCategory()) > 127){
      u8g2Fonts.setCursor(4,224);
      u8g2Fonts.print("空气" + airquality.getCategory());
      u8g2Fonts.setCursor(4,242);
      u8g2Fonts.print("空气指数 " + String(airquality.getAqi()));
    }else{
      display_at_middle(display.width()/6,224,"空气质量");
      u8g2Fonts.setCursor(4,242);
      u8g2Fonts.print("空气指数 " + String(airquality.getAqi())+" "+airquality.getCategory());
    }
    
    u8g2Fonts.setCursor(4,260);
    if(airquality.getPrimary() =="NA"){
      u8g2Fonts.print("主污染物 无");
    }else{
      u8g2Fonts.print("主污染物 "+airquality.getPrimary());
    }
    u8g2Fonts.setCursor(4,278);
    if(airquality.getPrimary() == "PM2.5"){
      u8g2Fonts.print("PM2.5 "+airquality.getpm2p5());
    }else if(airquality.getPrimary() == "PM10"){
      u8g2Fonts.print("PM10 "+airquality.getpm10());
    }else if(airquality.getPrimary() == "NO2"){
      u8g2Fonts.print("NO2 "+airquality.getno2());
    }else if(airquality.getPrimary() == "SO2"){
      u8g2Fonts.print("SO2 "+airquality.getso2());
    }else if(airquality.getPrimary() == "CO"){
      u8g2Fonts.print("CO "+airquality.getco());
    }else if(airquality.getPrimary() == "O3"){
      u8g2Fonts.print("O3 "+airquality.geto3());
    }else{
      u8g2Fonts.print("PM2.5 "+airquality.getpm2p5());
    }
    /*下栏*/   
    display_at_middle(display.width()/2,297,nvs.getString("text"));
    /*图表辅助线*/
    //display.drawLine(160,0,160,300,GxEPD_RED);
    //display.drawLine(213,0,213,300,GxEPD_RED);
    //display.drawLine(266,0,266,300,GxEPD_RED);
    //display.drawLine(320,0,320,300,GxEPD_RED);
    //display.drawLine(373,0,373,300,GxEPD_RED);
    /*5h天气*/
    display_at_middle(160,32,fxtime24_handle(weather24h.getFxDate(0))+"时");
    display_at_middle(213,32,fxtime24_handle(weather24h.getFxDate(1))+"时");
    display_at_middle(266,32,fxtime24_handle(weather24h.getFxDate(2))+"时");
    display_at_middle(320,32,fxtime24_handle(weather24h.getFxDate(3))+"时");
    display_at_middle(373,32,fxtime24_handle(weather24h.getFxDate(4))+"时");
    display_at_middle(160,50,weather24h.getTextDay(0));
    display_at_middle(213,50,weather24h.getTextDay(1));
    display_at_middle(266,50,weather24h.getTextDay(2));
    display_at_middle(320,50,weather24h.getTextDay(3));
    display_at_middle(373,50,weather24h.getTextDay(4));
    display_at_middle(160,68,String(weather24h.getTemp(0)));
    display_at_middle(213,68,String(weather24h.getTemp(1)));
    display_at_middle(266,68,String(weather24h.getTemp(2)));
    display_at_middle(320,68,String(weather24h.getTemp(3)));
    display_at_middle(373,68,String(weather24h.getTemp(4)));
    int Temp_max = get_max(weather24h.getTemp(0),weather24h.getTemp(1),weather24h.getTemp(2),weather24h.getTemp(3),weather24h.getTemp(4));
    int Temp_min = get_min(weather24h.getTemp(0),weather24h.getTemp(1),weather24h.getTemp(2),weather24h.getTemp(3),weather24h.getTemp(4));
    if(Temp_max == Temp_min){
      display.drawLine(160,107,373,107,GxEPD_BLACK);
    }else{
      display.drawLine(160,map(weather24h.getTemp(0),Temp_min,Temp_max,140,75),213,map(weather24h.getTemp(1),Temp_min,Temp_max,140,75),GxEPD_BLACK);
      display.drawLine(213,map(weather24h.getTemp(1),Temp_min,Temp_max,140,75),266,map(weather24h.getTemp(2),Temp_min,Temp_max,140,75),GxEPD_BLACK);
      display.drawLine(266,map(weather24h.getTemp(2),Temp_min,Temp_max,140,75),320,map(weather24h.getTemp(3),Temp_min,Temp_max,140,75),GxEPD_BLACK);
      display.drawLine(320,map(weather24h.getTemp(3),Temp_min,Temp_max,140,75),373,map(weather24h.getTemp(4),Temp_min,Temp_max,140,75),GxEPD_BLACK);
    }
    
    /*5day天气*/
    display_at_middle(160,168,fxtimeforecast_handle(weatherforecast.getFxDate(0)));
    display_at_middle(213,168,fxtimeforecast_handle(weatherforecast.getFxDate(1)));
    display_at_middle(266,168,fxtimeforecast_handle(weatherforecast.getFxDate(2)));
    display_at_middle(320,168,fxtimeforecast_handle(weatherforecast.getFxDate(3)));
    display_at_middle(373,168,fxtimeforecast_handle(weatherforecast.getFxDate(4)));
    display_at_middle(160,186,weatherforecast.getTextDay(0));
    display_at_middle(213,186,weatherforecast.getTextDay(1));
    display_at_middle(266,186,weatherforecast.getTextDay(2));
    display_at_middle(320,186,weatherforecast.getTextDay(3));
    display_at_middle(373,186,weatherforecast.getTextDay(4));
    display_at_middle(160,204,String(weatherforecast.getTempMax(0)));
    display_at_middle(213,204,String(weatherforecast.getTempMax(1)));
    display_at_middle(266,204,String(weatherforecast.getTempMax(2)));
    display_at_middle(320,204,String(weatherforecast.getTempMax(3)));
    display_at_middle(373,204,String(weatherforecast.getTempMax(4)));
    display_at_middle(160,278,String(weatherforecast.getTempMin(0)));
    display_at_middle(213,278,String(weatherforecast.getTempMin(1)));
    display_at_middle(266,278,String(weatherforecast.getTempMin(2)));
    display_at_middle(320,278,String(weatherforecast.getTempMin(3)));
    display_at_middle(373,278,String(weatherforecast.getTempMin(4)));
    int TempMax_max = get_max(weatherforecast.getTempMax(0),weatherforecast.getTempMax(1),weatherforecast.getTempMax(2),weatherforecast.getTempMax(3),weatherforecast.getTempMax(4));
    int TempMin_min = get_min(weatherforecast.getTempMin(0),weatherforecast.getTempMin(1),weatherforecast.getTempMin(2),weatherforecast.getTempMin(3),weatherforecast.getTempMin(4));
    //Max Line
    display.drawLine(160,map(weatherforecast.getTempMax(0),TempMin_min,TempMax_max,255,210),213,map(weatherforecast.getTempMax(1),TempMin_min,TempMax_max,255,210),GxEPD_BLACK);
    display.drawLine(213,map(weatherforecast.getTempMax(1),TempMin_min,TempMax_max,255,210),266,map(weatherforecast.getTempMax(2),TempMin_min,TempMax_max,255,210),GxEPD_BLACK);
    display.drawLine(266,map(weatherforecast.getTempMax(2),TempMin_min,TempMax_max,255,210),320,map(weatherforecast.getTempMax(3),TempMin_min,TempMax_max,255,210),GxEPD_BLACK);
    display.drawLine(320,map(weatherforecast.getTempMax(3),TempMin_min,TempMax_max,255,210),373,map(weatherforecast.getTempMax(4),TempMin_min,TempMax_max,255,210),GxEPD_BLACK);
    //Min Line
    display.drawLine(160,map(weatherforecast.getTempMin(0),TempMin_min,TempMax_max,255,210),213,map(weatherforecast.getTempMin(1),TempMin_min,TempMax_max,255,210),GxEPD_BLACK);
    display.drawLine(213,map(weatherforecast.getTempMin(1),TempMin_min,TempMax_max,255,210),266,map(weatherforecast.getTempMin(2),TempMin_min,TempMax_max,255,210),GxEPD_BLACK);
    display.drawLine(266,map(weatherforecast.getTempMin(2),TempMin_min,TempMax_max,255,210),320,map(weatherforecast.getTempMin(3),TempMin_min,TempMax_max,255,210),GxEPD_BLACK);
    display.drawLine(320,map(weatherforecast.getTempMin(3),TempMin_min,TempMax_max,255,210),373,map(weatherforecast.getTempMin(4),TempMin_min,TempMax_max,255,210),GxEPD_BLACK);
  }while (display.nextPage()); // 显示结束
  display.powerOff();
}

int get_max(int num1,int num2,int num3,int num4,int num5){
  int max_num;
  max_num = max(num1,num2);
  max_num = max(max_num,num3);
  max_num = max(max_num,num4);
  max_num = max(max_num,num5);
  return(max_num);
}

int get_min(int num1,int num2,int num3,int num4,int num5){
  int min_num;
  min_num = min(num1,num2);
  min_num = min(min_num,num3);
  min_num = min(min_num,num4);
  min_num = min(min_num,num5);
  return(min_num);
}

String fxtimeforecast_handle(String fxtime){
  return(fxtime.substring(5,7)+"/"+fxtime.substring(8,10));
}

String fxtime24_handle(String fxtime){
  return(fxtime.substring(11,13));
}

void weather_update(){
  for(int i = 0;i <= 2;i++){
    if(weatherNow.get()){
      break;
    }
  }
  for(int i = 0;i <= 2;i++){
    if(weatherforecast.get()){
      break;
    }
  }
  for(int i = 0;i <= 2;i++){
    if(airquality.get()){
      break;
    }
  }
  for(int i = 0;i <= 2;i++){
    if(weather24h.get()){
      break;
    }
  }
}

int check_wokeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason){
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); return(0);break;//由外部信号使用RTC_IO引起的唤醒
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); return(1);break;//使用RTC_CNTL的外部信号引起的唤醒
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); return(2);break;//由计时器引起的唤醒
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); return(3);break;//由触摸板引起的唤醒
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); return(4);break;//由ULP协处理器引起的唤醒
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); return(5);break;//由触摸板引起的唤醒
  }
}

void gpio_wokeup(){
  esp_sleep_enable_ext0_wakeup(WOKE_GOIO,1);//vp
}

void time_wokeup(){
  getLocalTime(&timeinfo);
  Serial.print("现在时间:");
  Serial.println(&timeinfo);
  struct tm timenew;
  time_t test=mktime(&timeinfo)+3600;//加1小时，[目的]后面分秒归零
  localtime_r(&test,&timenew);//时间戳转tm结构体函数
  timenew.tm_min = 0;
  timenew.tm_sec = 0;
  Serial.print("唤醒时间:");
  Serial.println(&timenew);
  Serial.print("时间差:");
  Serial.print(difftime(mktime(&timenew),mktime(&timeinfo)));
  Serial.println("s");
  esp_sleep_enable_timer_wakeup(difftime(mktime(&timenew),mktime(&timeinfo))*1000000);
}

String plus_0(int num){//补零函数[2位]
  if(num < 10){
    return("0"+String(num));
  }else{
    return(String(num));
  }
}

void time_update(){
  timeClient.begin();
  ESP_LOGI("NTP","begin...done");
  timeClient.update();
  ESP_LOGI("NTP","update...");
  timeClient.end();
  ESP_LOGI("NTP","stop...");
  struct timeval tv;
  tv.tv_sec=timeClient.getEpochTime();
  tv.tv_usec = 0;
  ESP_LOGI("NTP","setting RTC...");
  settimeofday(&tv, NULL);
  ESP_LOGI("NTP","setting RTC...done");

  int i = 0;
  while(!getLocalTime(&timeinfo)){
    ESP_LOGW("NTP","Failed to obtain time");
    ESP_LOGW("NTP","Trying to update time again");
    timeClient.begin();
    ESP_LOGI("NTP","begin...done");
    timeClient.update();
    ESP_LOGI("NTP","update...");
    timeClient.end();
    ESP_LOGI("NTP","stop...");
    struct timeval tv;
    tv.tv_sec=timeClient.getEpochTime();
    tv.tv_usec = 0;
    ESP_LOGI("NTP","setting RTC...");
    settimeofday(&tv, NULL);
    ESP_LOGI("NTP","setting RTC...done");

    if(i==5){
      break;
    }
    i++;
    delay(500);
  }
}

void display_setup(){
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);   // 设置前景色
  u8g2Fonts.setBackgroundColor(GxEPD_WHITE);   // 设置背景色
  display.setFullWindow();
  display.firstPage();
  do{
    u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312);
    display.drawBitmap(90,80,logo_550W1,212,74,GxEPD_BLACK);
    display.drawBitmap(90,80,logo_550W2,212,74,GxEPD_RED);
    display_at_middle(display.width()/2,176,"550W启动中...");
    display_at_middle(display.width()/2,194,"尝试连接WiFi "+ nvs.getString("WiFi_SSID"));
    display_at_middle(display.width()/2,212,"尝试更新天气");
    display_at_middle(display.width()/2,230,"尝试更新RTC时钟");
    
  }while (display.nextPage()); // 显示结束
  display.powerOff();
}

void setting_mode(int why){
  ESP_LOGI("setting","Entering setting mode...");
  String why_text;
  if(why == 0){
    why_text = "因无法连接WiFi";
  }else if(why == 1){
    why_text = "";//配置按钮
  }else if(why == 2){
    why_text = "已连接WiFi，但无法连接Internet";
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IPAddress(192,168,0,1),IPAddress(192,168,0,1),IPAddress(255,255,255,0));
  WiFi.softAP(AP_NAME);
  ESP_LOGI("setting","AP was set up");

  server.on("/",webserver_handlemainpage);
  server.on("/configwifi",webserver_handleconfigwifipage);
  server.on("/configweather",webserver_handleconfigweatherpage);
  server.on("/configtext",webserver_handleconfigtextpage);
  server.on("/configwifipost",HTTP_POST,webserver_handleconfigwifipage_post);
  server.on("/configweatherpost",HTTP_POST,webserver_handleconfigweatherpage_post);
  server.on("/configtextpost",HTTP_POST,webserver_handleconfigtextpage_post);
  server.onNotFound([](){server.send(404, "text/plain", "404 not found");});
  server.begin();
  ESP_LOGI("setting","Server starting...");
  xTaskCreatePinnedToCore(/*任务函数*/thread_server,/*任务名称*/"serverhandle",/*任务堆栈大小*/10000,/*任务的参数*/NULL,/*任务优先级*/1,/*用于跟踪已创建任务的任务句柄*/NULL,/*将任务固定到核心N上*/1);
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);   // 设置前景色
  u8g2Fonts.setBackgroundColor(GxEPD_WHITE);   // 设置背景色
  display.init();
  display.setFullWindow();
  display.firstPage();
  do{
    display.drawBitmap(90,80,logo_550W1,212,74,GxEPD_BLACK);
    display.drawBitmap(90,80,logo_550W2,212,74,GxEPD_RED);
    u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312);
    display_at_middle(display.width()/2,176,"系统"+why_text+"而进入配网模式");
    display_at_middle(display.width()/2,194,"请连接该设备发出的热点 Epaper");
    display_at_middle(display.width()/2,212,"并访问 192.168.0.1 以配置信息");
  }while (display.nextPage()); // 显示结束
  display.powerOff();
  delay(10*60*1000);
  display.init();
  display.setFullWindow();
  display.firstPage();
  do{
    display.drawBitmap(90,80,logo_550W1,212,74,GxEPD_BLACK);
    display.drawBitmap(90,80,logo_550W2,212,74,GxEPD_RED);
    u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312);
    display_at_middle(display.width()/2,176,"系统已进入睡眠状态");
    display_at_middle(display.width()/2,194,"请重新按配置键或拨动开关以进行重启");
  }while (display.nextPage()); // 显示结束
  display.powerOff();
  gpio_wokeup();
  esp_deep_sleep_start();
}

void display_at_middle(int x,int y,String text){
  const char *character = text.c_str();
  int text_width = u8g2Fonts.getUTF8Width(character);
  x=x-(text_width/2);
  u8g2Fonts.setCursor(x,y);
  u8g2Fonts.print(text);
}

int get_display_long(String text){
  const char *character = text.c_str();
  return(u8g2Fonts.getUTF8Width(character));
}

void AXP202_init(){
  ESP_LOGI("IIC","init...");
  Wire.begin(PMU_SDA, PMU_SCL);//启动IIC通信，用于连接AXP202
  ESP_LOGI("IIC","init...done");

  ESP_LOGI("AXP202","AXP202 ADC Test");
  int ret = axp202.begin(Wire, slave_address);
  if (ret) {
    ESP_LOGE("AXP202","AXP202 power chip detected ... Check your wiring!");
    while(1);
  }
  axp.setPowerOutPut(AXP202_LDO2, AXP202_OFF);
  axp.setPowerOutPut(AXP202_LDO3, AXP202_OFF);
  axp.setPowerOutPut(AXP202_LDO4, AXP202_OFF);
  axp.setPowerOutPut(AXP202_DCDC2, AXP202_OFF);
  axp.setPowerOutPut(AXP202_EXTEN, AXP202_OFF);
  axp202.adc1Enable(AXP202_VBUS_VOL_ADC1|AXP202_VBUS_CUR_ADC1|AXP202_BATT_CUR_ADC1|AXP202_BATT_VOL_ADC1,true);
  ESP_LOGI("AXP202","BATTERY STATUS:");
  if (axp202.isBatteryConnect()) {
    ESP_LOGI("AXP202","batter connect");
  } else {
    ESP_LOGW("AXP202","BATER DISCONNECT");
  }
}

void check_bat(){
  if(axp202.getBattPercentage()<5){
    display.setFullWindow();
    display.firstPage();
    do{
      u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312);
      display.drawBitmap(90,80,logo_550W1,212,74,GxEPD_BLACK);
      display.drawBitmap(90,80,logo_550W2,212,74,GxEPD_RED);
      display_at_middle(display.width()/2,176,"电池电量耗尽");
      display_at_middle(display.width()/2,194,"已关机");
    }while (display.nextPage()); // 显示结束
    display.powerOff();
    esp_deep_sleep_start();
  }
}

void app_init(){
  Serial.begin(115200);
  ESP_LOGI("nvs","init...");
  nvs.begin("mynamespace");//初始化nvs储存
  ESP_LOGI("nvs","init...done");
  AXP202_init();
  ESP_LOGI("display","init...");
  display.init();  // 初始化屏幕 
  display.setRotation(0);  // 设置屏幕旋转方向，分别有0，1，2，3这四个方向
  u8g2Fonts.begin(display);
  u8g2Fonts.setFontDirection(0);    // 设置文字显示方向
  u8g2Fonts.setFontMode(1);                        // 使用u8g2透明模式（这是默认设置）
  ESP_LOGI("display","init...done");
  check_bat();
  if(is_System_has_on == 0){//检查系统是不是已经启动过了
    display_setup();
  }
  is_System_has_on = 1;
  if(check_wokeup_reason()==0){//检测是不是gpio唤醒
    setting_mode(1);
  }
  ESP_LOGI("wifi","begin...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(nvs.getString("WiFi_SSID"),nvs.getString("WiFi_PASS"));
  int times = 0;
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    if(times == 30){ //15s
      Serial.println("");
      ESP_LOGW("wifi","enter setting mode...");
      setting_mode(0);
    }
    delay(500);
    times += 1;
  }
  delay(5000);
  ESP_LOGI("weather","config...");
  weatherNow.config(nvs.getString("weather_key"), nvs.getString("weather_loc"), "m", "zh");
  weatherforecast.config(nvs.getString("weather_key"), nvs.getString("weather_loc"), "m", "zh");
  airquality.config(nvs.getString("weather_key"), nvs.getString("weather_loc"), "m", "zh");
  weather24h.config(nvs.getString("weather_key"), nvs.getString("weather_loc"), "m", "zh");
  ESP_LOGI("weather","config...done");
}
