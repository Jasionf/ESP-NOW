#include<Arduino.h>
#include "WiFi.h"
#include "esp_now.h"

#define ESPNOW_WIFI_CHANNEL 0
#define MAX_ESP_NOW_MAC_LEN 6
#define BAUD 115200
#define MAX_CHARACTERS_NUMBER 20
#define NO_PMK_KEY false
#define ledPin A0

#define SUCCESSFUL 1
#define FAILD 0

typedef uint8_t XIAO;
typedef int status;

static uint8_t XIAOS3_Sender_MAC_Address[MAX_ESP_NOW_MAC_LEN] = {0x64, 0xe8, 0x33, 0x51, 0x1e, 0xe4};

esp_now_peer_info_t peerInfo_sender;

typedef struct XIAOC6_meesage_types{
  char XIAOC6_Reveiver_device[MAX_CHARACTERS_NUMBER];
  char XIAOC6_Reveiver_Trag[MAX_CHARACTERS_NUMBER];
  int miniLED_threshold;
  int XIAOC6_connect_sign;
  char miniLED_status[MAX_CHARACTERS_NUMBER];

}XIAOC6_meesage_types;

XIAOC6_meesage_types XIAOC6_RECEIVER_INFORATION;

/*sender information mumber*/
typedef struct XIAOS3_message_types{
  char device[MAX_CHARACTERS_NUMBER];
  char Trag[MAX_CHARACTERS_NUMBER];
  float light_intensity_feedback;
  float temperature_feedback;
  float humidity_feedback;
}XIAOS3_message_types;

XIAOS3_message_types XIAOS3_SENDER_INFORATION;

void Receiver_MACAddress_requir();
void espnow_init();
void espnow_deinit();
void ReceiverXIAOC6_Recive_Data_cb(const uint8_t * mac, const uint8_t *incomingData, int len);
void ReceiverXIAOC6_Send_Data();
void ReceiverXIAOC6_Send_Data_cb(const XIAO *mac_addr,esp_now_send_status_t status);
void Association_SenderXIAOS3_peer();
void miniLED_driver();

void setup() {
  Serial.begin(BAUD);
  while(!Serial);
  Receiver_MACAddress_requir();
  espnow_init();

  esp_now_register_recv_cb(ReceiverXIAOC6_Recive_Data_cb);

  esp_now_register_send_cb(ReceiverXIAOC6_Send_Data_cb);
  
  Association_SenderXIAOS3_peer();  
}

void loop() {
  miniLED_driver();
  ReceiverXIAOC6_Send_Data();
  delay(1000);
}

void espnow_init(){
  status espnow_sign = esp_now_init();
  if(espnow_sign == ESP_OK)
  {
    Serial.println("the esp now is successful init!");
  }else
  {
    Serial.println("the esp now is failed init");
  }
}

void espnow_deinit(){
  status espnow_sign = esp_now_deinit();
  if(espnow_sign == ESP_OK){
    Serial.println("the esp now is successful deinit!");
  }else
  {
    Serial.println("the esp now is failed deinit!");
  }
}

void Receiver_MACAddress_requir(){
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

void ReceiverXIAOC6_Recive_Data_cb(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  memcpy(&XIAOS3_SENDER_INFORATION, incomingData, sizeof(XIAOS3_SENDER_INFORATION));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Sender_device: ");
  Serial.println(XIAOS3_SENDER_INFORATION.device);
  Serial.print("Sender_Trag: ");
  Serial.println(XIAOS3_SENDER_INFORATION.Trag);
  Serial.print("The light instensity feedvack value is : ");
  Serial.println(XIAOS3_SENDER_INFORATION.light_intensity_feedback);
  Serial.println();
}

void ReceiverXIAOC6_Send_Data_cb(const XIAO *mac_addr,esp_now_send_status_t status){
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

void ReceiverXIAOC6_Send_Data(){
  strcpy(XIAOC6_RECEIVER_INFORATION.XIAOC6_Reveiver_device, "XIAOC6"); 
  strcpy(XIAOC6_RECEIVER_INFORATION.XIAOC6_Reveiver_Trag, "I'm get it"); 
  XIAOC6_RECEIVER_INFORATION.XIAOC6_connect_sign = SUCCESSFUL;
  miniLED_driver();

  esp_err_t XIAOC6_RECEIVER_INFORATION_data1 = esp_now_send(XIAOS3_Sender_MAC_Address, (uint8_t *)&XIAOC6_RECEIVER_INFORATION, sizeof(XIAOC6_meesage_types));

  if (XIAOC6_RECEIVER_INFORATION_data1 == ESP_OK)
  {
    Serial.println("Sent with success: XIAOC6_RECEIVER_INFORATION_data1");
  }
  delay(4000);
}

void Association_SenderXIAOS3_peer(){
  Serial.println("Attempting to associate peer for XIAOS3...");
  peerInfo_sender.channel = ESPNOW_WIFI_CHANNEL;
  peerInfo_sender.encrypt = NO_PMK_KEY;

  memcpy(peerInfo_sender.peer_addr, XIAOS3_Sender_MAC_Address, 6);
  esp_err_t addPressStatus = esp_now_add_peer(&peerInfo_sender);
  if (addPressStatus != ESP_OK)
  {
    Serial.print("Failed to add peer");
    Serial.println(addPressStatus);
  }else
  {
    Serial.println("Successful to add peer");
  }
}

void miniLED_driver(){
  if(XIAOS3_SENDER_INFORATION.light_intensity_feedback == XIAOC6_RECEIVER_INFORATION.miniLED_threshold)
  {
    analogWrite(ledPin,255);
    strcpy(XIAOC6_RECEIVER_INFORATION.miniLED_status, "the light is open "); 
  }else if(XIAOS3_SENDER_INFORATION.light_intensity_feedback > XIAOC6_RECEIVER_INFORATION.miniLED_threshold )
  {
    analogWrite(ledPin,0);
    strcpy(XIAOC6_RECEIVER_INFORATION.miniLED_status, "the light is close "); 
  }
}

void delete_peer(){
  esp_err_t del_status = esp_now_del_peer(XIAOS3_Sender_MAC_Address);
  if(del_status == ESP_OK){
    Serial.println("the peer is complete delete");
  }else{
    Serial.println("the peer is delete faild");
    }
}
