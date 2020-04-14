#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <FastLED.h>
#include <Servo.h>
#include "Queue.h"

#define NUM_LEDS 16
#define DATA_PIN 13
#define QUEUE_SIZE_ITEMS 40

const int potPin = 36;
int potValue = 0;
static const int servosPins[4] = {19, 21, 22, 23};
char queue[20];
Servo servos[4];
DataQueue<char> firing(QUEUE_SIZE_ITEMS);

const char* ssid = "b5";
const char* password =  "commanderkeen";
StaticJsonDocument<2000> doc;
int yellow = 0;
int yellowcnt = 0;
int green = 0;
int greencnt = 0;
int blue = 0;
int bluecnt = 0;
int red = 0;
int redcnt = 0;
int checkwifi = 10000;
CRGB leds[NUM_LEDS];
int checkled = 1000;
boolean stopped = false;
int det = 0;
const char* root_ca= 
"MIIFnDCCBISgAwIBAgIRAPDL8o%2FfIlacCAAAAAAv7iIwDQYJKoZIhvcNAQELBQAwQjELMAkGA1UEBhMCVVMxHjAcBgNVBAoTFUdvb2dsZSBUcnVzdCBTZXJ2aWNlczETMBEGA1UEAxMKR1RTIENBIDFPMTAeFw0yMDAyMjAxOTM2MDZaFw0yMTAyMTgxOTM2MDZaMGgxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpDYWxpZm9ybmlhMRYwFAYDVQQHEw1Nb3VudGFpbiBWaWV3MRMwEQYDVQQKEwpHb29nbGUgTExDMRcwFQYDVQQDEw5maXJlYmFzZWlvLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAN9RhBIBdkeJFsy84i71J%2BTSRC0699jyn%2FJElfSpV%2FTYsmbrgzYdtZSZoCE%2BF85kWmHrfy%2FwXL1EWgO54XSnvSjwUZ5ZzphbZuMOASJQo6qeVzXQrBH2LVinE8RJ%2BjyJZWBu%2BegQlXE3SvoRD08b%2Bn2M2%2BwvStTuKoalLxFEheNrbpSpvlrG%2FTCC7rsrPGm9KzrngxXBPLarq2LcE%2FibUUHfIcZwuk298HdgFjRWD8%2B1zykkRg0hBuY3C02ujsiNLkN%2BgSBQLHD2VRIsewFiG7MvZsEdYYxWEwywkDoKAgBMIhszFDXf3UHDGmp96j7scz5P%2B1pGQJgQNU2Kc6s7Y%2BkCAwEAAaOCAmUwggJhMA4GA1UdDwEB%2FwQEAwIFoDATBgNVHSUEDDAKBggrBgEFBQcDATAMBgNVHRMBAf8EAjAAMB0GA1UdDgQWBBTaIo3cokKrewVKbvbTYD7SWCwTQjAfBgNVHSMEGDAWgBSY0fhuEOvPm%2BxgnxiQG6DrfQn9KzBkBggrBgEFBQcBAQRYMFYwJwYIKwYBBQUHMAGGG2h0dHA6Ly9vY3NwLnBraS5nb29nL2d0czFvMTArBggrBgEFBQcwAoYfaHR0cDovL3BraS5nb29nL2dzcjIvR1RTMU8xLmNydDArBgNVHREEJDAigg5maXJlYmFzZWlvLmNvbYIQKi5maXJlYmFzZWlvLmNvbTAhBgNVHSAEGjAYMAgGBmeBDAECAjAMBgorBgEEAdZ5AgUDMC8GA1UdHwQoMCYwJKAioCCGHmh0dHA6Ly9jcmwucGtpLmdvb2cvR1RTMU8xLmNybDCCAQMGCisGAQQB1nkCBAIEgfQEgfEA7wB1APZclC%2FRdzAiFFQYCDCUVo7jTRMZM7%2FfDC8gC8xO8WTjAAABcGRQkc4AAAQDAEYwRAIgAqAKqje0yTE%2F6UGDNWPv0%2B5F4tnO5A7opAGi%2FJyRUfECIGArAusuTUnG4NKHM7C67015o766qReSFmomT%2FwAUFxRAHYARJRlLrDuzq%2FEQAfYqP4owNrmgr7YyzG1P9MzlrW2gagAAAFwZFCSJwAABAMARzBFAiBNBYTgBcqmFFGdxj5dhNamhICADLcc3Ubic2FAweOQ0QIhAM1MtmXumV%2Bxvh3WRYMM9DOrXIR76qzQYcdI8s5EnNdkMA0GCSqGSIb3DQEBCwUAA4IBAQCTa6t2LdK7gRuRKEl4vY%2FrNhPmbPLkkiZoCeYFjsvXYmo%2FHpeMR1etRi4WpXgno0lG8a52YIPBu3VlGJvHterX%2BSRNkPy5V%2B3Bc2qI6kGr%2FAzTljmOVsqjo0Xp1nCtHOUVFQZ0HrnGDVEKrjFvCT9QaVkHQ%2F9ua%2B0Boxz9TiGdBeu4dI2nQ9uyxQJ56wqNGrkIOQ76bUsFkqYVk%2F6HgcmArFYBGtQHhfFw%2FHZXfe9%2BeD3F6f4P1S7TuCcGtIVYsvU%2FizHf9qpbC6sDnAqy%2B8KQr%2FsioXuPbGT4x7SFahcrkczLB3V9%2FvV2yNKejsE32edzyjx6BFnqWciTi8t6eFHA&cert=MIIESjCCAzKgAwIBAgINAeO0mqGNiqmBJWlQuDANBgkqhkiG9w0BAQsFADBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEGA1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjAeFw0xNzA2MTUwMDAwNDJaFw0yMTEyMTUwMDAwNDJaMEIxCzAJBgNVBAYTAlVTMR4wHAYDVQQKExVHb29nbGUgVHJ1c3QgU2VydmljZXMxEzARBgNVBAMTCkdUUyBDQSAxTzEwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDQGM9F1IvN05zkQO9%2BtN1pIRvJzzyOTHW5DzEZhD2ePCnvUA0Qk28FgICfKqC9EksC4T2fWBYk%2FjCfC3R3VZMdS%2FdN4ZKCEPZRrAzDsiKUDzRrmBBJ5wudgzndIMYcLe%2FRGGFl5yODIKgjEv%2FSJH%2FUL%2BdEaltN11BmsK%2BeQmMF%2B%2BAcxGNhr59qM%2F9il71I2dN8FGfcddwuaej4bXhp0LcQBbjxMcI7JP0aM3T4I%2BDsaxmKFsbjzaTNC9uzpFlgOIg7rR25xoynUxv8vNmkq7zdPGHXkxWY7oG9j%2BJkRyBABk7XrJfoucBZEqFJJSPk7XA0LKW0Y3z5oz2D0c1tJKwHAgMBAAGjggEzMIIBLzAOBgNVHQ8BAf8EBAMCAYYwHQYDVR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMBIGA1UdEwEB%2FwQIMAYBAf8CAQAwHQYDVR0OBBYEFJjR%2BG4Q68%2Bb7GCfGJAboOt9Cf0rMB8GA1UdIwQYMBaAFJviB1dnHB7AagbeWbSaLd%2FcGYYuMDUGCCsGAQUFBwEBBCkwJzAlBggrBgEFBQcwAYYZaHR0cDovL29jc3AucGtpLmdvb2cvZ3NyMjAyBgNVHR8EKzApMCegJaAjhiFodHRwOi8vY3JsLnBraS5nb29nL2dzcjIvZ3NyMi5jcmwwPwYDVR0gBDgwNjA0BgZngQwBAgIwKjAoBggrBgEFBQcCARYcaHR0cHM6Ly9wa2kuZ29vZy9yZXBvc2l0b3J5LzANBgkqhkiG9w0BAQsFAAOCAQEAGoA%2BNnn78y6pRjd9XlQWNa7HTgiZ%2Fr3RNGkmUmYHPQq6Scti9PEajvwRT2iWTHQr02fesqOqBY2ETUwgZQ%2BlltoNFvhsO9tvBCOIazpswWC9aJ9xju4tWDQH8NVU6YZZ%2FXteDSGU9YzJqPjY8q3MDxrzmqepBCf5o8mw%2FwJ4a2G6xzUr6Fb6T8McDO22PLRL6u3M4Tzs3A2M1j6bykJYi8wWIRdAvKLWZu%2FaxBVbzYmqmwkm5zLSDW5nIAJbELCQCZwMH56t2Dvqofxs6BBcCFIZUSpxu6x6td0V7SvJCCosirSmIatj%2F9dSSVDQibet8q%2F7UK4v4ZUN80atnZz1yg%3D%3D&cert=MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4GA1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjIxEzARBgNVBAoTCkdsb2JhbFNpZ24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDYxMjE1MDgwMDAwWhcNMjExMjE1MDgwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEGA1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKbPJA6%2BLm8omUVCxKs%2BIVSbC9N%2FhHD6ErPLv4dfxn%2BG07IwXNb9rfF73OX4YJYJkhD10FPe%2B3t%2Bc4isUoh7SqbKSaZeqKeMWhG8eoLrvozps6yWJQeXSpkqBy%2B0Hne%2Fig%2B1AnwblrjFuTosvNYSuetZfeLQBoZfXklqtTleiDTsvHgMCJiEbKjNS7SgfQx5TfC4LcshytVsW33hoCmEofnTlEnLJGKRILzdC9XZzPnqJworc5HGnRusyMvo4KD0L5CLTfuwNhv2GXqF4G3yYROIXJ%2FgkwpRl4pazq%2Br1feqCapgvdzZX99yqWATXgAByUr6P6TqBwMhAo6CygPCm48CAwEAAaOBnDCBmTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH%2FBAUwAwEB%2FzAdBgNVHQ4EFgQUm%2BIHV2ccHsBqBt5ZtJot39wZhi4wNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2NybC5nbG9iYWxzaWduLm5ldC9yb290LXIyLmNybDAfBgNVHSMEGDAWgBSb4gdXZxwewGoG3lm0mi3f3BmGLjANBgkqhkiG9w0BAQUFAAOCAQEAmYFThxxol4aR7OBKuEQLq4GsJ0%2FWwbgcQ3izDJr86iw8bmEbTUsp9Z8FHSbBuOmDAGJFtqkIk7mpM0sYmsL4h4hO291xNBrBVNpGP%2BDTKqttVCL1OmLNIG%2B6KYnX3ZHu01yiPqFbQfXf5WRDLenVOavSot%2B3i9DAgBkcRcAtjOj4LaR0VknFBbVPFd5uRHg5h6h%2Bu%2FN5GJG79G%2BdwfCMNYxdAfvDbbnvRG15RjF%2BCv6pgsH%2F76tuIMRQyV%2BdTZsXjAzlAcmgQWpzU%2FqlULRuJQ%2F7TBj0%2FVLZjmmx6BEP3ojY%2Bx1J96relc8geMJgEtslQIxq%2FH5COEBkEveegeGTLg%3D%3D";
int servopos = 180;
int servocount = 0;
char active = 'n';
boolean dir = true;
int timer = 0;

void ledswitch(char color, int count){
  int shift = 5;
  if(count > shift*3){
    count = shift*3; 
  }
  switch(color){

    case 'r':
    {
      int base = 2;
      for(int i = 0; i<3; i++){
        int val = count;
        if(val > shift){
          val = shift;
        }
        if(val< 0){
          val = 0;
        }
        int index = i+base;
        leds[index].setRGB(val, val*10, val);
        count =  count - shift;
      }
      break;
    }
     case 'g':{
      int base = 5;
      for(int i = 0; i<3; i++){
        int val = count;
        if(val > shift){
          val = shift;
        }
        if(val
        < 0){
          val = 0;
        }
        int index = i+base;
        leds[index].setRGB(val*10, val*3, val);
        count =  count - shift;
      }
      break;
     }
     case 'b':
     {
      int base = 8;
      for(int i = 2; i>=0; i--){
        int val = count;
        if(val > shift){
          val = shift;
        }
        if(val< 0){
          val = 0;
        }
        int index = i+base;
        leds[index].setRGB(val*3, val, val*10);
        count =  count - shift;
      }
      break;
     }
     case 'y':{
      int base = 11;
      for(int i = 2; i>=0; i--){
        int val = count;
        if(val > shift){
          val = shift;
        }
        if(val< 0){
          val = 0;
        }
        int index = i+base;
        leds[index].setRGB(val*10, val*10, val*2);
        count =  count - shift;
      }
      break;
     }
  }
}
 
void setup() {
 
  Serial.begin(115200);
  delay(1000);
 
  WiFi.begin(ssid, password); 

 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
  FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
  for(int x = 0; x<16; x++){
    leds[x].setRGB(0, 0, 0);
  }
  FastLED.show();

  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
      Serial.println(checkwifi);
      HTTPClient http;
   
      http.begin("https://discord-cloak.firebaseio.com/messages.json", root_ca); //Specify the URL and certificate
      int httpCode = http.sendRequest("DELETE");                                                  //Make the request
  }
 for(int i = 0; i < 4; ++i) {
      if(!servos[i].attach(servosPins[i])) {
          Serial.print("Servo ");
          Serial.print(i);
          Serial.println("attach error");
      }
  }
  for(int i = 0; i < 4; ++i) {
        servos[i].write(180);
  }
  
}
 

void loop() {
  potValue = analogRead(potPin)/1000;
  if(potValue == 0){
    stopped = true;
    for(int x = 2; x<14; x++){
      leds[x].setRGB(25, 25, 25);

    }
    leds[0].setRGB(0, 0, 0);
    leds[1].setRGB(0, 0, 0);
    leds[14].setRGB(0, 0, 0);
    leds[15].setRGB(0, 0, 0);
    FastLED.show();
  }
  else{
    stopped = false;
  }
  switch(potValue){
    case 1:
      leds[0].setRGB(10, 10, 10);
      leds[1].setRGB(0, 0, 0);
      leds[14].setRGB(0, 0, 0);
      leds[15].setRGB(0, 0, 0);
      break;
    case 2:
       leds[0].setRGB(0, 0, 0);
      leds[1].setRGB(10, 10, 10);
      leds[14].setRGB(0, 0, 0);
      leds[15].setRGB(0, 0, 0);
      break;
    case 3:
      leds[0].setRGB(0, 0, 0);
      leds[1].setRGB(0, 0, 0);
      leds[14].setRGB(10, 10, 10);
      leds[15].setRGB(0, 0, 0);
      break;
    case 4:
      leds[0].setRGB(0, 0, 0);
      leds[1].setRGB(0, 0, 0);
      leds[14].setRGB(0, 0, 0);
      leds[15].setRGB(10, 10, 10);
      break;
  }
  FastLED.show();
  if(!stopped and potValue > 0){
    checkwifi = checkwifi + 1;
    checkled = checkled + 1;
    servocount = servocount + 1;
    if ((WiFi.status() == WL_CONNECTED) and checkwifi > 2500/(potValue)) { //Check the current connection status
      Serial.println(checkwifi);
      HTTPClient http;
   
      http.begin("https://discord-cloak.firebaseio.com/messages.json", root_ca); //Specify the URL and certificate
      int httpCode = http.sendRequest("GET");                                                  //Make the request
   
      if (httpCode > 0) { //Check for the returning code
   
          String payload = http.getString();
          Serial.println(httpCode);
          Serial.println(payload);
          DeserializationError error = deserializeJson(doc, payload);
          if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
            return;
          }
          int count = doc.size();
          if(count > 0){
            JsonObject root = doc.as<JsonObject>();
            for(JsonPair kv: root){
              JsonObject el = kv.value();
              String user = el["user"];
              int num = el ["count"];
              if (!firing.isFull()) {
                firing.enqueue(user.charAt(0));
              }
              switch(user.charAt(0)){
                case 'y':
                  yellow = yellow + num;
                  break;
                case 'b':
                  blue = blue + num;
                  break;
                case 'r':
                  red = red + num;
                  break;
                case 'g':
                  green = green + num;
                  break;
              }
  
              
            }
            Serial.print("Green ");
            Serial.println(green);
            Serial.print("Blue ");
            Serial.println(blue);
            Serial.print("Red ");
            Serial.println(red);
            Serial.print("Yellow ");
            Serial.println(yellow);
          }
        }
   
      else {
        Serial.println("Error on HTTP request");
      }
      http.sendRequest("DELETE");
      http.end(); //Free the resources
      checkwifi = 0;
    }
    if(checkled > 100){
      ledswitch('r', red);
      ledswitch('y', yellow);
      ledswitch('b', blue);
      ledswitch('g', green);
      if(det > 5){
        if(red - potValue > 0){
          red = red - potValue;
        }
        else{
          red = 0;
        }
        if(yellow - potValue > 0){
          yellow = yellow - potValue;
        }
        else{
          yellow = 0;
        }
  
        if(blue - potValue > 0){
          blue = blue - potValue;
        }
        else{
          blue = 0;
        }
  
        if(green - potValue > 0){
          green = green - potValue;
        }
        else{
          green = 0;
        }
        det = 0;
      }
      FastLED.show();
      det  = det +1;
      checkled = 0;
    }
    if(servocount > 8){
      Serial.print(active);
      Serial.print(" ");
      Serial.println(servopos);
       if (!firing.isEmpty() and active == 'n' and timer == 0) {
          active = firing.dequeue();
       }
      if(active != 'n'){
        if(dir){
          servopos = servopos - 5;
        }
        else{
          servopos = servopos + 5;
        }
        switch(active){
          case 'y':
            servos[2].write(servopos);
            break;
          case 'b':
            servos[0].write(servopos);
            break;
          case 'r':
            servos[3].write(servopos);
            break;
          case 'g':
            servos[1].write(servopos);
            break;
        }
        if(servopos < 100){
          dir = false;
        }
        if(servopos > 180){
          dir = true;
          active = 'n';
          timer = 100;
        }
      }
      servocount = 0;
      Serial.print(active);
      Serial.print(" ");
      Serial.println(servopos);
    }
    if(timer > 0){
      timer = timer -1;
    }
  }
  
  delay(1);
}
