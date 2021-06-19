#include <WiFi.h> 
#include "fpv_rover.h"

#include "esp_camera.h"
#define CAMERA_MODEL_AI_THINKER

#include "camera_pins.h"

#define RSSI_CYCLE 1000
#define WIFI_RECONNECT_DELAY 30000

// Pin connected to Rover LED
#define LED_PIN 2
// 4 PWM Pins connected to DRV8833
#define MOTOR_PIN1 13
#define MOTOR_PIN2 15
#define MOTOR_PIN3 12
#define MOTOR_PIN4 14

int rssi;

unsigned long last_rssi = 0;
unsigned long wifi_reconnect = 0;


void startCameraServer();
Buttons getWebButtons();


// enter your network data here
const char* ssid = "**************";
const char* password = "**************";

// Init Rover
FPV_Rover rover(MOTOR_PIN1, MOTOR_PIN2, MOTOR_PIN3, MOTOR_PIN4, LED_PIN);

// Set the server port number to default 80
WiFiServer server(80);



void setup() 
{
  // start serial communication
  Serial.begin(115200);

  setup_camera();
  setup_connect_wifi();
  startCameraServer();

  Serial.print("FPV-Rover Webserver Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}



void loop() {

  // Check signal strength
  check_RSSI();

  // Check if should try to reconnect
  check_reconnect();
  
  // Read buttons sended by webinterface
  rover.setButtons(getWebButtons());

  rover.adjust_track_speed();
}


void check_reconnect() {
  if ((rssi < -90) && ((millis() - wifi_reconnect) >= WIFI_RECONNECT_DELAY)) {
    searchConnectBetterAP();
    wifi_reconnect = millis();
  }
}


void check_RSSI() {
  if ((millis() - last_rssi) >= RSSI_CYCLE) {
    rssi = WiFi.RSSI();
    if (rssi < -85) rover.stop_move();
    last_rssi = millis();
  }
}



int getRSSI() {
  return rssi;
}



void searchConnectBetterAP() {
  int betterAP = 0;

  Serial.print("Connection is bad: ");
  Serial.println(rssi);
  Serial.println("Searching better AP ..");

  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("no networks found");
  }
  else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      
      if(!strcmp(WiFi.SSID(i).c_str(), ssid)) {
        Serial.print(" <-- possible AP: ");
        if (WiFi.RSSI(i) > rssi + 10) {
          Serial.println("trying reconnect");
          betterAP = 1;
        }
        else {
          Serial.println("not much better"); 
        }
      }
    }

    if (betterAP) {
      Serial.println("disconnecting and reconnecting");
      WiFi.disconnect();
      setup_connect_wifi();
    }
  }
}


void setup_connect_wifi() {
  Serial.print("Making connection to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
}




void setup_camera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();

  // drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_SVGA);
  s->set_hmirror(s, 1);        // 0 = disable , 1 = enable
  s->set_vflip(s, 1);          // 0 = disable , 1 = enable
}
