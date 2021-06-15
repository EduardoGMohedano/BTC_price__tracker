/*
 *  This sketch sends data via HTTP GET requests to Coin Desk API to check BTC current price,
 *  the current value of Bitcoin is shown in an OLED display or in serial monitor.
 *  
 *  The API returns a JSON restponse as the following example:
 *  
 *  {"time":{"updated":"Sep 18, 2013 17:27:00 UTC","updatedISO":"2013-09-18T17:27:00+00:00"},
 *  "disclaimer":"This data was produced from the CoinDesk Bitcoin Price Index. Non-USD currency data converted using hourly conversion rate from openexchangerates.org",
 *  "bpi":{"USD":{"code":"USD","symbol":"$","rate":"126.5235","description":"United States Dollar","rate_float":126.5235},"GBP":{"code":"GBP","symbol":"£","rate":"79.2495",
 *  "description":"British Pound Sterling","rate_float":79.2495},"EUR":{"code":"EUR","symbol":"€","rate":"94.7398","description":"Euro","rate_float":94.7398}}}
 *
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define OLED  1   //Compile time switch to show BTC price on OLED display, if this macro is not defined BTC price is shown in serial monitor
#define CD_API "https://api.coindesk.com/v1/bpi/currentprice/BTC.json"


#ifdef  OLED
  #include <SPI.h>
  #include <Wire.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>
  
  #define SCREEN_WIDTH 128 // OLED display width, in pixels
  #define SCREEN_HEIGHT 32 // OLED display height, in pixels
  #define OLED_RESET     -1 // price_intet pin # (or -1 if sharing Arduino price_intet pin)
  #define SCREEN_ADDRESS 0x3C ///< See datasheet for Addprice_ints; 0x3D for 128x64, 0x3C for 128x32
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#else
#endif

char* ssid     = "HUAWEI-106BU3";
char* password = "garciamohedano1!";

void setup()
{
    Serial.begin(115200);
    delay(10);

    #ifdef OLED
       // Oled init
      if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
      }
    
      // Show initial display buffer contents on the screen, the library initializes this with an Adafruit splash screen.
      display.display();
      delay(2000); 
    
      // Clear the buffer
      display.clearDisplay();
    #else
    #endif

    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP addprice_ints: ");
    Serial.println(WiFi.localIP());
}

int httpCode = 0;
DynamicJsonDocument doc(1000);
DeserializationError err;

void loop()
{
    delay(10000);
    // Use HTTPClient class to create a request
    HTTPClient http;   
    http.begin(CD_API); //here we can also indicate port and CA cert

    //Do a GET request and verify whether it was succesful by checking httpcode variable
    if( httpCode = http.GET() > 0 ){
      //Deserialize HTTP price_intponse to only get the BTC price
      String payload = http.getString();
      err=deserializeJson(doc, payload);
      if(err){
        Serial.println("deserializeJson() failed with code ");
        Serial.println(err.c_str());
      }

      //String manipulation to get an integer BTC price
      const char* price = doc["bpi"]["USD"]["rate"];
      String price_string(price);
      int point_index = price_string.indexOf('.');
      String price_int = price_string.substring(0,point_index);
      //Serial.println(price_int);
      #ifdef OLED      
        display.clearDisplay();
        display.setTextSize(1,2); // Draw 2X-scale text
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 8);
        display.print("BTC is $ ");
        display.print(price_int);
        display.println("  USD");
        display.setTextSize(0.5); // Draw 1/2X scale text
        display.print("Powered by CoinDesk");
        display.display();
       #else
        Serial.print("BTC is $");
        Serial.println(price);
       #endif
      //Serial.println(payload);
    }
    else{
      Serial.println(httpCode);
    }
    http.end();
    //Closing connection
}
