#include <Arduino.h>

String mainpage = R"rawliteral(
<!DOCTYPE html>
<html>
    <head>
        <meta charset="utf-8">
        <title>墨水屏天气站配置页面</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
    </head>
    
    <style type="text/css">
        .input{display: block; margin-top: 10px;}
        .input input{height: 30px;width: 200px;}
        .btn{width: 120px; height: 35px; background-color: #50a854; border:0px; color:#ffffff; margin-top:15px; margin-left:100px;}
    </style>
    
    <body>
        <h1>墨水屏天气站配置页面</h1>
        <p>在这个页面，您可以进入配置WiFi与天气更新信息与文字的页面</p>
        <a href="./configwifi">配置WiFi</a>
        <br />
        <a href="./configweather">配置天气更新信息</a>
        <br />
        <a href="./configtext">配置文字</a>
        <br />
        <p>固件版本:3.0 开发者:终极非专业程序员</p>
    </body>
</html>
)rawliteral";

String configweatherpage = R"rawliteral(
<!DOCTYPE html>
<html>
    <head>
        <meta charset="utf-8">
        <title>天气信息配置页面</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
    </head>
    
    <style type="text/css">
        .input{display: block; margin-top: 10px;}
        .input input{height: 30px;width: 200px;}
        .btn{width: 120px; height: 35px; background-color: #50a854; border:0px; color:#ffffff; margin-top:15px; margin-left:100px;}
    </style>
    
    <body>
        <h1>墨水屏天气站天气信息配置页面</h1>
        <p>这是墨水屏天气站天气信息配置页面</p>
        <p>中国天气城市代码需要进行查询,例子:广州 101280101</p>
        <p>中国天气城市代码可以尝试在这里查询:</p>
        <a href="https://blog.csdn.net/u013634862/article/details/44886769">https://blog.csdn.net/u013634862/article/details/44886769</a>
        <form method="POST" action="configweatherpost">
            <label class="input">
                <span>和风天气Key</span>
                <input type="text" name="weather_key">
            </label>
            <label class="input">
                <span>[1]和风天气城市代码</span> 
                <input type="text"  name="weather_citycode">
            </label>
            <input class="btn" type="submit" name="submit" value="保存并重启">         
        </form>
        
    </body>
</html>
)rawliteral";

String configwifipage = R"rawliteral(
<!DOCTYPE html>
<html>
    <head>
        <meta charset="utf-8">
        <title>WIFI信息配置页面</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
    </head>
    
    <style type="text/css">
        .input{display: block; margin-top: 10px;}
        .input input{height: 30px;width: 200px;}
        .btn{width: 120px; height: 35px; background-color: #50a854; border:0px; color:#ffffff; margin-top:15px; margin-left:100px;}
    </style>
    
    <body>
        <h1>墨水屏天气站WiFi信息配置页面</h1>
        <p>这是墨水屏天气站WiFi信息配置页面,该设备仅支持2.4GWiFi</p>
        <form method="POST" action="configwifipost">
            <label class="input">
                <span>WiFi SSID</span>
                <input type="text" name="wifi_ssid">
            </label>
            <label class="input">
                <span>WiFi PASS</span> 
                <input type="password"  name="wifi_pass">
            </label>
            <input class="btn" type="submit" name="submit" value="保存并重启">         
        </form>
        
    </body>
</html>
)rawliteral";

String configtextpage = R"rawliteral(
<!DOCTYPE html>
<html>
    <head>
        <meta charset="utf-8">
        <title>文字配置页面</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
    </head>
    
    <style type="text/css">
        .input{display: block; margin-top: 10px;}
        .input input{height: 30px;width: 200px;}
        .btn{width: 120px; height: 35px; background-color: #50a854; border:0px; color:#ffffff; margin-top:15px; margin-left:100px;}
    </style>
    
    <body>
        <h1>墨水屏天气站文字配置页面</h1>
        <p>这是墨水屏天气站文字配置页面,文字不能多余24个字符,可能会超出显示范围</p>
        <form method="POST" action="configtextpost">
            <label class="input">
                <span>文字</span>
                <input type="text" name="text_input">
            <input class="btn" type="submit" name="submit" value="保存并重启">         
        </form>
        
    </body>
</html>
)rawliteral";

void webserver_handlemainpage(){
    server.send(200,"text/html",mainpage);
}

void webserver_handleconfigweatherpage(){
    server.send(200,"text/html",configweatherpage);
}

void webserver_handleconfigwifipage(){
    server.send(200,"text/html",configwifipage);
}

void webserver_handleconfigtextpage(){
    server.send(200,"text/html",configtextpage);
}

void webserver_handleconfigwifipage_post(){
    String wifi_ssid = "";
    String wifi_pass = "";
    if (server.hasArg("wifi_ssid"))          //判断是否有账号参数
    {
        Serial.print("got ssid:");
        wifi_ssid = server.arg("wifi_ssid");   //获取html表单输入框name名为"ssid"的内容
    
        Serial.println(wifi_ssid);
    } 
    else                                //没有参数
    { 
        Serial.println("error, not found ssid");
        server.send(200, "text/html", "<meta charset='UTF-8'>error, not found ssid"); //返回错误页面
        return;
    }

    if (server.hasArg("wifi_pass")) 
    {
        Serial.print("got password:");
        wifi_pass = server.arg("wifi_pass");  //获取html表单输入框name名为"pwd"的内容
        Serial.println(wifi_pass);
    } 
    else 
    {
        Serial.println("error, not found password");
        server.send(200, "text/html", "<meta charset='UTF-8'>error, not found password");
        return;
    }
    server.send(200, "text/html", "<meta charset='UTF-8'>WiFi SSID:" + wifi_ssid + "<br />WiFi password:" + wifi_pass + "<br />正在保存WiFi信息<br />稍后墨水屏天气站将重启...");
    nvs.putString("WiFi_SSID", wifi_ssid);
    nvs.putString("WiFi_PASS", wifi_pass);
    ESP.restart();
}

void webserver_handleconfigweatherpage_post(){
    String weather_key = "";
    String weather_location = "";
    if (server.hasArg("weather_key"))          //判断是否有账号参数
    {
        Serial.print("got weather key:");
        weather_key = server.arg("weather_key");   //获取html表单输入框name名为"ssid"的内容
    
        Serial.println(weather_key);
    } 
    else                                //没有参数
    { 
        Serial.println("error, not found weather key");
        server.send(200, "text/html", "<meta charset='UTF-8'>error, not found weather key"); //返回错误页面
        return;
    }

    if (server.hasArg("weather_citycode")) 
    {
        Serial.print("got weather citycode:");
        weather_location = server.arg("weather_citycode");  //获取html表单输入框name名为"pwd"的内容
        Serial.println(weather_location);
    } 
    else 
    {
        Serial.println("error, not found weather citycode");
        server.send(200, "text/html", "<meta charset='UTF-8'>error, not found weather citycode");
        return;
    }

    server.send(200, "text/html", "<meta charset='UTF-8'>和风天气Key:" + weather_key + "<br />和风天气城市:" + weather_location + "<br />正在保存天气更新信息<br />稍后墨水屏天气站将重启...");
    nvs.putString("weather_key", weather_key);
    nvs.putString("weather_loc", weather_location);
    ESP.restart();
}

void webserver_handleconfigtextpage_post(){
    String text = "";
    if (server.hasArg("text_input"))          //判断是否有账号参数
    {
        Serial.print("got text ");
        text = server.arg("text_input");   //获取html表单输入框name名为"ssid"的内容
    
        Serial.println(text);
    } 
    else                                //没有参数
    { 
        Serial.println("error, not found text");
        server.send(200, "text/html", "<meta charset='UTF-8'>error, not found text"); //返回错误页面
        return;
    }
    server.send(200, "text/html", "<meta charset='UTF-8'>文字:" + text + "<br />正在保存文字信息<br />稍后墨水屏天气站将重启...");
    nvs.putString("text", text);
    ESP.restart();
}

void thread_server(void *parameter){
  while(true){
    server.handleClient();
  }
}