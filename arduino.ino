// Code for Arduino Uno 

// INFO Step motor setup
#define STEPPER_PIN_1 9
#define STEPPER_PIN_2 10
#define STEPPER_PIN_3 11
#define STEPPER_PIN_4 12
int step_number = 0;

// INFO Ultrasonic sensor setup
#define ULT_TRIG_PIN 7
#define ULT_ECHO_PIN 6
int distance = 0;
bool blocked = false;
bool objLeft = false;

// INFO Gas sensor setup
#define GAS_SENSOR_PIN A0
#define GAS_LED_PIN 8
int gas_sensor_value = 0;
bool gas_sensor_value_changed = false;

// INFO Load cell setup
#include <HX711_ADC.h>
#define HX711_DOUT 4
#define HX711_SCK 5
#define MASS_REACHED_LED_PIN A4
unsigned long HX711_T = 0;
int MAX_MASS = 500;
bool MASS_REACHED = false;
int load_cell_value;
HX711_ADC LoadCell(HX711_DOUT, HX711_SCK);

// INFO Humidity sensor setup
#include <dht11.h>
#define DHT11PIN 3
bool DHT11_SENT = false;
dht11 DHT11;

// INFO dark sensor setup
#define DARK_SENSOR_PIN A1
#define DARK_LED_PIN 2

// INFO IR sensor setup
#define IR_SENSOR_PIN 13
#define BUZZER_PIN A2

void setup(){
    Serial.begin (9600);

    pinMode(ULT_TRIG_PIN, OUTPUT);
    pinMode(ULT_ECHO_PIN, INPUT);

    pinMode(STEPPER_PIN_1, OUTPUT);
    pinMode(STEPPER_PIN_2, OUTPUT);
    pinMode(STEPPER_PIN_3, OUTPUT);
    pinMode(STEPPER_PIN_4, OUTPUT);

    pinMode(GAS_SENSOR_PIN, INPUT);
    pinMode(GAS_LED_PIN, OUTPUT);

    pinMode(MASS_REACHED_LED_PIN, OUTPUT);
    LoadCell.begin();
    LoadCell.start(2000,true);
    LoadCell.setCalFactor(696.0);

    pinMode(DHT11PIN, INPUT);

    pinMode(DARK_SENSOR_PIN, INPUT);
    pinMode(DARK_LED_PIN, OUTPUT);

    pinMode(IR_SENSOR_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
}

void loop(){
    // Ulltrasonic sensor + motor
    objLeft = objLeftSensor();
    if(objLeft){
        // Object detected
        for(int i = 0; i < 2500; i++){
          OneStep(true);
          delay(3);
        }
    }
    
    // Gas sensor
    gas_sensor_value=analogRead(GAS_SENSOR_PIN);
    if(gas_sensor_value > 500 ){
      digitalWrite(GAS_LED_PIN,HIGH);   
      if(!gas_sensor_value_changed){
        Serial.print("Gas detected");
        gas_sensor_value_changed = true;
      }
    }else{
      digitalWrite(GAS_LED_PIN,LOW);    
    }

    // Load Cell sensor
    static boolean newDataReady = 0;
    const int serialPrintInterval = 0;
    if (LoadCell.update()) newDataReady = true;
    if (newDataReady) {
      if (millis() > HX711_T + serialPrintInterval) {
        float i = LoadCell.getData();
        // Serial.print("Load_cell output val: ");
        // Serial.println(i);
        if (i>MAX_MASS && !MASS_REACHED){
          MASS_REACHED = true;
          Serial.print("Mass exceeded");
          digitalWrite(MASS_REACHED_LED_PIN,HIGH);
        }
        newDataReady = 0;
        HX711_T = millis();
      }
    }

    // Humidity sensor
    int chk = DHT11.read(DHT11PIN);
    if(DHT11.humidity >80 && !DHT11_SENT){
      // Serial.println((float)DHT11.humidity, 2);
      Serial.println("Humidity too high");
      DHT11_SENT = true;
    }
  
    // Dark sensor
    int dark_sensor_value = analogRead(DARK_SENSOR_PIN);
    if(dark_sensor_value <100){
      digitalWrite(DARK_LED_PIN,HIGH);
    }else{
      digitalWrite(DARK_LED_PIN,LOW);
    }

    // IR sensor
    if(digitalRead(IR_SENSOR_PIN) == HIGH){
      digitalWrite(BUZZER_PIN, HIGH);
    }else{
      digitalWrite(BUZZER_PIN, LOW);
    }

  // Reset Btn
  if(digitalRead(A3) == HIGH){
    refresh_all_value();
  }
}

void OneStep(bool dir){
    if(dir){
switch(step_number){
  case 0:
  digitalWrite(STEPPER_PIN_1, HIGH);
  digitalWrite(STEPPER_PIN_2, LOW);
  digitalWrite(STEPPER_PIN_3, LOW);
  digitalWrite(STEPPER_PIN_4, LOW);
  break;
  case 1:
  digitalWrite(STEPPER_PIN_1, LOW);
  digitalWrite(STEPPER_PIN_2, HIGH);
  digitalWrite(STEPPER_PIN_3, LOW);
  digitalWrite(STEPPER_PIN_4, LOW);
  break;
  case 2:
  digitalWrite(STEPPER_PIN_1, LOW);
  digitalWrite(STEPPER_PIN_2, LOW);
  digitalWrite(STEPPER_PIN_3, HIGH);
  digitalWrite(STEPPER_PIN_4, LOW);
  break;
  case 3:
  digitalWrite(STEPPER_PIN_1, LOW);
  digitalWrite(STEPPER_PIN_2, LOW);
  digitalWrite(STEPPER_PIN_3, LOW);
  digitalWrite(STEPPER_PIN_4, HIGH);
  break;
} 
  }else{
    switch(step_number){
  case 0:
  digitalWrite(STEPPER_PIN_1, LOW);
  digitalWrite(STEPPER_PIN_2, LOW);
  digitalWrite(STEPPER_PIN_3, LOW);
  digitalWrite(STEPPER_PIN_4, HIGH);
  break;
  case 1:
  digitalWrite(STEPPER_PIN_1, LOW);
  digitalWrite(STEPPER_PIN_2, LOW);
  digitalWrite(STEPPER_PIN_3, HIGH);
  digitalWrite(STEPPER_PIN_4, LOW);
  break;
  case 2:
  digitalWrite(STEPPER_PIN_1, LOW);
  digitalWrite(STEPPER_PIN_2, HIGH);
  digitalWrite(STEPPER_PIN_3, LOW);
  digitalWrite(STEPPER_PIN_4, LOW);
  break;
  case 3:
  digitalWrite(STEPPER_PIN_1, HIGH);
  digitalWrite(STEPPER_PIN_2, LOW);
  digitalWrite(STEPPER_PIN_3, LOW);
  digitalWrite(STEPPER_PIN_4, LOW);
 
  
} 
  }
step_number++;
  if(step_number > 3){
    step_number = 0;
  }
}

bool objLeftSensor(){
    int distance;
    long duration;
    digitalWrite(ULT_TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(ULT_TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(ULT_TRIG_PIN, LOW);
    duration = pulseIn(ULT_ECHO_PIN, HIGH);
    distance = duration*0.034/2;
    // Serial.print("Distance: ");
    // Serial.println(distance);
    if(distance > 10 && blocked){
        blocked = false;
        return true;
    }
    if(distance < 10 && !blocked){
        blocked = true;
        return false;
    }
    return false;
}

void refresh_all_value(){
  gas_sensor_value_changed = false;
  MASS_REACHED = false;
  DHT11_SENT = false;
  digitalWrite(MASS_REACHED_LED_PIN,LOW);

  Serial.print("Refreshed");
}