<h1 ailgn="center">Run start sample plant systems on ESP-NOW Protocol</h1>

<br><br>

<div style="text-align: center;">
    <img src="https://github.com/Jasionf/ESP_NOW/raw/Main/download-removebg-preview-removebg-preview.png" style="width: 1000px; height: auto;" />
</div>

<br><br>

> _This project is using the ESP-NOW protocol launched by espressif , to manufacture a samples plant system . With more and more technonlogical choices available for smart agriculture ,  Many wireless communcation protocols have emerged . But i find the ESP-NOW protocol can help us to quckily create a greatly projcet , and bettter chocies for environmental scope , where it refers to network connection , communcation distances , and so on._

## Preparation Work

In this project , i am using ***Seeed Studio*** development board and Grove Sensors , Which it provide a large number of WiKi can help us quckily start and create project you want , it is friendly to novice beginners or those with a weak foundation.

### Development Boards
- *XIAO ESP32S3*
- *XIAO ESP32S3*
- *XIAO ESP32S3*

### Grove Sensors
- *LED Grove*
- *DHT20 Grove*
- *Spray Grove*
- *Light Grove*
- *OLED Screen*

## System Functions 

***This demo is not diffcult , so i will be from three XIAO ESP32 aspects to introduce the roles they play.***

### Base Function

- DHT20 Grove/Light Grove/OLED Screen,three Sensors connect to XIAO ESP32S3 , it play a role is collect plant enviorment detail information display on OLED Screen , besides send the collected information to the corresponding object.
 
- Spray Grove connected in XIAO ESP32C3 , there you can set humidity threshold value , if XIAO ESP32C3 receive humidity from XIAO ESP32S3 greater than threshold value , connnected in XIAO ESP32C3 Spray Grove Sensor will be run start , until less than threshold value.

- LED Grove connected in XIAO ESP32C6 , there you can set light intensity threshold value , if XIAO ESP32C6 reveive light from XIAO ESP32C6 greater than threshold value , connected in XIAO ESP32Cc LED Grove Sensor will be ruun start , unitl less than threshold value.

### Technical Points

***The three development boards transfer data from Grove sensors using the ESP-NOW protocol. Here are the key technical points:***

- **MAC Address Requirement**: Each XIAO ESP32 board needs to have its MAC address configured to allow communication between the devices.
  
- **WiFi Mode Configuration**: Set up the WiFi mode to STA (Station mode) for proper connectivity.

- **ESP-NOW Protocol**: This protocol enables low-power, low-latency communication between devices without the need for a traditional WiFi infrastructure, making it ideal for IoT applications.

- **Data Transmission**: Each board collects data from its connected sensors and transmits it to other boards using the ESP-NOW protocol, ensuring efficient communication.

### ESP-NOW Features

- **Low Power Consumption**: Ideal for battery-operated devices, extending their operational lifespan.

- **Low Latency**: Provides fast data transmission, suitable for real-time applications like sensor data responses.

- **No WiFi Router Required**: Allows devices to communicate directly without the need for a traditional WiFi network, simplifying setup.

- **Many-to-Many Communication**: Supports data exchange among multiple devices simultaneously, enhancing system flexibility.

- **Security**: Supports encryption to ensure secure data transmission and protect user privacy.


<h1 ailgn="center">
    🤘That's it, everyone can use this to modify a better demo🤘
</h1>

<p align="center">
        <img src="https://raw.githubusercontent.com/mayhemantt/mayhemantt/Update/svg/Bottom.svg" alt="Github Stats" />
</p>

