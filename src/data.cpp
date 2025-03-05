/*
      * SD card attached to SPI bus as follows:
     ** MOSI - pin 11 on Arduino Uno/Duemilanove/Diecimila
     ** MISO - pin 12 on Arduino Uno/Duemilanove/Diecimila
     ** CLK - pin 13 on Arduino Uno/Duemilanove/Diecimila
     ** CS - depends on your SD card shield or module.
     ** Pin 4 used here for consistency with other Arduino examples
*/

#include "SD.h"
#include <data.hpp>

/*
uncoment below to enble debug mode
DEBUG disbles row 2 of the keypad {"4", "5", "6"}
this is that way, beacause pin 0 is used for Serial comunication and all other pins are in use
*/
//#define DEBUG

File myFile;

//not used at the moment
void setup_sd() {
#ifdef DEBUG
  Serial.print("Init SD card...");
#endif
  if (!SD.begin(4)) {
    Serial.println(1);
    while (1)
      ;
  }

#ifdef DEBUG
  Serial.println(0);
#endif
  // re-open the file for reading:
  load_user();
  save_user();
}
//not used at the moment
void load_user() {
  myFile = SD.open("test.txt");
  String data = "";
  if (myFile) {
    // read from the file until there's nothing else in it:
    while (myFile.available()){
      data = myFile.readString();
    }
    myFile.close();    
#ifdef DEBUG
    Serial.println(data);
#endif
    // close the file:
  } else {
    // if the file didn't open, print an error:
    
#ifdef DEBUG
    Serial.println(5);
#endif
  }
  //parse data to user array
  int pos = data.lastIndexOf("\n");
  int counter = 0;
  while (pos != -1) {//separates string in reverse order and pushes to args
    String user = data.substring(0,pos);

    int seperator = user.lastIndexOf(",");
    if(seperator<=0){
      break;
    }   
    users[counter].id=(String)(user.substring(0,seperator)); 
    users[counter].money=user.substring(seperator+1,user.lastIndexOf("\n")).toInt();
    
    data.remove(pos, pos + user.length());
    pos = data.lastIndexOf("\n");
    
#ifdef DEBUG
    Serial.println(users[counter].id);
    Serial.println(users[counter].money);
#endif
    counter++;
  }
}

void save_user(){
  String data = "";
  for(User user:users){
    if(user.id!=""){ 
      data += user.id+","+user.money+"\n";
    }
  }

#ifdef DEBUG
  Serial.println("{");
  Serial.print(data);
  Serial.println("}");
#endif
  // if the file opened okay, write to it:
 
  myFile = SD.open("test.txt", (O_READ | O_WRITE | O_CREAT));
  if (myFile) {
    myFile.print(data);
    // close the file:
    myFile.close();

#ifdef DEBUG
    Serial.println(0);
#endif
  } else {
    // if the file didn't open, print an error: 
#ifdef DEBUG
    Serial.println(4);
#endif
  }

}
/*
Notes:
0116646F302C,100
2F004461DFD5,0
*/
