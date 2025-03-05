/*
Code written by Ztirom45
LICENSE: GPL4

Warning:
- This code is not ready for real usecases
*/

//include libraries
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Keypad.h>
#include <rgb_lcd.h>

//include own librarys
#include <user.hpp>
#include <data.hpp>

//keypad setup
const byte ROWS = 4;
const byte COLS = 3;
//define chars on keypad
char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
//define used pins (keypad works as a button matrix)
byte rowPins[ROWS] = { 5, 1, 0, 6 };
byte colPins[COLS] = { 9, 8, 7 };

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//lcd
rgb_lcd lcd;
SoftwareSerial SoftSerial(2, 3);


User user_edit(User user) {
  lcd.setCursor(0, 0);
  lcd.print(user.id);
  lcd.setCursor(0, 1);
  lcd.print(user.money);
  lcd.print("ct");
  bool loop = true;
  int changes_pos = String(user.money).length() + 4;
  bool add = true;
  String number = "";
  while (loop) {
    char customKey = customKeypad.getKey();
    if (customKey) {
      if (customKey == (char)'#') {  //enter
        loop = false;
      } else if (customKey == (char)'*') {  // +/-
        lcd.setCursor(changes_pos, 1);
        if (add) {
          lcd.print("-");
          add = false;
        } else {
          lcd.print("+");
          add = true;
        }
      } else {
        number += customKey;
        lcd.setCursor(changes_pos + 1, 1);
        lcd.print(number);
      }
    }
  }
  lcd.setCursor(0, 1);
  lcd.print("new: ");
  if (add) {
    user.money += number.toInt();
  } else {
    user.money -= number.toInt();
  }
  lcd.print(user.money);
  lcd.print("       ");
  delay(500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Next One");
  Serial.println("done");
  return user;
}

/*
add user datastructure to ram
show update on screen
*/
void add_user(String id) {
  users[USER_COUNT]=(User){id,0};
  USER_COUNT+=1;

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("New user:");
  lcd.setCursor(0, 1);
  lcd.print(id);
  delay(2000);
  //reset LCD screen
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Next One");
}


void setup() {
  
  setup_sd();
  
  //setup RFID reader 
  SoftSerial.begin(9600);
 
  //setup lcd 
  lcd.begin(16, 2);
  lcd.setRGB(255, 255, 255);
  lcd.print("Next One");
}

void loop() {
  if (millis() > 5000) {// start loop after 5 secs to avoid memory issues from slow loading libraries
    if (SoftSerial.available()) {
      String buffer2 = "";

      if (SoftSerial.available())  //parse data into String
      {
        buffer2 = SoftSerial.readString();
        buffer2 = buffer2.substring(1, buffer2.length() - 1);  //cut weard chars away
      }
      //compare known users with the RFID tag
      bool known_user = false;
      for (int user_pointer = 0; user_pointer < USER_COUNT; user_pointer++) {
        if (users[user_pointer].id == buffer2) {
          users[user_pointer] = user_edit(users[user_pointer]);
          save_user();
          known_user = true;
          break;
        }
      }
      if (!known_user) {
        add_user(buffer2);
      }
    }
  }
}
