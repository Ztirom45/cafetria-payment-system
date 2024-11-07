/*
Code written by Ztirom45 (Moritz)
LICENSE: GPL4

Warning:
- SD-card user saving does not work at the moment
- This code is not ready for real usecases
- The users and theyer money are stored in RAM
*/

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Keypad.h>
#include <rgb_lcd.h>

//keypad setup
const byte ROWS = 4;
const byte COLS = 3;

char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
byte rowPins[ROWS] = { 5, 12, 11, 6 };
byte colPins[COLS] = { 9, 8, 7 };

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//lcd
rgb_lcd lcd;
SoftwareSerial SoftSerial(2, 3);

//User data:
//Warning cant store more than ten Users
typedef struct User {
  String id;
  int money;
  /*Sting name;*/
} User;

int USER_COUNT = 1;
#define USER_ARRAY_SIZE 10
User users[USER_ARRAY_SIZE] = {
  { "0116646F302C", 10000 },
};

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

void add_user(String id) {
  users[USER_COUNT]=(User){id,0};
  USER_COUNT+=1;

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("New user:");
  lcd.setCursor(0, 1);
  lcd.print(id);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Next One");
}
/*              SD card           */
#include <SPI.h>
#include <SD.h>
File myFile;

//not used at the moment
void setup_sd() {
  Serial.print("Init SD card...");

  if (!SD.begin(4)) {
    Serial.println(1);
    while (1)
      ;
  }
  Serial.println(0);

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  //myFile = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  /*
  if (myFile) {
    Serial.print(3);
    myFile.println("test");
    // close the file:
    myFile.close();
    Serial.println(0);
  } else {
    // if the file didn't open, print an error:
    Serial.println(4);
  }*/

  // re-open the file for reading:
  //read_from_sd();
}
//not used at the moment
void read_from_sd() {
  /*
    not implemented
  */
  myFile = SD.open("test.txt");
  String data = "";
  if (myFile) {
    
    // read from the file until there's nothing else in it:
    while (myFile.available()){
      data = myFile.readString();
    }
    Serial.println(data);

    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println(5);
  }
  //parse data to user array
  int pos = data.lastIndexOf("\n");
  int counter = 1;
  while (pos != -1) {//separates string in reverse order and pushes to args
    String user = data.substring(0,pos);
    //Serial.println(user);
    int seperator = user.lastIndexOf(",");
    //Serial.println(user.substring(0,seperator));
    //Serial.println(user.substring(seperator+1,user.lastIndexOf("\n")));
    Serial.println(user.substring(0,seperator-2));
    users[counter].id=(String)(user.substring(0,seperator));
    Serial.println(users[counter].id);
    users[counter++].money=user.substring(seperator+1,user.lastIndexOf("\n")).toInt();
    data.remove(pos, pos + user.length());
    pos = data.lastIndexOf("\n");
  }
  Serial.println(users[1].id);
  Serial.println(users[1].money);
}

void setup() {
  Serial.begin(9600);  // the Serial port of Arduino baud rate.
  //setup_sd();
  SoftSerial.begin(9600);  // the SoftSerial baud rate

  lcd.begin(16, 2);

  lcd.setRGB(255, 255, 255);

  // Print a message to the LCD.
  lcd.print("Next One");
}

void loop() {
  if (millis() > 3000) {// start loop after 3 secs
    if (SoftSerial.available()) {
      String buffer2 = "";

      if (SoftSerial.available())  // reading data into char array
      {
        buffer2 = SoftSerial.readString();
        buffer2 = buffer2.substring(1, buffer2.length() - 1);  //cut weard chars away
      }
      bool known_user = false;
      for (int user_pointer = 0; user_pointer < USER_COUNT; user_pointer++) {
        if (users[user_pointer].id == buffer2) {
          users[user_pointer] = user_edit(users[user_pointer]);
          Serial.println(users[user_pointer].money);
          known_user = true;
          break;
        }
      }
      if (!known_user) {
        add_user(buffer2);
      }
      Serial.println(buffer2);
    }
    if (Serial.available())             // if data is available on hardware serial port ==> data is coming from PC or notebook
      SoftSerial.write(Serial.read());  // write it to the SoftSerial shield
  }
}
