
/*  Copyright (C) 2018  Martin Spiessl
 *  All rights reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


// do not define DEBUG to remove debug output
#define DEBUG
// do not define REGULAROUTPUT to suppress output to stdout also:
#define REGULAROUTPUT

// For verification, define VERIFY in next line:
//#define VERIFY

#ifdef VERIFY
  extern void __VERIFIER_error(void);
  extern int  __VERIFIER_nondet_int();
  extern void __VERIFIER_assume(int  ) ;
  #define VERIFIER_ERROR() __VERIFIER_error()
#endif
#ifndef VERIFY
  #define VERIFIER_ERROR() do {} while (0)
#endif


#ifdef DEBUG
#define debug_print(...) \
            fprintf(stderr, __VA_ARGS__)
#endif
#ifndef DEBUG
#define debug_print(...) do {} while (0)
#endif
#ifdef REGULAROUTPUT
#define regular_print(...) \
            fprintf(stdout, __VA_ARGS__)
#endif
#ifndef REGULAROUTPUT
#define regular_print(...) do {} while (0)
#endif

enum mode {Off, On};
enum geofencing {Near, In, Nowhere};
struct State {
    enum mode window;
    enum mode heating;
    enum mode door;
    enum mode light;
    enum mode coffeemachine;
    enum mode music;
    enum mode sun;
    enum geofencing geo;
};


#ifdef REGULAROUTPUT
// matching indices to enum values is dirty, do not do this:
const char * modeNames[] = {"Off\t", "On\t"};
const char * doorNames[] = {"Closed\t", "Open\t"};
const char * geofencingNames[] = {"Near house", "In house", "Nowhere near the house"};
#endif


// Automaton state variables:
struct State currentState;
struct State nextState;

int currentTemperature;

//###################### Automata Section  ######################

int windowSensor(){
    return currentState.window;
}

int temperatureSensor(){
    //0 cold, 1 not cold
    return rand() % 2;
}

void printStates() {
  regular_print("\nCurrent state of the smart home:\nWindow: %s\nHeating: %s\nDoor: %s\nLight: %s\nCoffee machine: %s\nMusic: %s\nDaylight: %s\nGeofencing: %s\n",
         doorNames[currentState.window],
         modeNames[currentState.heating],
         doorNames[currentState.door],
         modeNames[currentState.light],
         modeNames[currentState.coffeemachine],
         modeNames[currentState.music],
         modeNames[currentState.sun],
         geofencingNames[currentState.geo]);
}

void homeBeSmart(){
    currentTemperature = temperatureSensor();

    // G(f => -h)
    if(nextState.window == On && nextState.heating == On){
        printf("Turning off the heating.\n");
        nextState.heating = Off;
    }

    // G (-f & geo = 1 & -ts => h)
    if(nextState.window == Off && currentTemperature == 0 && nextState.geo == In && nextState.heating == Off){
        printf("Temperature sensor thinks it's cold and geofencing thinks there is someone in the house; turning the heating on.\n");
        nextState.heating = On;
    }

    //G (us & gs != 1 => -L)
    if(nextState.sun == On && (nextState.geo != In) && (nextState.light == On)){
        printf("No one in the house during the day - Lights off.");
        nextState.light = Off;
    }
    //G (-us & gs == 3 => -L)
    if(nextState.sun == Off &&(nextState.geo == Nowhere) && (nextState.light == On)){
        printf("No one is any where near the house - Lights off.");
        nextState.light = Off;
    }

    //G (-us & (gs = 1 | gs = 2) => L)
    if(nextState.sun == Off && nextState.light == On && nextState.geo != Nowhere){
        printf("There is someone around or in the house - Lights on.");
        nextState.light = On;
    }

    currentState = nextState;
}

void handleUserInput(char* input){
    struct State nextStateTemp;
    nextStateTemp.window = input[0] - '0';
    nextStateTemp.heating = input[2] - '0';
    nextStateTemp.door = input[4] - '0';
    nextStateTemp.light = input[6] - '0';
    nextStateTemp.coffeemachine = input[8] - '0';
    nextStateTemp.music = input[10] - '0';
    nextStateTemp.sun = input[12] - '0';
    nextStateTemp.geo = input[14] - '0';

    nextState = nextStateTemp;
}

void initializeHome(){
    struct State state;
    state.window = Off;
    state.heating = Off;
    state.door = Off;
    state.light = Off;
    state.coffeemachine = Off;
    state.music = Off;
    state.sun = Off;
    state.geo = Nowhere;
    currentState = state;
}

#ifndef VERIFY
bool validInput(char* input){
    printf(input);
    if(input[1] != '-' || input[3] != '-' || input[5] != '-' || input[7] != '-' || input[9] != '-' || input[11] != '-' || input[13] != '-'){
        return false;
    }

    if(input[14] != '0' && input[14] != '1' && input[14] != '2'){
        return false;
    }

    if((input[0] != '0' && input[0] != '1') || (input[2] != '0' && input[2] != '1') || (input[4] != '0' && input[4] != '1') ||
       (input[6] != '0' && input[6] != '1') || (input[8] != '0' && input[8] != '1') ||
       (input[10] != '0' && input[10] != '1') || (input[12] != '0' && input[12] != '1')){
        return false;
    }

    return true;
}

void getUserInput() {
  char input[16] = "+++++++++++++++";
  while (!validInput(input)) {
    regular_print("\nPlease enter a feature vector of the following form: \nwindow-heating-door-light-coffee-music-daylight-geofencing\nwhere all but geofencing have to be 0 or 1 (off/closed, on/open), geofencing has to be 0, 1, or 2 (near house, in house, not near house).");
    scanf("%15s",input);
    printf("\n");
  }
  //printf("Input war okay! :)");
  handleUserInput(input);
}
#endif
#ifdef VERIFY
void getUserInput() {
  switchState = __VERIFIER_nondet_int();
  __VERIFIER_assume(switchState==0 || switchState==1);
}
#endif
int main() {
    initializeHome();
  // check initial switching of the traffic lights:
  //if (!propertiesHold()) {
   //   VERIFIER_ERROR();
  //};
  // query input, step and check forever:
  for (;;) {
    printStates();
    getUserInput();
    homeBeSmart();
    //if (!propertiesHold()) {
      //VERIFIER_ERROR();
    //}
  }
  return 0;
}
