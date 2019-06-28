enum state {
  STOPPED,
  GOING_FORWARD,
  RECENTERING,
  TURNING_BACK,
  GOAL_REACHED
};
//#include "Decisions.h"

// PIN OUT
const int RIGHT_PIN = D2;
const int CENTER_PIN = D1;
const int LEFT_PIN = D0;
const int LEFT_WHEEL_PIN1 = D3;
const int LEFT_WHEEL_PIN2 = D4;
const int RIGHT_WHEEL_PIN1 = D7;
const int RIGHT_WHEEL_PIN2 = D8;
const int GREEN_PIN = D5;
const int BLUE_PIN = D6;


// CONSTANTS
const int GOAL_COUNTER_THRESHOLD = 5;
const int MOVE_TIME = 50;
const int CENTER_SEARCH_MOVE_LAPSE = 15;
const int SEARCH_ITERS = 6;
const int U_TURN_INITIAL_MOVE_LAPSE = 250;
const int TURN_INIT_LAPSE = 250;
const int REFINEMENT_LAPSE = 25;
const int MAX_REFINEMENT_FACTOR = 5;
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
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT); 
  Serial.begin(9600);
  delay(5000);
}

/*void loop(){
  delay(100);
  if(digitalRead(CENTER_PIN)){
    go_forward();
  }
}*/
void loop(){
  delay(30);
  turn_both_leds_off();
  if(robot_state == GOAL_REACHED){
    victory_dance();
  }
  if(left_present()){
    //blue_led_on();
    turn_left();
  } else {
    if(center_present()){
      green_led_on();
      go_forward();
    } else{
      if(right_present()){
        //blue_led_on();
        turn_right();
      } else{ //no left, center nor right
        bool recentered = find_center();
        if (!recentered){
          turn_back();
        }  
      }
    }  
  }
} //end loop

void check_goal(){
  if(goal_counter >= GOAL_COUNTER_THRESHOLD){
    robot_state = GOAL_REACHED;
  }
}

void turn_left(){
  bool right_was_present = right_present();
  bool confirmed_left = left_present();
  if(!confirmed_left){
    return;
  }
  start_right_wheel_forward();
  delay(TURN_INIT_LAPSE);
  stop_motors();
  delay(100);
  bool right_still_present = digitalRead(RIGHT_PIN);
  if(right_was_present && right_still_present){
    //return to previous position
     start_left_wheel_forward();
     delay(TURN_INIT_LAPSE);
     stop_motors();
     robot_state = GOAL_REACHED;
     return;
  }
  bool center = digitalRead(CENTER_PIN);
  while(!center){
    rotate_left(CENTER_SEARCH_MOVE_LAPSE*2);
    stop_motors();
    delay(100);
    center = digitalRead(CENTER_PIN);
  }
  //refine_center(true);
}

void turn_right(){
  bool left_was_present = left_present();
  bool confirmed_right = right_present();
  if(!confirmed_right){
    return;
  }
  start_left_wheel_forward();
  delay(TURN_INIT_LAPSE);
  stop_motors();
  delay(100);
  bool left_still_present = digitalRead(LEFT_PIN);
  if(left_was_present && left_still_present){
    //return to previous position
     start_right_wheel_forward();
     delay(TURN_INIT_LAPSE);
     stop_motors();
     robot_state = GOAL_REACHED;
     return;
  }
  bool center = digitalRead(CENTER_PIN);
  while(!center){
    rotate_right(CENTER_SEARCH_MOVE_LAPSE*2);
    stop_motors();
    delay(100);
    center = digitalRead(CENTER_PIN);
  }
  //refine_center(false);
}


void go_forward(){
  //Serial.println("Hacia delante!!!");  
  start_right_wheel_forward();
  start_left_wheel_forward();
  delay(MOVE_TIME);
  stop_motors();
}

bool find_center(){
  Serial.println("Buscando Centro...");
  int center = digitalRead(CENTER_PIN);
  bool found_from_left = true;
  for(int i=0; !center && i<SEARCH_ITERS; i+=2){
    int max_rotation = CENTER_SEARCH_MOVE_LAPSE * (i+1);
    for(int rotation=CENTER_SEARCH_MOVE_LAPSE; !center && rotation<max_rotation; rotation+=CENTER_SEARCH_MOVE_LAPSE){
      rotate_right(rotation);
      delay(60);
      center = digitalRead(CENTER_PIN);
      if(center){
        found_from_left = false;
        break;
      }  
    }
    if(!center){
      int max_rotation =  (i < 5)? CENTER_SEARCH_MOVE_LAPSE * (i+2) : CENTER_SEARCH_MOVE_LAPSE * (i+5);  //compensation for difference in response of the motors
      for(int rotation=CENTER_SEARCH_MOVE_LAPSE; !center && rotation<max_rotation; rotation+=CENTER_SEARCH_MOVE_LAPSE){
        rotate_left(rotation);
        delay(60);
        center = digitalRead(CENTER_PIN);
        if(center){
          found_from_left = true;
          break;
        }
      }  
    }
  }
  if(center){
    refine_center(found_from_left);
    return true;
  }else{
    // return to original position
    //rotate_right(CENTER_SEARCH_MOVE_LAPSE * (SEARCH_ITERS));
    return false;  
  }
}

void turn_back(){
  //robot_state = TURNING_BACK;
  //take_turn = true;
  bool strip_found = false;
  rotate_right(U_TURN_INITIAL_MOVE_LAPSE);
  while (!strip_found){
    rotate_right(CENTER_SEARCH_MOVE_LAPSE*2);
    delay(100);
    strip_found = digitalRead(CENTER_PIN);
  }
  refine_center(true); 
}

void refine_center(bool from_left){
  int refinement_factor  = 1;
  if(!center_present()){
    return;  
  }
  blue_led_on();
  //delay(1000); // TODO remove
  _refine_center(from_left, REFINEMENT_LAPSE);
  delay(100);
  while(!center_present()){
    _refine_center(!from_left, REFINEMENT_LAPSE/refinement_factor);
    delay(100);
    if(center_present())
      break;
    refinement_factor++;
    _refine_center(!from_left, REFINEMENT_LAPSE/refinement_factor);
    delay(100); 
    if(refinement_factor > MAX_REFINEMENT_FACTOR){
      find_center();  
    }
  }  
  blue_led_off();
}

void _refine_center(bool from_left, int time_ms){
  if(from_left){
    rotate_left(time_ms);
  } else {
    rotate_right(time_ms);
  }
}

void victory_dance(){
  go_forward_for(MOVE_TIME*5);
  //rotate_left(500);
  //rotate_right(500);
  for(int i=0; i<20; i++){
    green_led_on();
    rotate_left(120);
    green_led_off();
    blue_led_on();
    rotate_right(120);
    blue_led_off(); 
  }
  while(true){
    green_led_on();
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

///////////////////////////////////////////
/////////   Long term MEMORY   ///////////

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
    if(last_read_index > READS_REMEMBERED -1){
      last_read_index = 0;  
    }   
  }
  if(right && center && left){
    goal_counter++;
  }
}

int* get_last_read(){
  if (last_read_index == 0)
    return last_reads[READS_REMEMBERED-1];
  else
    return last_reads[last_read_index-1];
}


///////////////////////////////////////////
////////      IR  SENSORS      ///////////
bool center_present(){
  return digitalRead(CENTER_PIN);
}

bool right_present(){
  return digitalRead(RIGHT_PIN);
}

bool left_present(){
  return digitalRead(LEFT_PIN);
}

///////////////////////////////////////////
////////     WHEELS / MOTORS    ///////////

void go_forward_for(int time_ms){
  start_right_wheel_forward();
  start_left_wheel_forward();
  delay(time_ms);
  stop_motors();
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
///////////////////////////////////////////
////////         LEDS          ///////////

void green_led_on(){
  digitalWrite(GREEN_PIN, HIGH);
}

void green_led_off(){
  digitalWrite(GREEN_PIN, LOW);
}

void blue_led_on(){
  digitalWrite(BLUE_PIN, HIGH);
}

void blue_led_off(){
  digitalWrite(BLUE_PIN, LOW);
}

void turn_both_leds_on(){
  green_led_on();
  blue_led_on();
}

void turn_both_leds_off(){
  green_led_off();
  blue_led_off();
}
