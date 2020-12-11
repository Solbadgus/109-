#include <FS.h> 
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include<Timer.h>

const char* ssid = "ESP8266_AP";
const char* password = "@Aass_esp8266";
String usn ="admin";
String psd ="admin";

String set_stime_m;
String set_stime_s;
String set_ftime_m;
String set_ftime_s;
String set_ptime_m;
String set_ptime_s;
int set_stime;
int set_ftime;
int set_ptime;
int ttime = 0; //總時間
int stime = 0; //剩餘時間
int nstate = 3;//倒數狀態 STOP/START/SETTING/UP
int ftime = 5; //閃爍時間
int ptime = 30;//提示鈴時間
const char RE_LOG[] =
"<!DOCTYPE html>"
"<html lang=\"zh-TW\">"
"<head>"
    "<meta charset=\"UTF-8\">"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
    "<link rel=\"stylesheet\" href=\"css/bootstrap.min.css.gz\">"
    "<script src=\"js/jquery-3.5.1.min.js.gz\"></script>"
    "<script src=\"js/bootstrap.min.js.gz\"></script>"
    "<title>index</title>"
"</head>"
"<body>"
 "<div class=\"container-fluid\">"
  "<div class=\"row\">"
    
    "<div class=\"container p-4 my-4 bg-dark text-white\">"
      "<a href=\"LOG/LOG.html\" role=\"button\" class=\"btn btn-outline-danger active float-right\">進階設定</a>"
      "<h1 class=\"display-3\">專題計數器頁面</h1>"
    "</div>"
  "</div>"
  "<div class=\"row\">"
    "<div class=\"col\">"
      "<div class=\"container p-4 my-4 bg-info text-white\">"
         "<ul class=\"list-group\">"
           "<li class=\"list-group-item list-group-item-info\">"
            "<div class=\"row\">"
              "<div class=\"col-sm-3\"><span>登入失敗請重新登入</span></div>"
              "<div class=\"col-sm-9\">"
                              "<input type=\"button\" class=\"btn btn-outline-danger active float-right\" value=\"返回登入\" onclick=\'location.href=\"LOG.html\"\'>"
                          "</div>"
            "</div>"
          "</li>"
        "</ul>"
      "</div>"
    "</div>"
  "</div>"
  "<div class=\"col-sm-9\">"
    "</div>"
    "<div class=\"col-sm-3\">"
      "<h6 class=\"text-muted\">切勿抄襲 <small>by Mockingjay </small></h6>"
    "</div>"
  "</div>"
"</div>"

"</body>"
"</html>";

const char RE_SET[] =
"<!DOCTYPE html>"
"<html lang=\"zh-TW\">"
"<head>"
    "<meta charset=\"UTF-8\">"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
    "<link rel=\"stylesheet\" href=\"css/bootstrap.min.css.gz\">"
    "<script src=\"js/jquery-3.5.1.min.js.gz\"></script>"
    "<script src=\"js/bootstrap.min.js.gz\"></script>"
    "<title>index</title>"
"</head>"
"<body>"
"<script>document.location.href=\"set.html\";</script>"
"</body>"
"</html>";

const char SET_VAR[] =
"<!DOCTYPE html>"
"<html lang=\"zh-TW\">"
"<head>"
    "<meta charset=\"UTF-8\">"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
    "<link rel=\"stylesheet\" href=\"css/bootstrap.min.css.gz\">"
    "<script src=\"js/jquery-3.5.1.min.js.gz\"></script>"
    "<script src=\"js/bootstrap.min.js.gz\"></script>"
    "<title>index</title>"
"</head>"
"<body>"

"<script language=\"javascript\">"
  "alert(\'完成設定\');"
  "document.location.href=\"set.html\";"
"</script>"
"</body>"
"</html>";


ESP8266WebServer server(80);
Timer t1s;
Timer t5s;

void correction()
{
  Serial.print("Am");
  Serial.print(String(ttime/60)+",");
  Serial.print("As");
  Serial.print(String(ttime%60)+",");
  Serial.print("Bm");
  Serial.print(String(stime/60)+",");
  Serial.print("Bs");
  Serial.print(String(stime%60)+",");
  Serial.print("C");
  Serial.print(String(nstate)+",");
  Serial.print("D");
  Serial.print(String(ftime)+",");
  Serial.print("E");
  //Serial.print(String(ptime)+",");
  Serial.println(String(ptime)+",");
}
int flag_tx = 0;
void get_info_arm()
{
  if(Serial.available())
  {
    if(flag_tx == 0)
    {
      flag_tx = 1;
    }
    else 
    {
      flag_tx = 0;
    }
    digitalWrite(LED_BUILTIN,flag_tx);
    
    String str1 = Serial.readString();
    str1.trim();
    int value [4]; //[0]:stime [1]:state [2]:position of , [3]:position of .
    value[2] = str1.indexOf(",",0);
    value[3] = str1.indexOf(".",0);
    String a = "";
    String b = "";
    nstate = str1.substring(value[2]+1,value[3]).toInt();
    if(nstate == 3)
    {
      stime = str1.substring(0,value[2]).toInt();
    }
  }
}

void count()
{
  ttime +=1;
  if(stime < 0 && nstate == 2)
  {
    nstate = 4;
  }
  if(nstate == 2 || nstate ==  4)
  {
    stime -=1;
  }
}

void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password); 
   // Wait for connection
  WiFi.config(IPAddress(192,168,4,101),IPAddress(192,168,4,254),IPAddress(255,255,255,0));//固定IP
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  server.begin(); 
  server.on("/", fileindex);
  server.on("/index.html", fileindex);
  server.on("/LOG.html", LOG);
  server.on("/set.html", SET);
  server.on("/var/function.js",func);
  server.on("/css/bootstrap.min.css.gz", bootstrap);
  server.on("/js/bootstrap.min.js.gz", bootstrapmin);
  server.on("/js/jquery-3.5.1.min.js.gz", jquery);
  server.on("/check", []() {
    String username = server.arg("username");
    String passwd = server.arg("password");
    if((username == usn) &&(passwd == psd))
    {
      server.send(200, "text/html",RE_SET );
    }
    else
    {
      server.send(200, "text/html",RE_LOG );
    }
  });
  server.on("/set_state_on", []() { 
    server.send(200, "text/html",RE_SET );
    //Serial.print("nstate: ");
    nstate = 2;
    //Serial.println(nstate);
  });
  server.on("/set_state_off", []() {
    server.send(200, "text/html",RE_SET );
    //Serial.print("nstate: ");
    nstate = 1;
    //Serial.println(nstate);
  });

  server.on("/data.json", []() {
    server.send(200, "application/json", "[{\"ttime_m\":\""+ String(ttime/60) + "\",\"ttime_s\":\""+ String(ttime%60) + "\",\"stime_m\":\""+ String(stime/60) + "\",\"stime_s\":\"" + String(stime%60) + "\",\"nstate\":\"" + String(nstate) + "\", \"ftime\":\"" + String(ftime) + "\", \"ptime\":\"" + String(ptime) + "\"}]");
  });

  server.on("/set_var", []() {
    set_stime_m = server.arg("set_stime_m");
    set_stime_s = server.arg("set_stime_s");
    if((set_stime_m=="") && (set_stime_s==""))
    {
        //Serial.println("No Enter the stime Data.");
    }
    else
    {
      set_stime =  (set_stime_m.toInt()*60)+(set_stime_s.toInt());
      //Serial.print("stime: ");
      //Serial.println(set_stime);
      stime = set_stime;
      nstate = 3;
    }
    
    set_ftime_m = server.arg("set_ftime_m");
    set_ftime_s = server.arg("set_ftime_s");
    if((set_ftime_m=="") && (set_ftime_s==""))
    {
        //Serial.println("No Enter the ftime Data.");
    }
    else
    {
      set_ftime =  (set_ftime_m.toInt()*60)+(set_ftime_s.toInt());
      //Serial.print("ftime: ");
      //Serial.println(set_ftime);
      ftime = set_ftime;
      nstate = 3;
    }

    set_ptime_m = server.arg("set_ptime_m");
    set_ptime_s = server.arg("set_ptime_s");
    if((set_ptime_m=="") && (set_ptime_s==""))
    {
        //Serial.println("No Enter the ptime Data.");
    }
    else
    {
      set_ptime =  (set_ptime_m.toInt()*60)+(set_ptime_s.toInt());
      //Serial.print("ptime: ");
      //Serial.println(set_ptime);
      ptime = set_ptime;
      nstate = 3;
    }
    server.send(200, "text/html",SET_VAR );
    //Serial.println("已完成設定");
  });


//NEW
  SPIFFS.begin(); 
  t1s.every(1000,count);
  t5s.every(1000,correction);
}

void loop() 
{
  server.handleClient();
  t1s.update();
  t5s.update();
  get_info_arm();
}

void LOG()
{
  File file = SPIFFS.open("/LOG.html", "r"); 
  size_t sent = server.streamFile(file, "text/html");
}
void SET()
{
  File file = SPIFFS.open("/set.html", "r"); 
  size_t sent = server.streamFile(file, "text/html");
}

void func()
{
  File file = SPIFFS.open("/var/function.js", "r"); 
  size_t sent = server.streamFile(file, "application/javascript");
}

void fileindex()
{
  File file = SPIFFS.open("/index.html", "r"); 
  size_t sent = server.streamFile(file, "text/html");
}
void bootstrap()
{
  File file = SPIFFS.open("/css/bootstrap.min.css.gz", "r"); 
  size_t sent = server.streamFile(file, "text/css");

}
void bootstrapmin()
{
  File file = SPIFFS.open("/js/bootstrap.min.js.gz", "r"); 
  size_t sent = server.streamFile(file, "application/javascript");

}
void jquery()
{
  File file = SPIFFS.open("/js/jquery-3.5.1.min.js.gz", "r"); 
  size_t sent = server.streamFile(file, "application/javascript");
}
