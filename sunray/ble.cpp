// Ardumower Sunray 
// Copyright (c) 2013-2020 by Alexander Grau, Grau GmbH
// Licensed GPLv3 for open source use
// or Grau GmbH Commercial License for commercial use (http://grauonline.de/cms2/?page_id=153)


// Bluetooth BLE/4.0 module (HC-08/HM-10 ZS-040)
// https://www.marotronics.de/HM-10-Bluetooth-BLE-BT-40-CC2541-CC2540-fuer-Arduino-iOS-Android-Low-Energy
// docs:  http://denethor.wlu.ca/arduino/MLT-BT05-AT-commands-TRANSLATED.pdf

// to send/receive data from this module, use BLE characteristic write/read:  
// UART_CHARACTERISTIC = "0000ffe1-0000-1000-8000-00805f9b34fb";



#include "ble.h"
#include <Arduino.h>
#include "config.h"


String BLEConfig::read(){
  String res;    
  unsigned long timeout = millis() + 2000;  
  while (millis() < timeout){
    while (BLE.available()){
      timeout = millis() + 200; 
      char ch = BLE.read();
      CONSOLE.print(ch);
      res += ch;
    }
  }
  return res;
}

String BLEConfig::exec(String cmd, bool doRetry){
  String res;
  delay(500);    
  for (int retry=0; retry < 3; retry++){
    CONSOLE.print("BLE: ");
    CONSOLE.print(cmd);
    BLE.print(cmd);
    res = read();
    if ((res != "") || (!doRetry)) break;
    CONSOLE.print("retry ");
    CONSOLE.print(retry+1);
    CONSOLE.print("  ");
  }
  return res;
}

void BLEConfig::run(){  
  int baud;
  bool found = false;
  //while (true){    
    for (int i=0; i < 12; i++){
      switch(i){
        /*case 0: baud=1200; break;  
        case 1: baud=2400; break;  
        case 2: baud=4800; break;  */
        case 3: baud=9600; break;  
        //case 4: baud=19200; break;  
        //case 5: baud=38400; break;  
        //case 6: baud=57600; break;  
        case 7: baud=115200; break;  
        /*case 8: baud=230400; break;  
        case 9: baud=460800; break;  
        case 10: baud=921600; break;  
        case 11: baud=1382400; break;  */
        default: continue;
      }
      CONSOLE.print("trying to detect Bluetooth 4.0/BLE module (make sure your phone is NOT connected)");
      CONSOLE.print(baud);
      CONSOLE.println("...");
      BLE.begin(baud);    
      //BLE.flush();
      String res = exec("AT\r\n", false);
      if (res.indexOf("OK") != -1){
        CONSOLE.println("Bluetooth 4.0/BLE module found!");
        if (baud == BLE_BAUDRATE) {
          found = true;
          break;
        } else {
          exec("AT+BAUD8\r\n", true);
          BLE.begin(BLE_BAUDRATE);
          found = true;
          break;
        }
      }
    }

    if (found) {
      exec("AT+VERSION\r\n", true);
      //exec("AT+LADDR\r\n", true);
      //exec("AT+CHAR\r\n", true);
#if defined(BLE_NAME)      
      exec("AT+NAME" BLE_NAME "\r\n", true);
#endif
      //exec("AT+TYPE2\r\n", true);                        
      //exec("AT+PASS111111\r\n", true);                  
      //exec("AT+HELP\r\n", true);                        
      //exec("AT+RESET\r\n", true);  // apply new settings and reboot module
      return;
    } else {
      CONSOLE.println("error: no BLE module found!");
    }
    //delay(1000);
  //}
}
