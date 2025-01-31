#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>  

const char* ssid = "Your SSID";
const char* password = "Your SSID PASWORD";

const char* apiKey = "Your API KEY";

const uint8_t deepseek_logo_30x30[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x02, 0x00, 0x00, 0x01, 0xfe, 0x0c, 0x00, 0x0f, 0xfe, 0x0e, 0x0c, 0x1f, 0xff, 0x0f, 0xfc,
  0x3f, 0xff, 0x87, 0xf8, 0x7f, 0xff, 0xc7, 0xf8, 0x7f, 0xff, 0xe3, 0xf0, 0xff, 0xff, 0xf3, 0x80,
  0xc0, 0xff, 0xff, 0x80, 0xc0, 0x7f, 0xbf, 0x80, 0xc0, 0x1f, 0x9f, 0x80, 0xe0, 0x0f, 0x8f, 0x00,
  0xe0, 0x0f, 0xcf, 0x00, 0x60, 0x07, 0xff, 0x00, 0x70, 0x03, 0xfe, 0x00, 0x78, 0x01, 0xfe, 0x00,
  0x38, 0x21, 0xfc, 0x00, 0x1e, 0x38, 0xfc, 0x00, 0x0f, 0xbc, 0x7e, 0x00, 0x07, 0xff, 0xfe, 0x00,
  0x01, 0xff, 0x80, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wifi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_BLUE);
  tft.setTextSize(2);
  tft.drawBitmap(100, 0, deepseek_logo_30x30, 30, 30, TFT_BLUE);
  tft.setCursor(10, 35);
  tft.println("DeepSeek: ");
  tft.setTextColor(TFT_WHITE);
  tft.println(" How Can I Help You ?");

  Serial.println("\nWiFi connection Success!");
  Serial.println("How Can I Help You");
}

void loop() {

  if (Serial.available() > 0) {
    String userQuestion = Serial.readStringUntil('\n');

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.drawBitmap(100, 0, deepseek_logo_30x30, 30, 30, TFT_BLUE);
    tft.setTextSize(2);
    tft.setCursor(10, 35);
    tft.println("Question: " + userQuestion);

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin("https://openrouter.ai/api/v1/chat/completions");
      http.addHeader("Content-Type", "application/json");
      http.addHeader("Authorization", String("Bearer ") + apiKey);

      StaticJsonDocument<512> jsonDoc;
      //jsonDoc["model"] = "openai/gpt-4o-mini-2024-07-18";  // For Gpt4o Language Model
      jsonDoc["model"] = "deepseek/deepseek-r1-distill-llama-70b"; // For DeepSeek R1 Language Model
      JsonArray messages = jsonDoc.createNestedArray("messages");

      JsonObject systemMessage = messages.createNestedObject();
      systemMessage["role"] = "system";
      systemMessage["content"] = "Answer";

      JsonObject userMessage = messages.createNestedObject();
      userMessage["role"] = "user";
      userMessage["content"] = userQuestion;

      String requestBody;
      serializeJson(jsonDoc, requestBody);

      int httpResponseCode = http.POST(requestBody);
      String response = http.getString();

      StaticJsonDocument<1024> responseDoc;
      DeserializationError error = deserializeJson(responseDoc, response);

      if (!error) {
        String assistantResponse = responseDoc["choices"][0]["message"]["content"].as<String>();

        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_BLUE);
        tft.setTextSize(2);
        tft.drawBitmap(100, 0, deepseek_logo_30x30, 30, 30, TFT_BLUE);
        tft.setCursor(10, 35);
        tft.println("DeepSeek: ");
        tft.setTextColor(TFT_WHITE);
        tft.println(assistantResponse);
      } else {
        tft.println("JSON Solve error!");
      }
      http.end();
    }
  }
}