
int frwd_counter = 0;
const int MOVE_TIME = 250;
const int CENTER_SEARCH_MOVE_LAPSE = 50;
const int SEARCH_ITERS = 15;
void setup() {
  pinMode(D0, INPUT); //Sensor infrarrojo como entrada
  pinMode(D1, INPUT);
  pinMode(D2, INPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);
  
  Serial.begin(9600);
}

void loop() {
  //delay(100);
  //frwd_counter++; 
  int right_value = digitalRead(D0); //leemos el valor del sensor infrarrojo
  int center_value = digitalRead(D1);
  int left_value = digitalRead(D2);    
  print_sensors(right_value, center_value, left_value);
  if (center_value) {
    go_forward();
  } else {
    bool center_found = find_center();
    if(!center_found)
      delay(10000); 
  }
}

void print_sensors(int sensor_right, int sensor_center, int sensor_left){
  Serial.println("izquierdo");
  Serial.println(sensor_right);
  Serial.println("Centro");
  Serial.println(sensor_center);
  Serial.println("Derecho");  
  Serial.println(sensor_left);
  
}

void go_forward(){
  Serial.println("Hacia delante!!!");  
  // Right wheel motor
  digitalWrite (D5, LOW);
  digitalWrite (D6, HIGH);
  // Left wheel motor
  digitalWrite (D7, LOW);
  digitalWrite (D8, HIGH);

  delay(MOVE_TIME);
  stop_motors();
}

bool find_center(){
  int center = digitalRead(D1);
  Serial.println("BUSCANDO HACIA LA DERECHA");
  //arc to the right in search for the center strip
  for(int i=0; !center && i<SEARCH_ITERS; i++){
    move_right(CENTER_SEARCH_MOVE_LAPSE);
    delay(100);
    center = digitalRead(D1);
  }
  if (center)
    return true;
  delay(3000);
  
  Serial.println("BUSCANDO HACIA LA IZQUIERDA");
  //arc to the left in search for the center strip, have to account for the previous move to the right
  int search_iters = SEARCH_ITERS * 3;
  for(int i=0; !center && i<search_iters; i++){
    move_left(CENTER_SEARCH_MOVE_LAPSE);
    delay(100);
    center = digitalRead(D1); 
  }
  if(center)
    return true;
  else
    return false;  
}

void go_backwards(){
  // Right wheel motor
  digitalWrite (D5, LOW);
  digitalWrite (D6, HIGH);
  // Left wheel motor
  digitalWrite (D7, LOW);
  digitalWrite (D8, HIGH);

  delay(1000);
  stop_motors();
}

void move_left(int time_ms){
  // move Right wheel motor to move left
  digitalWrite (D5, LOW);
  digitalWrite (D6, HIGH);
  delay(time_ms);
  stop_motors();
}

void move_right(int time_ms){
  // move left wheel motor to move right
  digitalWrite (D7, LOW);
  digitalWrite (D8, HIGH);
  delay(time_ms);
  stop_motors();
}


void stop_motors()
{
  digitalWrite (D5, LOW);
  digitalWrite (D6, LOW);
  
  digitalWrite (D7, LOW);
  digitalWrite (D8, LOW);
}
