
/* Edge Impulse Arduino examples
   Copyright (c) 2021 EdgeImpulse Inc.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

/* Includes ---------------------------------------------------------------- */
#include <iot_door_eknarin_inferencing.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_HTS221.h>
#include <Adafruit_NeoPixel.h>
#include <ESPAsync_WiFiManager.h>
#include <PubSubClient.h>

// constants
#define I2C_SDA     41
#define I2C_SCL     40
#define LED_PIN     2
#define RGBLED_PIN  18

const char SENSOR_TOPIC[] = "cn466/door/cucumber_nut";
//const char LED_TOPIC[] = "cn466/leds/cucumber_nut";

// persistence variables
Adafruit_HTS221 hts;
Adafruit_MPU6050 mpu;
Adafruit_NeoPixel pixels(1, RGBLED_PIN, NEO_GRB + NEO_KHZ800);

AsyncWebServer webServer(80);
WiFiClient esp32Client;
DNSServer dnsServer;
PubSubClient mqttClient(esp32Client);


// setup sensors and LED
void setupHardware() {
  Wire.begin(I2C_SDA, I2C_SCL, 100000);
  if (hts.begin_I2C()) {    // prepare HTS221 sensor
    Serial.println("HTS221 sensor ready");
  }
  if (mpu.begin()) {       // prepare MPU6050 sensor
    Serial.println("MPU6050 sensor ready");
  }
  pinMode(LED_PIN, OUTPUT);      // prepare LED
  digitalWrite(LED_PIN, HIGH);
  pixels.begin();                // prepare NeoPixel LED
  pixels.clear();
}
void setupNetwork() {
  // WiFi
  WiFi.begin("3BB_SARAWUT(2.4GHz)", "0315232312");
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // HiveMQ
  mqttClient.setServer("broker.hivemq.com", 1883);
  //mqttClient.setCallback(callback);
}
void connectBroker() {
  char client_id[20];

  sprintf(client_id, "cucumber-%d", esp_random() % 10000);
  if (mqttClient.connect(client_id)) {
    //  mqttClient.subscribe(LED_TOPIC);
  }
}

static float features[] = {
  -0.0500, 0.0000, -0.0200,
  -0.0600, 0.0000, -0.0200,
  -0.0500, 0.0000, -0.0200,
  -0.0600, 0.0000, -0.0200,
  -0.0600, 0.0000, -0.0200,
  -0.0500, -0.0100, -0.0200,
  -0.0500, 0.0000, -0.0200,
  -0.0500, -0.0100, -0.0200,
  -0.0500, -0.0100, -0.0200,
  -0.0500, 0.0000, -0.0200,
  -0.0600, 0.0200, -0.0200,
  -0.0600, 0.0000, -0.0200,
  -0.0500, -0.0200, -0.0200,
  -0.0500, -0.0300, -0.0200,
  0.0200, -0.2700, -0.0200,
  0.1400, -0.6900, -0.0200,
  0.2300, -0.9400, -0.0200,
  0.2600, -1.0700, -0.0200
};

/**
   @brief      Copy raw feature data in out_ptr
               Function called by inference library

   @param[in]  offset   The offset
   @param[in]  length   The length
   @param      out_ptr  The out pointer

   @return     0
*/
int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
  memcpy(out_ptr, features + offset, length * sizeof(float));
  return 0;
}


/**
   @brief      Arduino setup function
*/
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  setupHardware();
  setupNetwork();
  connectBroker();
  Serial.println("Starting");
}

/**
   @brief      Arduino main function
*/
char json_test[] = "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f";
char *st = "undefined";
char *record = "undefined" ;
int i = 54;
void loop()
{ char json_body[100];
  const char json_door[] = "{\"status\": \"%s\"}";
  static uint32_t prev_millis = 0;
  sensors_event_t temp, humid;
  sensors_event_t a, g;
  if (i < 54) {
    if (millis() - prev_millis > 100) {
      prev_millis = millis();
      hts.getEvent(&humid, &temp);
      mpu.getEvent(&a, &g, &temp);
      float gx = g.gyro.x;
      float gy = g.gyro.y;
      float gz = g.gyro.z;
      features[i] = gx;
      features[i+1] = gy;
      features[i+2] = gz;
      i= i+3;
      Serial.printf("%d ", i);
    }
  }
  if(i == 54) {
    ei_printf("\n Edge Impulse standalone inferencing (Arduino)\n");

    if (sizeof(features) / sizeof(float) != EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
      ei_printf("The size of your 'features' array is not correct. Expected %lu items, but had %lu\n",
                EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, sizeof(features) / sizeof(float));
      delay(1000);
      return;
    }

    ei_impulse_result_t result = { 0 };

    // the features are stored into flash, and we don't want to load everything into RAM
    signal_t features_signal;
    features_signal.total_length = sizeof(features) / sizeof(features[0]);
    features_signal.get_data = &raw_feature_get_data;

    // invoke the impulse
    EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false /* debug */);
    ei_printf("run_classifier returned: %d\n", res);
    if (res != 0) return;
    // print the predictions
    ei_printf("Predictions ");
    ei_printf(": \n");
    // human-readable predictions
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
      ei_printf("    %s: %.5f\n", result.classification[ix].label, result.classification[ix].value);
    }

    if (result.classification[0].value > 0.85 ) {
      record = "close";
    }
    if (result.classification[1].value > 0.85 ) {
      record = "open";
    }
    i= 0;
    Serial.printf("\n %s \n", st);
    delay(1000);
  }

  
  if (strcmp(st, record) != 0) {
    st = record;
    sprintf(json_body, json_door, st);
    Serial.println(json_body);
    mqttClient.publish(SENSOR_TOPIC, json_body);
  }
  if (!mqttClient.connected()) {
    connectBroker();
  }
  mqttClient.loop();
  delay(100);

}

/**
   @brief      Printf function uses vsnprintf and output using Arduino Serial

   @param[in]  format     Variable argument list
*/
void ei_printf(const char *format, ...) {
  static char print_buf[1024] = { 0 };

  va_list args;
  va_start(args, format);
  int r = vsnprintf(print_buf, sizeof(print_buf), format, args);
  va_end(args);

  if (r > 0) {
    Serial.write(print_buf);
  }
}
