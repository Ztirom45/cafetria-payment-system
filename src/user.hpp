#pragma once


#include <Arduino.h>
//User data:
//Warning cant store more than ten Users
typedef struct User {
  String id;
  int money;
  /*Sting name;*/
} User;


#define USER_ARRAY_SIZE 10

extern User users[USER_ARRAY_SIZE];
extern int USER_COUNT;
