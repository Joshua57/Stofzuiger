int val = 0;  
int sensorVal47 = 0;//bumper links
int sensorVal49 = 0;//bumper linksvoor
int sensorVal51 = 0;//bumper rechtsvoor
int sensorVal53 = 0;//bumper rechts

void setup() {
  //start serial connection
  Serial.begin(9600);
  //configure pin2 as an input and enable the internal pull-up resistor
  pinMode(47, INPUT_PULLUP);
  pinMode(49, INPUT_PULLUP);
  pinMode(51, INPUT_PULLUP);
  pinMode(53, INPUT_PULLUP);
  pinMode(13, OUTPUT);
}

void loop() {
  //bumper rechts
sensorVal47 = digitalRead(47);
sensorVal49 = digitalRead(49);
sensorVal51 = digitalRead(51);
sensorVal53 = digitalRead(53);

//val = analogRead(sensorpin);       // reads the value of the sharp sensor

//print out the value of the pushbutton
//  Serial.println(sensorVal);

  // Keep in mind the pullup means the pushbutton's
  // logic is inverted. It goes HIGH when it's open,
  // and LOW when it's pressed. Turn on pin 13 when the
  // button's pressed, and off when it's not:

  if (sensorVal47 == HIGH)
  {digitalWrite(13, LOW);}
  else
  {digitalWrite(13, HIGH);}

  if (sensorVal49 == HIGH)
  {digitalWrite(13, LOW);}
  else
  {digitalWrite(13, HIGH);}
  
  if (sensorVal51 == HIGH)
  {digitalWrite(13, LOW);}
  else
  {digitalWrite(13, HIGH);}
  
  if (sensorVal53 == HIGH)
  {digitalWrite(13, LOW);}
  else
  {digitalWrite(13, HIGH);}
/*  
  if (sensorVal5 == HIGH)
  {digitalWrite(13, LOW);}
  else
  {digitalWrite(13, HIGH);}
*/  
//  Serial.println(val);            // prints the value of the sensor to the serial monitor
//   delay(100);    
}
