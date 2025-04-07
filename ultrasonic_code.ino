#include <Servo.h>

const int servo_pin = 10;
Servo scanner_servo;

const int sensor_pins[4][2] = { {2, 3}, {4, 5}, {6, 7}, {8, 9}};

float previous_estimates[4] = {0, 0, 0, 0};
float error_estimates[4] = {1, 1, 1, 1};
float measurement_noise = 2.0;


const int step_size = 15;
const int settle_delay = 300;


float kalman_filter(float measurement, float &prev_estimate, float &error_estimate) {

  float kalman_gain = error_estimate / (error_estimate + measurement_noise);

  float current_estimate = prev_estimate + kalman_gain * (measurement - prev_estimate);
  error_estimate = (1 - kalman_gain) * error_estimate;
  prev_estimate = current_estimate;
  return current_estimate;
}


float get_distance(int trig_pin, int echo_pin)
{
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);

  long duration = pulseIn(echo_pin, HIGH, 20000);  
  float distance = duration * 0.0343 / 2.0;        

  if (duration == 0 || distance > 400) return -1;   
  return distance;
}

void setup()
{
  Serial.begin(9600);

  for (int i = 0; i < 4; i++) {
    pinMode(sensor_pins[i][0], INPUT);   
    pinMode(sensor_pins[i][1], OUTPUT);  
  }

  scanner_servo.attach(servo_pin);
  scanner_servo.write(0);  
  delay(500);
}

void loop() {
  
  for (int angle = 0; angle <= 180; angle += step_size) {
    scanner_servo.write(angle);
    delay(settle_delay);

    for (int i = 0; i < 4; i++) {
      float raw_dist = get_distance(sensor_pins[i][1], sensor_pins[i][0]);
      unsigned long timestamp = millis();

      if (raw_dist > 0) {
        float filtered_dist = kalman_filter(raw_dist, previous_estimates[i], error_estimates[i]);

        Serial.print(angle);
        Serial.print(",");
        Serial.print(i + 1);  
        Serial.print(",");
        Serial.print(filtered_dist, 2);
        Serial.print(",");
        Serial.println(timestamp);
      }
    }
  }

  
  for (int angle = 180; angle >= 0; angle -= step_size) {
    scanner_servo.write(angle);
    delay(settle_delay);

    for (int i = 0; i < 4; i++) {
      float raw_dist = get_distance(sensor_pins[i][1], sensor_pins[i][0]);
      unsigned long timestamp = millis();

      if (raw_dist > 0) {
        float filtered_dist = kalman_filter(raw_dist, previous_estimates[i], error_estimates[i]);

        Serial.print(angle);
        Serial.print(",");
        Serial.print(i + 1);  
        Serial.print(",");
        Serial.print(filtered_dist, 2);
        Serial.print(",");
        Serial.println(timestamp);
      }
    }
  }
}
