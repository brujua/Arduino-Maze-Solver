#include "States.h"
//#include "Decisions.h"

// PIN OUT
const int RIGHT_PIN = D0;
const int CENTER_PIN = D1;
const int LEFT_PIN = D2;
const int LEFT_WHEEL_PIN1 = D3;
const int LEFT_WHEEL_PIN2 = D4;
const int RIGHT_WHEEL_PIN1 = D7;
const int RIGHT_WHEEL_PIN2 = D8;


// CONSTANTS
const int GOAL_COUNTER_THRESHOLD = 2;
const int MOVE_TIME = 150;
const int CENTER_SEARCH_MOVE_LAPSE = 15;
const int SEARCH_ITERS = 12;
const int U_TURN_INITIAL_MOVE_LAPSE = 300;

const int READS_REMEMBERED = 100;
const int RIGHT_INDEX = 0;
const int LEFT_INDEX = 1;
const int CENTER_INDEX = 2;

// ROBOT MEMORY
enum state robot_state; 
int last_reads [READS_REMEMBERED][3]; 
int last_read_index = 0;
int goal_counter = 0;
unsigned long time_last_log = millis();
//enum decision posibilities [5]= {NO_OP, NO_OP, NO_OP, NO_OP, NO_OP};
//int decision_index = 0;
//bool take_turn = false;


void setup() {
  robot_state = STOPPED;
  pinMode(RIGHT_PIN, INPUT); //Sensor infrarrojo como entrada
  pinMode(CENTER_PIN, INPUT);
  pinMode(LEFT_PIN, INPUT);
  pinMode(LEFT_WHEEL_PIN1, OUTPUT);
  pinMode(LEFT_WHEEL_PIN2, OUTPUT);
  pinMode(RIGHT_WHEEL_PIN1, OUTPUT);
  pinMode(RIGHT_WHEEL_PIN2, OUTPUT); 
  Serial.begin(9600);
  delay(3000);
}

/*void loop(){
  delay(100);
  if(digitalRead(CENTER_PIN)){
    go_forward();
  }
}*/
void loop(){
  delay(50);
  update_knowledge();
  switch(robot_state){
    case STOPPED: {
      if(center_present()){
        go_forward();
      }else{
        turn_back();
      }
      break;
    }
    case GOING_FORWARD: {
      if(!center_present()){
        stop_motors();
        robot_state = STOPPED;
        find_center();
      }
      break;
    }    
    case GOAL_REACHED: {
      victory_dance();
      break;
    }
  }//end switch state
  check_goal(); 
} //end loop

void check_goal(){
  if(goal_counter >= GOAL_COUNTER_THRESHOLD){
    robot_state = GOAL_REACHED;
  }
}

void update_knowledge(){
  int right = digitalRead(RIGHT_PIN); 
  int center = digitalRead(CENTER_PIN);
  int left = digitalRead(LEFT_PIN);    
  log_state(right, center, left);
  // Update only if its different than last read.
  int * last_read = get_last_read();
  if(last_read[RIGHT_INDEX] != right || last_read[CENTER_INDEX] != center || last_read[LEFT_INDEX] != left){
    last_reads[last_read_index][RIGHT_INDEX] = right;
    last_reads[last_read_index][CENTER_INDEX] = center;
    last_reads[last_read_index][LEFT_INDEX] = left;
    last_read_index++;
    if(last_read_index > READS_REMEMBERED -1)
      last_read_index = 0;  
  }

  if(right && center && left){
    goal_counter++;
  } else{
    //goal_counter = 0;
  }
  
}

int* get_last_read(){
  if (last_read_index == 0)
    return last_reads[READS_REMEMBERED-1];
  else
    return last_reads[last_read_index-1];
}

bool center_present(){
  int * last_read = get_last_read();
  return last_read[CENTER_INDEX];
}

void go_forward(){
  robot_state = GOING_FORWARD;
  //Serial.println("Hacia delante!!!");  
  start_right_wheel_forward();
  start_left_wheel_forward();
  //delay(MOVE_TIME);
  //stop_motors();
}

bool find_center(){
  Serial.println("Buscando Centro...");
  int center = digitalRead(CENTER_PIN);
  
  for(int i=0; !center && i<SEARCH_ITERS; i+=2){
    rotate_right(CENTER_SEARCH_MOVE_LAPSE * (i+1));
    delay(100);
    center = digitalRead(CENTER_PIN);
    if(center){
      /*rotate_right(CENTER_SEARCH_MOVE_LAPSE*3); // move again to better re-center
      delay(100);
      center = digitalRead(CENTER_PIN);
      if(!center){
        //moved too much
        rotate_left(CENTER_SEARCH_MOVE_LAPSE*2);
      }*/
      break;
    }      
    rotate_left(CENTER_SEARCH_MOVE_LAPSE * (i+2));
    delay(100);
    center = digitalRead(CENTER_PIN);
    if(center){
      /*rotate_left(CENTER_SEARCH_MOVE_LAPSE*3); // move again to better re-center
      delay(100);
      center = digitalRead(CENTER_PIN);
      if(!center){
        //moved too much
        rotate_right(CENTER_SEARCH_MOVE_LAPSE*2);
      }*/
      break;
    }
  }
  if(center)
    return true;
  else
    return false;  
}

void turn_back(){
  //robot_state = TURNING_BACK;
  //take_turn = true;
  bool strip_found = false;
  rotate_left(U_TURN_INITIAL_MOVE_LAPSE);
  while (!strip_found){
    rotate_left(CENTER_SEARCH_MOVE_LAPSE*2);
    delay(100);
    strip_found = digitalRead(CENTER_PIN);
  } 
}

void rotate_left(int time_ms){
  start_left_wheel_backwards();
  start_right_wheel_forward();
  delay(time_ms);
  stop_motors();
}

void rotate_right(int time_ms){
  start_left_wheel_forward();
  start_right_wheel_backwards();
  delay(time_ms);
  stop_motors();
}

void start_right_wheel_forward(){
  digitalWrite (RIGHT_WHEEL_PIN1, LOW);
  digitalWrite (RIGHT_WHEEL_PIN2, HIGH);
}

void start_left_wheel_forward(){
  digitalWrite (LEFT_WHEEL_PIN2, LOW);
  digitalWrite (LEFT_WHEEL_PIN1, HIGH);
}

void start_right_wheel_backwards(){
  digitalWrite (RIGHT_WHEEL_PIN1, HIGH);
  digitalWrite (RIGHT_WHEEL_PIN2, LOW);
}

void start_left_wheel_backwards(){
  digitalWrite (LEFT_WHEEL_PIN2, HIGH);
  digitalWrite (LEFT_WHEEL_PIN1, LOW);
}

void stop_motors()
{
  digitalWrite (LEFT_WHEEL_PIN1, LOW);
  digitalWrite (LEFT_WHEEL_PIN2, LOW);
 
  digitalWrite (RIGHT_WHEEL_PIN1, LOW);
  digitalWrite (RIGHT_WHEEL_PIN2, LOW);
}

void victory_dance(){
  rotate_left(500);
  rotate_right(500);
  for(int i=0; i<10; i++){
    rotate_left(100);
    rotate_right(100); 
  }
  while(true){
    delay(100000);
  }
}

void log_state(int sensor_right, int sensor_center, int sensor_left){
  unsigned long now = millis();
  if( now - time_last_log > 1000){
    Serial.print("Robot State: ");
    if(robot_state == STOPPED)
      Serial.print("STOPPED");
    if(robot_state == GOING_FORWARD){
      Serial.print("GOING FORWARD");
    }
    Serial.println("izquierdo");
    Serial.println(sensor_right);
    Serial.println("Centro");
    Serial.println(sensor_center);
    Serial.println("Derecho");  
    Serial.println(sensor_left);
    time_last_log = now;
   }
}

void go_backwards(){
  // Right wheel motor
  digitalWrite (LEFT_WHEEL_PIN2, HIGH);
  digitalWrite (LEFT_WHEEL_PIN1, LOW);
  // Left wheel motor
  digitalWrite (RIGHT_WHEEL_PIN1, HIGH);
  digitalWrite (RIGHT_WHEEL_PIN2, LOW);

  delay(MOVE_TIME);
  stop_motors();
}
