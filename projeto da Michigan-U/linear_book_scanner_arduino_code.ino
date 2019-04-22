// Copyright 2014 Garrett Cullen, Kelsey Lindberg, Lauren Staszel, Teresa Tombelli
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


// Motor Driver Program for the Google Linear Book Scanner
// Written by University of Michigan Mechanical Engineering 450 team 19
// 4/23/2014

//Define Variables
const int Duration = 22000; //Duration is the number of steps (of size stepsize) to reach the other side of the scanner 
const int stepsize = 4; //Size of each step: 1 = full step, 2 = half step, 3 = 1/8 step, 4 = 1/16 step
const int Delay = 0; //wait between steps
const int StepDelay = 1; //Turn on each step for 1 mS
const int SensDist = 10500; //Number of steps to reach the page sensors               
const int SensTol = 250; //Tolerances in steps for detecting page errors
const int Rdist = 100; //Number of steps to reverse from sensors to reset the page   
const int IRval = 500; //The cutoff value for the IR sensor

//Define Pins
const int Dir = 5; //Direction pin is connected to pin 5
const int STEP = 6; //Step command pin is connected to pin 6
const int Sleep = 7; //Sleep pin is connected to pin 7
const int M0 = 8; //Step Size pin 0 is connected to pin 8
const int M1 = 9; //Step Size pin 1 is connected to pin 9 
const int LS1 = 10; //Scanner side limit switch is connected to pin 10
const int LS2 = 11; //Output side limit switch is connected to pin 11
const int LED = 2; //LED output for testing
const int IR = A2; //Infrared sensor is connected to analogue pin 2
const int Scanner = 3; //The scan bars are activated by pin 3

void setup () {
  
  //Define Pins
  pinMode(Dir, OUTPUT);
  pinMode(STEP, OUTPUT);
  pinMode(Sleep, OUTPUT);
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT); 
  pinMode(LS1, INPUT);
  pinMode(LS2, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(IR, INPUT); 

  //Disable Sleep mode
  digitalWrite(Sleep, HIGH);

  digitalWrite(LED, HIGH); //Blink LED
  delay(1000);
  digitalWrite(LED, LOW);

  //Reverse to reset the scanner location
  driver(stepsize, 1, Duration);
}

void loop () {

  //Activate the Scan bars
  digitalWrite(Scanner, HIGH);
  delay(1);
  digitalWrite(Scanner, LOW);
  
  //Drive the motor forward for the specified duration
  driver(stepsize, 0, Duration); 
  digitalWrite(LED, HIGH); //Blink LED
  delay(1);
  digitalWrite(LED, LOW);
  
  //Drive the motor backward for the specified duration
  driver(stepsize, 1, Duration);
  digitalWrite(LED, HIGH); //Blink LED
  delay(1);
  digitalWrite(LED, LOW);
}

void driver( int stepsize, int Direction, int Duration) {

  if (stepsize == 2) {
    digitalWrite(M0, HIGH);
    digitalWrite(M1, LOW);
  }
  else if (stepsize == 3) {
    digitalWrite(M0, LOW);
    digitalWrite(M1, HIGH);
  }
  else if (stepsize == 4) {
    digitalWrite(M0, HIGH);
    digitalWrite(M1, HIGH);
  }
  else { //(stepsize == 1)
    digitalWrite(M0, LOW);
    digitalWrite(M1, LOW);
  }
  
  //Set the Direction
  if (Direction == 1) {  //Forwards
    digitalWrite(Dir, LOW);
  }
  else {                 //Backwards
    digitalWrite(Dir, HIGH);
  }
  //Start going forward to get clear of the limit switch
  for(int n = 0; n < 5; n++){
    if((Direction == 0 & digitalRead(LS2) == LOW) | (Direction == 1 & digitalRead(LS1) == LOW)) {      
      //step forward one incriment
      digitalWrite(STEP, HIGH); //turn on the motor
      delay(StepDelay); //run for the step duration
      digitalWrite(STEP, LOW); //turn off the motor
      delay(Delay); //wait between steps to slow down the motor
    }
  }
  //Drive in Direction for Duration with steps of stepsize
  for (int n = 5; n < Duration; n++){
    if (digitalRead(LS1) == LOW & digitalRead(LS2) == LOW) { //If the saddle is not at the end of the scanner
      //Check for page errors
      if (n > (SensDist-SensTol) & n < (SensDist + SensTol) & Direction == 0){         
        digitalWrite(LED, HIGH); //Blink LED 
        if (analogRead(IR) < IRval ) {  //Check sensors for errors
          driver( stepsize, 1, Rdist); //Reverse to correct page errors
        }
      }
      else{
          digitalWrite(LED, LOW); //LED off
      }
      //step forward one incriment
      digitalWrite(STEP, HIGH); //turn on the motor
      delay(StepDelay); //run for the step duration
      digitalWrite(STEP, LOW); //turn off the motor
      delay(Delay); //wait between steps to slow down the motor
    } 
    else {
      //Stop Driving
      break;
    }
  }
}
