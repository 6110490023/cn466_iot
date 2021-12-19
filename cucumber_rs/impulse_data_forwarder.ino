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

const char SENSOR_TOPIC[] = "cn466/sensors/cucumber_nut";


// persistence variables
Adafruit_HTS221 hts;
Adafruit_MPU6050 mpu;
Adafruit_NeoPixel pixels(1, RGBLED_PIN, NEO_GRB + NEO_KHZ800);



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

// initialization
void setup() {
  Serial.begin(115200);
  setupHardware();
  Serial.println("Starting");
}

void loop() {
  static uint32_t prev_millis = 0;
  char json_body[200];
  const char json_test[] = "%.2f,%.2f,%.2f";
  sensors_event_t temp, humid;
  sensors_event_t a, g;

  if (millis() - prev_millis > 100) {
    prev_millis = millis();
    mpu.getEvent(&a, &g, &temp);
    //float ax = a.acceleration.x;
    //float ay = a.acceleration.y;
    //float az = a.acceleration.z;
    float gx = g.gyro.x;
    float gy = g.gyro.y;
    float gz = g.gyro.z;
    //sprintf(json_body, json_tmpl, p, t, h, ax, ay, az, gx, gy, gz);
    sprintf(json_body, json_test, gx, gy, gz);
    Serial.println(json_body);
  }
   delay(100);
  }
 
