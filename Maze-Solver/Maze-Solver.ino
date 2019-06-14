
int frwd_counter = 0;

void setup() {
  pinMode(D0  , INPUT) ; //Sensor infrarrojo como entrada
  pinMode( D1 , INPUT) ;
  pinMode( D2, INPUT) ;
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);
  
  Serial.begin(9600);
}

void loop() {
  delay(1000);
  frwd_counter++; 
  int valor1 = digitalRead(D0) ; //leemos el valor del sensor infrarrojo
  int valor2 = digitalRead(D1) ;
  int valor3 = digitalRead(D2) ;
  Serial.println("counter:");
  Serial.println(frwd_counter);    
  print_sensors(valor1, valor2, valor3);
  if(frwd_counter >= 10){
    frwd_counter = 0;
    go_forward();
  }

  
  /*MotorHorario();
  Serial.println("Giro del Motor en sentido horario");
  delay(1000);
  
  MotorAntihorario();
  Serial.println("Giro del Motor en sentido antihorario");
  delay(5000);
  
  MotorStop();
  Serial.println("Motor Detenido");
  delay(5000);*/
  
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

  delay(700);
  motors_stop();
}

void go_backwards(){
  // Right wheel motor
  digitalWrite (D5, LOW);
  digitalWrite (D6, HIGH);
  // Left wheel motor
  digitalWrite (D7, LOW);
  digitalWrite (D8, HIGH);

  delay(1000);
  motors_stop();
}


void motors_stop()
{
  digitalWrite (D5, LOW);
  digitalWrite (D6, LOW);
  
  digitalWrite (D7, LOW);
  digitalWrite (D8, LOW);
}
