//imports
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <FastLED.h>
#include <Servo.h>
#include "Queue.h"

//pin definitions
#define NUM_LEDS 16
#define DATA_PIN 13
const int potPin = 36;
int potValue = 0;
static const int servosPins[4] = {19, 21, 22, 23};
char queue[20];
Servo servos[4];

//Define the queue for firing the servos (so only once fires at a time)
#define QUEUE_SIZE_ITEMS 40
DataQueue<char> firing(QUEUE_SIZE_ITEMS);

//define wifi settings
const char* ssid = "wifi_name";
const char* password =  "password";

//the https certificate for our firebase
const char* root_ca=
"MIIFnDCCBISgAwIBAgIRAPDL8o%2FfIlacCAAAAAAv7iIwDQYJKoZIhvcNAQELBQAwQjELMAkGA1UEBhMCVVMxHjAcBgNVBAoTFUdvb2dsZSBUcnVzdCBTZXJ2aWNlczETMBEGA1UEAxMKR1RTIENBIDFPMTAeFw0yMDAyMjAxOTM2MDZaFw0yMTAyMTgxOTM2MDZaMGgxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpDYWxpZm9ybmlhMRYwFAYDVQQHEw1Nb3VudGFpbiBWaWV3MRMwEQYDVQQKEwpHb29nbGUgTExDMRcwFQYDVQQDEw5maXJlYmFzZWlvLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAN9RhBIBdkeJFsy84i71J%2BTSRC0699jyn%2FJElfSpV%2FTYsmbrgzYdtZSZoCE%2BF85kWmHrfy%2FwXL1EWgO54XSnvSjwUZ5ZzphbZuMOASJQo6qeVzXQrBH2LVinE8RJ%2BjyJZWBu%2BegQlXE3SvoRD08b%2Bn2M2%2BwvStTuKoalLxFEheNrbpSpvlrG%2FTCC7rsrPGm9KzrngxXBPLarq2LcE%2FibUUHfIcZwuk298HdgFjRWD8%2B1zykkRg0hBuY3C02ujsiNLkN%2BgSBQLHD2VRIsewFiG7MvZsEdYYxWEwywkDoKAgBMIhszFDXf3UHDGmp96j7scz5P%2B1pGQJgQNU2Kc6s7Y%2BkCAwEAAaOCAmUwggJhMA4GA1UdDwEB%2FwQEAwIFoDATBgNVHSUEDDAKBggrBgEFBQcDATAMBgNVHRMBAf8EAjAAMB0GA1UdDgQWBBTaIo3cokKrewVKbvbTYD7SWCwTQjAfBgNVHSMEGDAWgBSY0fhuEOvPm%2BxgnxiQG6DrfQn9KzBkBggrBgEFBQcBAQRYMFYwJwYIKwYBBQUHMAGGG2h0dHA6Ly9vY3NwLnBraS5nb29nL2d0czFvMTArBggrBgEFBQcwAoYfaHR0cDovL3BraS5nb29nL2dzcjIvR1RTMU8xLmNydDArBgNVHREEJDAigg5maXJlYmFzZWlvLmNvbYIQKi5maXJlYmFzZWlvLmNvbTAhBgNVHSAEGjAYMAgGBmeBDAECAjAMBgorBgEEAdZ5AgUDMC8GA1UdHwQoMCYwJKAioCCGHmh0dHA6Ly9jcmwucGtpLmdvb2cvR1RTMU8xLmNybDCCAQMGCisGAQQB1nkCBAIEgfQEgfEA7wB1APZclC%2FRdzAiFFQYCDCUVo7jTRMZM7%2FfDC8gC8xO8WTjAAABcGRQkc4AAAQDAEYwRAIgAqAKqje0yTE%2F6UGDNWPv0%2B5F4tnO5A7opAGi%2FJyRUfECIGArAusuTUnG4NKHM7C67015o766qReSFmomT%2FwAUFxRAHYARJRlLrDuzq%2FEQAfYqP4owNrmgr7YyzG1P9MzlrW2gagAAAFwZFCSJwAABAMARzBFAiBNBYTgBcqmFFGdxj5dhNamhICADLcc3Ubic2FAweOQ0QIhAM1MtmXumV%2Bxvh3WRYMM9DOrXIR76qzQYcdI8s5EnNdkMA0GCSqGSIb3DQEBCwUAA4IBAQCTa6t2LdK7gRuRKEl4vY%2FrNhPmbPLkkiZoCeYFjsvXYmo%2FHpeMR1etRi4WpXgno0lG8a52YIPBu3VlGJvHterX%2BSRNkPy5V%2B3Bc2qI6kGr%2FAzTljmOVsqjo0Xp1nCtHOUVFQZ0HrnGDVEKrjFvCT9QaVkHQ%2F9ua%2B0Boxz9TiGdBeu4dI2nQ9uyxQJ56wqNGrkIOQ76bUsFkqYVk%2F6HgcmArFYBGtQHhfFw%2FHZXfe9%2BeD3F6f4P1S7TuCcGtIVYsvU%2FizHf9qpbC6sDnAqy%2B8KQr%2FsioXuPbGT4x7SFahcrkczLB3V9%2FvV2yNKejsE32edzyjx6BFnqWciTi8t6eFHA&cert=MIIESjCCAzKgAwIBAgINAeO0mqGNiqmBJWlQuDANBgkqhkiG9w0BAQsFADBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEGA1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjAeFw0xNzA2MTUwMDAwNDJaFw0yMTEyMTUwMDAwNDJaMEIxCzAJBgNVBAYTAlVTMR4wHAYDVQQKExVHb29nbGUgVHJ1c3QgU2VydmljZXMxEzARBgNVBAMTCkdUUyBDQSAxTzEwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDQGM9F1IvN05zkQO9%2BtN1pIRvJzzyOTHW5DzEZhD2ePCnvUA0Qk28FgICfKqC9EksC4T2fWBYk%2FjCfC3R3VZMdS%2FdN4ZKCEPZRrAzDsiKUDzRrmBBJ5wudgzndIMYcLe%2FRGGFl5yODIKgjEv%2FSJH%2FUL%2BdEaltN11BmsK%2BeQmMF%2B%2BAcxGNhr59qM%2F9il71I2dN8FGfcddwuaej4bXhp0LcQBbjxMcI7JP0aM3T4I%2BDsaxmKFsbjzaTNC9uzpFlgOIg7rR25xoynUxv8vNmkq7zdPGHXkxWY7oG9j%2BJkRyBABk7XrJfoucBZEqFJJSPk7XA0LKW0Y3z5oz2D0c1tJKwHAgMBAAGjggEzMIIBLzAOBgNVHQ8BAf8EBAMCAYYwHQYDVR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMBIGA1UdEwEB%2FwQIMAYBAf8CAQAwHQYDVR0OBBYEFJjR%2BG4Q68%2Bb7GCfGJAboOt9Cf0rMB8GA1UdIwQYMBaAFJviB1dnHB7AagbeWbSaLd%2FcGYYuMDUGCCsGAQUFBwEBBCkwJzAlBggrBgEFBQcwAYYZaHR0cDovL29jc3AucGtpLmdvb2cvZ3NyMjAyBgNVHR8EKzApMCegJaAjhiFodHRwOi8vY3JsLnBraS5nb29nL2dzcjIvZ3NyMi5jcmwwPwYDVR0gBDgwNjA0BgZngQwBAgIwKjAoBggrBgEFBQcCARYcaHR0cHM6Ly9wa2kuZ29vZy9yZXBvc2l0b3J5LzANBgkqhkiG9w0BAQsFAAOCAQEAGoA%2BNnn78y6pRjd9XlQWNa7HTgiZ%2Fr3RNGkmUmYHPQq6Scti9PEajvwRT2iWTHQr02fesqOqBY2ETUwgZQ%2BlltoNFvhsO9tvBCOIazpswWC9aJ9xju4tWDQH8NVU6YZZ%2FXteDSGU9YzJqPjY8q3MDxrzmqepBCf5o8mw%2FwJ4a2G6xzUr6Fb6T8McDO22PLRL6u3M4Tzs3A2M1j6bykJYi8wWIRdAvKLWZu%2FaxBVbzYmqmwkm5zLSDW5nIAJbELCQCZwMH56t2Dvqofxs6BBcCFIZUSpxu6x6td0V7SvJCCosirSmIatj%2F9dSSVDQibet8q%2F7UK4v4ZUN80atnZz1yg%3D%3D&cert=MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4GA1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjIxEzARBgNVBAoTCkdsb2JhbFNpZ24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDYxMjE1MDgwMDAwWhcNMjExMjE1MDgwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEGA1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKbPJA6%2BLm8omUVCxKs%2BIVSbC9N%2FhHD6ErPLv4dfxn%2BG07IwXNb9rfF73OX4YJYJkhD10FPe%2B3t%2Bc4isUoh7SqbKSaZeqKeMWhG8eoLrvozps6yWJQeXSpkqBy%2B0Hne%2Fig%2B1AnwblrjFuTosvNYSuetZfeLQBoZfXklqtTleiDTsvHgMCJiEbKjNS7SgfQx5TfC4LcshytVsW33hoCmEofnTlEnLJGKRILzdC9XZzPnqJworc5HGnRusyMvo4KD0L5CLTfuwNhv2GXqF4G3yYROIXJ%2FgkwpRl4pazq%2Br1feqCapgvdzZX99yqWATXgAByUr6P6TqBwMhAo6CygPCm48CAwEAAaOBnDCBmTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH%2FBAUwAwEB%2FzAdBgNVHQ4EFgQUm%2BIHV2ccHsBqBt5ZtJot39wZhi4wNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2NybC5nbG9iYWxzaWduLm5ldC9yb290LXIyLmNybDAfBgNVHSMEGDAWgBSb4gdXZxwewGoG3lm0mi3f3BmGLjANBgkqhkiG9w0BAQUFAAOCAQEAmYFThxxol4aR7OBKuEQLq4GsJ0%2FWwbgcQ3izDJr86iw8bmEbTUsp9Z8FHSbBuOmDAGJFtqkIk7mpM0sYmsL4h4hO291xNBrBVNpGP%2BDTKqttVCL1OmLNIG%2B6KYnX3ZHu01yiPqFbQfXf5WRDLenVOavSot%2B3i9DAgBkcRcAtjOj4LaR0VknFBbVPFd5uRHg5h6h%2Bu%2FN5GJG79G%2BdwfCMNYxdAfvDbbnvRG15RjF%2BCv6pgsH%2F76tuIMRQyV%2BdTZsXjAzlAcmgQWpzU%2FqlULRuJQ%2F7TBj0%2FVLZjmmx6BEP3ojY%2Bx1J96relc8geMJgEtslQIxq%2FH5COEBkEveegeGTLg%3D%3D";




//definte the json that we'll dump webdata to
StaticJsonDocument<2000> doc;

//the number of words that each person has said so far
int yellow = 0;
int green = 0;
int blue = 0;
int red = 0;

//Our timer variable
int checkwifi = 10000;
int checkled = 1000;
int servocount = 0;
int timer = 0;
int det = 0; //timer variable for word deterioration

//whether the system should be running or not
boolean stopped = false;

//what position our servo should be in
int servopos = 180;

//whether there is an active servo or not
char active = 'n';

//which direction our servo should be in
boolean dir = true;

//set up the led array
CRGB leds[NUM_LEDS];

//function for controlling the leds
//color: the first character of the color we're controlling
//count: the number of words of that color that are undecayed
void ledswitch(char color, int count){

  //shift is the number of words per led needed to light up it up
  //at 5, it takes 5 to light the first, 10 the second and 15 the last
  int shift = 5;

  //if count is greater than the max of the three leds, change it to the max
  if(count > shift*3){
    count = shift*3;
  }
  switch(color){

	//if red
    case 'r':
    {
	  //the first red led is leds[2]
      int base = 2;

	  //iterate over the 3 leds
      for(int i = 0; i<3; i++){

		//take a sample of count
        int val = count;

		//if it's greater than shift, change it to shift
        if(val > shift){
          val = shift;
        }

		//if it's less than zero, change it to zero
        if(val< 0){
          val = 0;
        }

		//find the correct led
        int index = i+base;

		//set that led to the correct color
        leds[index].setRGB(val, val*10, val);

		//reduce count by shift for next led
        count =  count - shift;
      }
      break;
    }
	 //if green
     case 'g':{

	  //the first green led is leds[5]
      int base = 5;

	  //iterate over the 3 leds
      for(int i = 0; i<3; i++){
		//take a sample of count
        int val = count;

		//if it's greater than shift, change it to shift
        if(val > shift){
          val = shift;
        }
		//if it's less than zero, change it to zero
        if(val< 0){
          val = 0;
        }
		//find the correct led
        int index = i+base;
		//set that led to the correct color
        leds[index].setRGB(val*10, val*3, val);
		//reduce count by shift for next led
        count =  count - shift;
      }
      break;
     }
	 //if blue
     case 'b':
     {
	  //the first blue led is leds[8]
      int base = 8;
	  //iterate over the 3 leds
      for(int i = 2; i>=0; i--){
		//take a sample of count
        int val = count;
		//if it's greater than shift, change it to shift
        if(val > shift){
          val = shift;
        }
		//if it's less than zero, change it to zero
        if(val< 0){
          val = 0;
        }
		//find the correct led
        int index = i+base;
		//set that led to the correct color
        leds[index].setRGB(val*3, val, val*10);
		//reduce count by shift for next led
        count =  count - shift;
      }
      break;
     }
	 //if yellow
     case 'y':{
	  //the first yellow led is leds[11]
      int base = 11;
	  //iterate over the 3 leds
      for(int i = 2; i>=0; i--){
		//take a sample of count
        int val = count;
		//if it's greater than shift, change it to shift
        if(val > shift){
          val = shift;
        }
		//if it's less than zero, change it to zero
        if(val< 0){
          val = 0;
        }
		//find the correct led
        int index = i+base;
		//set that led to the correct color
        leds[index].setRGB(val*10, val*10, val*2);
		//reduce count by shift for next led
        count =  count - shift;
      }
      break;
     }
  }
}

//set up code
void setup() {
  //begin the serial connection
  Serial.begin(115200);
  //wait for the serial to be set up
  delay(1000);

  //wifi based on: https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFiClientSecure/examples/WiFiClientSecure/WiFiClientSecure.ino

  //start the wifi
  WiFi.begin(ssid, password);

  //connect to wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");


  //check if the wifi is connected
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
      Serial.println(checkwifi);
      HTTPClient http;
   	  //connect to the api end point
      http.begin("https://discord-cloak.firebaseio.com/messages.json", root_ca); //Specify the URL and certificate
	  //clear the database
      int httpCode = http.sendRequest("DELETE");
  }

  //add the leds
  FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
  //set all to black
  for(int x = 0; x<16; x++){
	leds[x].setRGB(0, 0, 0);
  }
  FastLED.show();
 //connect the servos
 for(int i = 0; i < 4; ++i) {
      if(!servos[i].attach(servosPins[i])) {
          Serial.print("Servo ");
          Serial.print(i);
          Serial.println("attach error");
      }
  }
  //set all servos to base position
  for(int i = 0; i < 4; ++i) {
        servos[i].write(180);
  }

}


void loop() {

  //poll the potentiometer and divide by 1000 to give a val between 0-4
  potValue = analogRead(potPin)/1000;
  //if 0
  if(potValue == 0){

	//stop the program
    stopped = true;

	//write white to all leds to show that it's stopped
    for(int x = 2; x<14; x++){
      leds[x].setRGB(25, 25, 25);

    }

	//set the control leds to white
    leds[0].setRGB(0, 0, 0);
    leds[1].setRGB(0, 0, 0);
    leds[14].setRGB(0, 0, 0);
    leds[15].setRGB(0, 0, 0);
    FastLED.show();
  }

  //otherwise start the program again
  else{
    stopped = false;
  }
  //set the control leds to according to the potentiometer value
  //1: led[0]
  //2: led[1]
  //3: led[14]
  //4: led[15]
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

  //If the program is running
  if(!stopped and potValue > 0){
	//iterate the timer variables
    checkwifi = checkwifi + 1;
    checkled = checkled + 1;
    servocount = servocount + 1;

	//again from: https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFiClientSecure/examples/WiFiClientSecure/WiFiClientSecure.ino
	//if wifi is connected and the timer has past
    if ((WiFi.status() == WL_CONNECTED) and checkwifi > 2500/(potValue)) {
	  //Check the current connection status
      Serial.println(checkwifi);
      HTTPClient http;

	  //connect to end point
      http.begin("https://discord-cloak.firebaseio.com/messages.json", root_ca);
	  //send get
      int httpCode = http.sendRequest("GET");
   	  //make sure the request went through
      if (httpCode > 0) {

		  //store the payload
          String payload = http.getString();
          Serial.println(httpCode);
          Serial.println(payload);

		  //convert to json
          DeserializationError error = deserializeJson(doc, payload);
          if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
            return;
          }

		  //if the json is non-null
          int count = doc.size();
          if(count > 0){

			//convert json base to a usable form
            JsonObject root = doc.as<JsonObject>();

			//for each element in the json array
            for(JsonPair kv: root){
              JsonObject el = kv.value();

			  //inside should be a user and a count
              String user = el["user"];
              int num = el ["count"];

			  //if the firing queue is not full, queue the user and count
              if (!firing.isFull()) {
                firing.enqueue(user.charAt(0));
              }

			  //add the count to the right user
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
          }
        }

      else {
        Serial.println("Error on HTTP request");
      }

	  //clear the database
      http.sendRequest("DELETE");
      http.end();

	  //reset timer
      checkwifi = 0;
    }

	//when the led timer has fire
    if(checkled > 100){

	  //draw each of the leds
      ledswitch('r', red);
      ledswitch('y', yellow);
      ledswitch('b', blue);
      ledswitch('g', green);

	  //if the decay timer has tripped
      if(det > 5){

		//reduce the count based on the potentiometer reading
		//of the correct user
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

		//reset decay timer
        det = 0;
      }
      FastLED.show();

	  //increment decay
      det  = det +1;

	  //reset led timer
      checkled = 0;
    }

	//if the servo timer has fired
    if(servocount > 8){
	   //if the firing queue has a job and the servos aren't in use
       if (!firing.isEmpty() and active == 'n' and timer == 0) {

		  //pop the first element and set it as active
          active = firing.dequeue();
       }

	  //if the servos are in use
      if(active != 'n'){

		//change the servos based on the current direction
        if(dir){
          servopos = servopos - 5;
        }
        else{
          servopos = servopos + 5;
        }

		//write to the active servo
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

		//if the servo go beyond 100 degrees, switch directions
        if(servopos < 100){
          dir = false;
        }

		//if the servos go beyond 180 degressm switch direction, reset the active servo and turn on a waiting timer
        if(servopos > 180){
          dir = true;
          active = 'n';
          timer = 100;
        }
      }

	  //reset servo timer
      servocount = 0;
    }

	//tick down the waiting timer if it's active
    if(timer > 0){
      timer = timer -1;
    }
  }
  //wait for a second
  delay(1);
}
