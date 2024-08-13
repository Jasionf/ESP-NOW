#include "WiFi.h"
#include "esp_now.h" 
#include "DHT.h"
#include "Wire.h"
#include "Digital_Light_TSL2561.h"
#include <U8x8lib.h>

#define ESPNOW_WIFI_CHANNEL 0
#define MAX_ESP_NOW_MAC_LEN 6
#define BAUD 115200
#define MAX_CHARACTERS_NUMBER 20
#define NO_PMK_KEY false
#define ledPin D0
#define DHTTYPE DHT20 
#define OLED_LEN 5


U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(SCL, SDA, U8X8_PIN_NONE);

#define SUCCESSFUL 1
#define FAILD 0

float temp_hum_val[2] = {0};

DHT dht(DHTTYPE);  

typedef uint8_t XIAO;
typedef int XIAO_status;

static uint8_t Receiver_XIAOC3_MAC_Address[MAX_ESP_NOW_MAC_LEN] = {0x64, 0xe8, 0x33, 0x89, 0x80, 0xb8};
static uint8_t Receiver_XIAOC6_MAC_Address[MAX_ESP_NOW_MAC_LEN] = {0xf0, 0xf5, 0xbd, 0x1a, 0x97, 0x20};

esp_now_peer_info_t peerInfo;
esp_now_peer_info_t peerInfo1;

typedef struct OLED{
  char humidity_str[OLED_LEN];
  char temperature_str[OLED_LEN];
  char lightIntensity_str[OLED_LEN];
}OLED;

OLED XIAO_OLED_Driver;

typedef struct XIAOC3_meesage_types{
  char XIAOC3_Reveiver_device[MAX_CHARACTERS_NUMBER];
  char XIAOC3_Reveiver_Trag[MAX_CHARACTERS_NUMBER]; 
  int atomizing_threshold;
  int XIAOC3_connect_sign;
  char atomizing_status[MAX_CHARACTERS_NUMBER];

}XIAOC3_meesage_types;

XIAOC3_meesage_types XIAOC3_RECEIVER_INFORATION;

typedef struct XIAOC6_meesage_types{
  char XIAOC6_Reveiver_device[MAX_CHARACTERS_NUMBER];
  char XIAOC6_Reveiver_Trag[MAX_CHARACTERS_NUMBER];
  int miniLED_threshold;
  int XIAOC6_connect_sign;
  char miniLED_status[MAX_CHARACTERS_NUMBER];

}XIAOC6_meesage_types;

XIAOC6_meesage_types XIAOC6_RECEIVER_INFORATION;


typedef struct XIAOS3_message_types{
  char device[MAX_CHARACTERS_NUMBER];
  char Trag[MAX_CHARACTERS_NUMBER];
  float light_intensity_feedback;
  float temperature_feedback;
  float humidity_feedback;
}XIAOS3_message_types;

XIAOS3_message_types Send_XIAOC3_Information;
XIAOS3_message_types Send_XIAOC6_Information;

void espnow_init();
void espnow_deinit();
void SenderXIAOS3_MACAddress_Requir();
void SenderXIAOS3_Send_Data();
void SenderXIAOS3_Send_Data_cb(const XIAO *mac_addr,esp_now_send_status_t status);
void Association_ReceiverXIAOC3_peer();
void Association_ReceiverXIAOC6_peer();
void ReceiverXIAOC3_Recive_Data_cb(const esp_now_recv_info *info, const uint8_t *incomingData, int len);
void ReceiverXIAOC6_Recive_Data_cb(const esp_now_recv_info *info, const uint8_t *incomingData, int len);
void connect_LED();
float humidity_value();
float temperature_value();
float light_instensity_value();
void delete_peer_C6();
void delete_peer_C3();

void setup(){
    Wire.begin();
    TSL2561.init();
    dht.begin();
    u8x8.begin();
    u8x8.setFlipMode(3);   
    Serial.begin(BAUD);
    while(!Serial);
    SenderXIAOS3_MACAddress_Requir();
    espnow_init();
    pinMode(ledPin,OUTPUT);
    esp_now_register_send_cb(SenderXIAOS3_Send_Data_cb);

    Association_ReceiverXIAOC6_peer();
    Association_ReceiverXIAOC3_peer();
    // delete_peer_C3();
    esp_now_register_recv_cb(ReceiverXIAOC6_Recive_Data_cb);
    esp_now_register_recv_cb(ReceiverXIAOC3_Recive_Data_cb);
    connect_LED();
}

void loop(){
  humidity_value();
  temperature_value();
  OLED_driver();
  Serial.println(XIAOC3_RECEIVER_INFORATION.XIAOC3_connect_sign);
  Serial.println(XIAOC6_RECEIVER_INFORATION.XIAOC6_connect_sign);
  SenderXIAOS3_Send_Data();
  connect_LED();
  delay(100);
}

void SenderXIAOS3_Send_Data_cb(const XIAO *mac_addr,esp_now_send_status_t status){
  char macStr[18];
  Serial.print("Packet to: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  delay(500);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  Serial.println("");
}

void Association_ReceiverXIAOC3_peer(){
  Serial.println("Attempting to associate peer for XIAOC3...");
  peerInfo.channel = ESPNOW_WIFI_CHANNEL;
  peerInfo.encrypt = NO_PMK_KEY;

  memcpy(peerInfo.peer_addr, Receiver_XIAOC3_MAC_Address, 6);
  esp_err_t addPressStatus = esp_now_add_peer(&peerInfo);
  if (addPressStatus != ESP_OK)
  {
    Serial.print("Failed to add peer");
    Serial.println(addPressStatus);
  }else
  {
    Serial.println("Successful to add peer");
  }
}

void Association_ReceiverXIAOC6_peer(){
  Serial.println("Attempting to associate peer for XIAOC6...");
  peerInfo1.channel = ESPNOW_WIFI_CHANNEL;
  peerInfo1.encrypt = NO_PMK_KEY;

  memcpy(peerInfo1.peer_addr, Receiver_XIAOC6_MAC_Address, 6);
  esp_err_t addPressStatus = esp_now_add_peer(&peerInfo1);
  if (addPressStatus != ESP_OK)
  {
    Serial.print("Failed to add peer");
    Serial.println(addPressStatus);
  }else
  {
    Serial.println("Successful to add peer");
  }
}

void SenderXIAOS3_Send_Data(){
  
  Send_XIAOC3_Information.humidity_feedback = humidity_value();
  Send_XIAOC3_Information.temperature_feedback = temperature_value();

  strcpy(Send_XIAOC3_Information.device, "XIAOS3"); 
  strcpy(Send_XIAOC3_Information.Trag, "Hello,i'm sender"); 

  Send_XIAOC6_Information.light_intensity_feedback = light_instensity_value();

  strcpy(Send_XIAOC6_Information.device, "XIAOS3"); 
  strcpy(Send_XIAOC6_Information.Trag, "Hello,i'm sender"); 

  esp_err_t XIAOS3_RECEIVER_INFORATION_data1 = esp_now_send(Receiver_XIAOC3_MAC_Address, (uint8_t *)&Send_XIAOC3_Information, sizeof(XIAOS3_message_types));
  esp_err_t XIAOS3_RECEIVER_INFORATION_data2 = esp_now_send(Receiver_XIAOC6_MAC_Address, (uint8_t *)&Send_XIAOC6_Information, sizeof(XIAOC6_meesage_types));

  if (XIAOS3_RECEIVER_INFORATION_data1 == ESP_OK && XIAOS3_RECEIVER_INFORATION_data2 == ESP_OK)
  {
    Serial.println("Sent with success: XIAOS3_RECEIVER_INFORATION_data1 and XIAOS3_RECEIVER_INFORATION_data2");
  }
  delay(4000);
}

void ReceiverXIAOC3_Recive_Data_cb(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  memcpy(&XIAOC3_RECEIVER_INFORATION, incomingData, sizeof(XIAOC3_RECEIVER_INFORATION));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Reveiver_device: ");
  Serial.println(XIAOC3_RECEIVER_INFORATION.XIAOC3_Reveiver_device);
  Serial.print("Reveiver_Trag: ");
  Serial.println(XIAOC3_RECEIVER_INFORATION.XIAOC3_Reveiver_Trag);
  Serial.print("XIAOC3_connect_sign: ");
  Serial.println(XIAOC3_RECEIVER_INFORATION.XIAOC3_connect_sign);
  Serial.print("atomizing_status: ");
  Serial.println(XIAOC3_RECEIVER_INFORATION.atomizing_status);
  Serial.println();
}

void ReceiverXIAOC6_Recive_Data_cb(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  memcpy(&XIAOC6_RECEIVER_INFORATION, incomingData, sizeof(XIAOC6_RECEIVER_INFORATION));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Reveiver_device: ");
  Serial.println(XIAOC6_RECEIVER_INFORATION.XIAOC6_Reveiver_device);
  Serial.print("Reveiver_Trag: ");
  Serial.println(XIAOC6_RECEIVER_INFORATION.XIAOC6_Reveiver_Trag);
  Serial.print("XIAOC6_connect_sign: ");
  Serial.println(XIAOC6_RECEIVER_INFORATION.XIAOC6_connect_sign);
  Serial.print("miniLED_status: ");
  Serial.println(XIAOC6_RECEIVER_INFORATION.miniLED_status);
  Serial.println();
}

void SenderXIAOS3_MACAddress_Requir(){
    WiFi.mode(WIFI_STA);
    WiFi.setChannel(ESPNOW_WIFI_CHANNEL);
    XIAO mac[MAX_ESP_NOW_MAC_LEN];
    while(!WiFi.STA.started()){
      Serial.print(".");
      delay(100);
    }
      WiFi.macAddress(mac);
      Serial.println();
      Serial.printf("const uint8_t mac_self[6] = {0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x};", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
      Serial.println();
}

void espnow_init(){
  XIAO_status espnow_sign = esp_now_init();
  if(espnow_sign == ESP_OK)
  {
    Serial.println("the esp now is successful init!");
  }else
  {
    Serial.println("the esp now is failed init");
  }
}

void espnow_deinit(){
  XIAO_status espnow_sign = esp_now_deinit();
  if(espnow_sign == ESP_OK){
    Serial.println("the esp now is successful deinit!");
  }else
  {
    Serial.println("the esp now is failed deinit!");
  }
}

void connect_LED(){
  if(XIAOC3_RECEIVER_INFORATION.XIAOC3_connect_sign == FAILD)
  {
    connect_status_led();
  }else if(XIAOC3_RECEIVER_INFORATION.XIAOC3_connect_sign == SUCCESSFUL)
  {
    close_status_led();
  }
}

void OLED_driver(){
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.setCursor(0,0);
  u8x8.print("Temp : ");
  u8x8.setCursor(0,1);
  u8x8.print("Humid : ");
  u8x8.setCursor(0,2);
  u8x8.print("Light : ");
  u8x8.setCursor(9,1);
  u8x8.print(" %\t");
  u8x8.setCursor(9,0);
  u8x8.print(" *C");
  u8x8.setCursor(9,2);
  u8x8.print(" Cd");
  u8x8.setCursor(1,6);
  u8x8.print("Seeed Studio");

  Send_XIAOC3_Information.humidity_feedback = humidity_value();
  Send_XIAOC3_Information.temperature_feedback = temperature_value();
  Send_XIAOC3_Information.light_intensity_feedback = light_instensity_value();

  strcpy(XIAO_OLED_Driver.humidity_str,u8x8_u8toa(Send_XIAOC3_Information.humidity_feedback,2));
  strcpy(XIAO_OLED_Driver.temperature_str,u8x8_u8toa(Send_XIAOC3_Information.temperature_feedback,2));
  strcpy(XIAO_OLED_Driver.lightIntensity_str,u8x8_u8toa(Send_XIAOC3_Information.light_intensity_feedback,2));
  
  u8x8.drawString(8, 0, XIAO_OLED_Driver.temperature_str);
  u8x8.drawString(8, 1, XIAO_OLED_Driver.humidity_str);
  u8x8.drawString(8, 2, XIAO_OLED_Driver.lightIntensity_str);
}

float humidity_value(){
  if (!dht.readTempAndHumidity(temp_hum_val)) 
  {
    return temp_hum_val[0];
  }
}

float temperature_value(){
  if (!dht.readTempAndHumidity(temp_hum_val)) 
  {
    return temp_hum_val[1];
  }
}

float light_instensity_value()
{
  return TSL2561.readVisibleLux();
}

void delete_peer_C6(){
  esp_err_t del_status = esp_now_del_peer(Receiver_XIAOC6_MAC_Address);
  if(del_status == ESP_OK){
    Serial.println("the C6_peer is complete delete");
  }else{
    Serial.println("the C6_peer is delete faild");
    }
  }

void delete_peer_C3(){
  esp_err_t del_status = esp_now_del_peer(Receiver_XIAOC3_MAC_Address);
  if(del_status == ESP_OK){
    Serial.println("the C3_peer is complete delete");
  }else{
    Serial.println("the C3_peer is delete faild");
    }
}


void connect_status_led()
{
  for(int fadeValue = 0; fadeValue <= 255; fadeValue +=5) {
  analogWrite(ledPin, fadeValue);
  delay(20);
  }
  for(int fadeValue = 255; fadeValue >= 0; fadeValue -=5) {
    analogWrite(ledPin, fadeValue);
    delay(20);
  }
}

void close_status_led(){
  for(int fadeValue = 255; fadeValue >= 0; fadeValue -=5) {
  analogWrite(ledPin, fadeValue);
  }
}
