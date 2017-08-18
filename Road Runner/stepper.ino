  #include "Arduino.h"
  
  
  
  
  
  
  
  class myStepper {
    public:
    
  
      myStepper(int number_of_steps, int motor_pin_1, int motor_pin_2,
                                     int motor_pin_3, int motor_pin_4,
                                     int motor_pin_6, int motor_pin_7,
                                     int motor_pin_8, int motor_pin_9,
                                     int motor_pin_10, int motor_pin_11,
                                     int motor_pin_12, int motor_pin_13,
                                     int motor_pin_14, int motor_pin_15,
                                     int motor_pin_16, int motor_pin_17);
  
      // speed setter method:
      void mysetSpeed(long whatSpeed);
  
      // mover method:
      void mystep(int number_of_steps);
  
      int myversion(void);
  
      void move_forword(int number_of_steps);
  
      void move_left(int number_of_steps);
  
    private:
      void stepMotor(int this_step);
      void stepMotorForword( int this_step);
      void stepMotorLeft(int thisStep);
      void myStepper :: magneto_move_forword (int steps_to_move);
  
      int mydirection;            // Direction of rotation
      unsigned long step_delay; // delay between steps, in ms, based on speed
      int number_of_steps;      // total number of steps this motor can take
      int pin_count;            // how many pins are in use.
      int step_number;          // which mystep the motor is on
  
      // motor pin numbers:
      int motor_pin_1;
      int motor_pin_2;
      int motor_pin_3;
      int motor_pin_4;
      int motor_pin_5;          // Only 5 phase motor
      int motor_pin_6;
      int motor_pin_7;
      int motor_pin_8;
      int motor_pin_9;
      int motor_pin_10;
      int motor_pin_11;
      int motor_pin_12;
      int motor_pin_13;
      int motor_pin_14;
      int motor_pin_15;
      int motor_pin_16;
      int motor_pin_17;
      unsigned long last_step_time; // time stamp in us of when the last mystep was taken
  };
  
  
  myStepper :: myStepper(int number_of_steps, int motor_pin_1, int motor_pin_2,
                                     int motor_pin_3, int motor_pin_4,
                                     int motor_pin_6, int motor_pin_7,
                                     int motor_pin_8, int motor_pin_9,
                                     int motor_pin_10, int motor_pin_11,
                                     int motor_pin_12, int motor_pin_13,
                                     int motor_pin_14, int motor_pin_15,
                                     int motor_pin_16, int motor_pin_17)
  {
    this->step_number = 0;    // which mystep the motor is on
    this->mydirection = 0;    // motor mydirection
    this->last_step_time = 0; // time stamp in us of the last mystep taken
    this->number_of_steps = number_of_steps; // total number of steps for this motor
  
    // Arduino pins for the motor control connection:
    this->motor_pin_1 = motor_pin_1;
    this->motor_pin_2 = motor_pin_2;
    this->motor_pin_3 = motor_pin_3;
    this->motor_pin_4 = motor_pin_4;
    this->motor_pin_6 = motor_pin_6;
    this->motor_pin_7 = motor_pin_7;
    this->motor_pin_8 = motor_pin_8;
    this->motor_pin_9 = motor_pin_9;
    this->motor_pin_10 = motor_pin_10;
    this->motor_pin_11 = motor_pin_11;
    this->motor_pin_12 = motor_pin_12;
    this->motor_pin_13 = motor_pin_13;
    this->motor_pin_14 = motor_pin_14;
    this->motor_pin_15 = motor_pin_15;
    this->motor_pin_16 = motor_pin_16;
    this->motor_pin_17 = motor_pin_17;
  
    // setup the pins on the microcontroller:
    pinMode(this->motor_pin_1, OUTPUT);
    pinMode(this->motor_pin_2, OUTPUT);
    pinMode(this->motor_pin_3, OUTPUT);
    pinMode(this->motor_pin_4, OUTPUT);
    pinMode(this->motor_pin_6, OUTPUT);
    pinMode(this->motor_pin_7, OUTPUT);
    pinMode(this->motor_pin_8, OUTPUT);
    pinMode(this->motor_pin_9, OUTPUT);
    pinMode(this->motor_pin_10, OUTPUT);
    pinMode(this->motor_pin_11, OUTPUT);
    pinMode(this->motor_pin_12, OUTPUT);
    pinMode(this->motor_pin_13, OUTPUT);
    pinMode(this->motor_pin_14, OUTPUT);
    pinMode(this->motor_pin_15, OUTPUT);
    pinMode(this->motor_pin_16, OUTPUT);
    pinMode(this->motor_pin_17, OUTPUT);
    // pin_count is used by the stepMotor() method:
    this->pin_count = 16;
  }
  
  /*
   * Sets the speed in revs per minute
   */
  void myStepper::mysetSpeed(long whatSpeed)
  {
    this->step_delay = 60L * 1000L * 1000L / this->number_of_steps / whatSpeed;
  }
  
  /*
   * Moves the motor steps_to_move steps.  If the number is negative,
   * the motor moves in the reverse mydirection.
   */
  
  void myStepper :: move_forword (int steps_to_move)
  {
    int steps_left = abs(steps_to_move);  // how many steps to take
  
    // determine mydirection based on whether steps_to_mode is + or -:
    if (steps_to_move > 0) { this->mydirection = 1; }
    if (steps_to_move < 0) { this->mydirection = 0; }
  
  
    // decrement the number of steps, moving one mystep each time:
    while (steps_left > 0)
    {
      unsigned long now = micros();
      
      // move only if the appropriate delay has passed:
      if (now - this->last_step_time >= this->step_delay)
      {
        // get the timeStamp of when you stepped:
        this->last_step_time = now;
        // increment or decrement the mystep number,
        // depending on mydirection:
        if (this->mydirection == 1)
        {
          this->step_number++;
          if (this->step_number == this->number_of_steps) {
            this->step_number = 0;
          }
        }
        else
        {
          if (this->step_number == 0) {
            this->step_number = this->number_of_steps;
          }
          this->step_number--;
        }
        // decrement the steps left:
        steps_left--;
        // mystep the motor to mystep number 0, 1, ..., {3 or 10}
        stepMotorForword(this->step_number % 4);
  
      }
    }
  }
  
  void myStepper :: stepMotorForword(int thisStep)
  {
    switch (thisStep) {
        case 0:  // 1010
          digitalWrite(motor_pin_1, HIGH);
          digitalWrite(motor_pin_6, HIGH);
          digitalWrite(motor_pin_10, HIGH);
          digitalWrite(motor_pin_14, HIGH);
  
          digitalWrite(motor_pin_2, LOW);
          digitalWrite(motor_pin_7, LOW);
          digitalWrite(motor_pin_11, LOW);
          digitalWrite(motor_pin_15, LOW);
          
          digitalWrite(motor_pin_3, HIGH);
          digitalWrite(motor_pin_8, HIGH);
          digitalWrite(motor_pin_12, HIGH);
          digitalWrite(motor_pin_16, HIGH);
          
          digitalWrite(motor_pin_4, LOW);
          digitalWrite(motor_pin_9, LOW);
          digitalWrite(motor_pin_13, LOW);
          digitalWrite(motor_pin_17, LOW);
        break;
        case 1:  // 0110
          digitalWrite(motor_pin_1, LOW);
          digitalWrite(motor_pin_6, LOW);
          digitalWrite(motor_pin_10, LOW);
          digitalWrite(motor_pin_14, LOW);
          
          digitalWrite(motor_pin_2, HIGH);
          digitalWrite(motor_pin_7, HIGH);
          digitalWrite(motor_pin_11, HIGH);
          digitalWrite(motor_pin_15, HIGH);
          
          digitalWrite(motor_pin_3, HIGH);
          digitalWrite(motor_pin_8, HIGH);
          digitalWrite(motor_pin_12, HIGH);
          digitalWrite(motor_pin_16, HIGH);
          
          digitalWrite(motor_pin_4, LOW);
          digitalWrite(motor_pin_9, LOW);
          digitalWrite(motor_pin_13, LOW);
          digitalWrite(motor_pin_17, LOW);
        break;
        case 2:  //0101
          digitalWrite(motor_pin_1, LOW);
          digitalWrite(motor_pin_6, LOW);
          digitalWrite(motor_pin_10, LOW);
          digitalWrite(motor_pin_14, LOW);
          
          digitalWrite(motor_pin_2, HIGH);
          digitalWrite(motor_pin_7, HIGH);
          digitalWrite(motor_pin_11, HIGH);
          digitalWrite(motor_pin_15, HIGH);
          
          digitalWrite(motor_pin_3, LOW);
          digitalWrite(motor_pin_8, LOW);
          digitalWrite(motor_pin_12, LOW);
          digitalWrite(motor_pin_16, LOW);
          
          digitalWrite(motor_pin_4, HIGH);
          digitalWrite(motor_pin_9, HIGH);
          digitalWrite(motor_pin_13, HIGH);
          digitalWrite(motor_pin_17, HIGH);
        break;
        case 3:  //1001
          digitalWrite(motor_pin_1, HIGH);
          digitalWrite(motor_pin_6, HIGH);
          digitalWrite(motor_pin_10, HIGH);
          digitalWrite(motor_pin_14, HIGH);
          
          digitalWrite(motor_pin_2, LOW);
          digitalWrite(motor_pin_7, LOW);
          digitalWrite(motor_pin_11, LOW);
          digitalWrite(motor_pin_15, LOW);
          
          digitalWrite(motor_pin_3, LOW);
          digitalWrite(motor_pin_8, LOW);
          digitalWrite(motor_pin_12, LOW);
          digitalWrite(motor_pin_16, LOW);
          
          digitalWrite(motor_pin_4, HIGH);
          digitalWrite(motor_pin_9, HIGH);
          digitalWrite(motor_pin_13, HIGH);
          digitalWrite(motor_pin_17, HIGH);
        break;
      }
  }
  
  
  
  
  
  
  void myStepper :: move_left (int steps_to_move)
  {
    int steps_left = abs(steps_to_move);  // how many steps to take
  
    // determine mydirection based on whether steps_to_mode is + or -:
    if (steps_to_move > 0) { this->mydirection = 1; }
    if (steps_to_move < 0) { this->mydirection = 0; }
  
  
    // decrement the number of steps, moving one mystep each time:
    while (steps_left > 0)
    {
      
      unsigned long now = micros();
      // move only if the appropriate delay has passed:
      if (now - this->last_step_time >= this->step_delay)
      {
        // get the timeStamp of when you stepped:
        this->last_step_time = now;
        // increment or decrement the mystep number,
        // depending on mydirection:
        if (this->mydirection == 1)
        {
          this->step_number++;
          if (this->step_number == this->number_of_steps) {
            this->step_number = 0;
          }
        }
        else
        {
          if (this->step_number == 0) {
            this->step_number = this->number_of_steps;
          }
          this->step_number--;
        }
        // decrement the steps left:
        steps_left--;
        // mystep the motor to mystep number 0, 1, ..., {3 or 10}
        stepMotorLeft(this->step_number % 4);
  
      }
    }
  }
  
  
  
  
  void myStepper :: stepMotorLeft(int thisStep)
  {
    switch (thisStep) {
        case 0:  // 1010
          digitalWrite(motor_pin_1, HIGH);
          digitalWrite(motor_pin_6, HIGH);
          digitalWrite(motor_pin_10, HIGH);
          digitalWrite(motor_pin_14, HIGH);
  
          digitalWrite(motor_pin_2, LOW);
          digitalWrite(motor_pin_7, LOW);
          digitalWrite(motor_pin_11, LOW);
          digitalWrite(motor_pin_15, LOW);
          
          digitalWrite(motor_pin_3, HIGH);
          digitalWrite(motor_pin_8, HIGH);
          digitalWrite(motor_pin_12, HIGH);
          digitalWrite(motor_pin_16, HIGH);
          
          digitalWrite(motor_pin_4, LOW);
          digitalWrite(motor_pin_9, LOW);
          digitalWrite(motor_pin_13, LOW);
          digitalWrite(motor_pin_17, LOW);
        break;
        case 1:  // 0110
          digitalWrite(motor_pin_1, LOW);
          digitalWrite(motor_pin_6, LOW);
          digitalWrite(motor_pin_10, HIGH);
          digitalWrite(motor_pin_14, HIGH);
          
          digitalWrite(motor_pin_2, HIGH);
          digitalWrite(motor_pin_7, HIGH);
          digitalWrite(motor_pin_11, LOW);
          digitalWrite(motor_pin_15, LOW);
          
          digitalWrite(motor_pin_3, HIGH);
          digitalWrite(motor_pin_8, HIGH);
          digitalWrite(motor_pin_12, LOW);
          digitalWrite(motor_pin_16, LOW);
          
          digitalWrite(motor_pin_4, LOW);
          digitalWrite(motor_pin_9, LOW);
          digitalWrite(motor_pin_13, HIGH);
          digitalWrite(motor_pin_17, HIGH);
        break;
        case 2:  //0101
          digitalWrite(motor_pin_1, LOW);
          digitalWrite(motor_pin_6, LOW);
          digitalWrite(motor_pin_10, LOW);
          digitalWrite(motor_pin_14, LOW);
          
          digitalWrite(motor_pin_2, HIGH);
          digitalWrite(motor_pin_7, HIGH);
          digitalWrite(motor_pin_11, HIGH);
          digitalWrite(motor_pin_15, HIGH);
          
          digitalWrite(motor_pin_3, LOW);
          digitalWrite(motor_pin_8, LOW);
          digitalWrite(motor_pin_12, LOW);
          digitalWrite(motor_pin_16, LOW);
          
          digitalWrite(motor_pin_4, HIGH);
          digitalWrite(motor_pin_9, HIGH);
          digitalWrite(motor_pin_13, HIGH);
          digitalWrite(motor_pin_17, HIGH);
        break;
        case 3:  //1001
          digitalWrite(motor_pin_1, HIGH);
          digitalWrite(motor_pin_6, HIGH);
          digitalWrite(motor_pin_10, LOW);
          digitalWrite(motor_pin_14, LOW);
          
          digitalWrite(motor_pin_2, LOW);
          digitalWrite(motor_pin_7, LOW);
          digitalWrite(motor_pin_11, HIGH);
          digitalWrite(motor_pin_15, HIGH);
          
          digitalWrite(motor_pin_3, LOW);
          digitalWrite(motor_pin_8, LOW);
          digitalWrite(motor_pin_12, HIGH);
          digitalWrite(motor_pin_16, HIGH);
          
          digitalWrite(motor_pin_4, HIGH);
          digitalWrite(motor_pin_9, HIGH);
          digitalWrite(motor_pin_13, LOW);
          digitalWrite(motor_pin_17, LOW);
        break;
      }
  }
  
  
  
  
  const int stepsPerRevolution = 100;
  
  
  myStepper two_at_a_time(stepsPerRevolution, 2, 3, 4, 5, 8, 9, 10, 11, 42 , 43, 40, 41, 46, 47, 48, 49);
  int motor_pin1 = 22;
  int motor_pin2 = 23;

void setup() {
  // set the speed at 60 rpm:

  two_at_a_time.mysetSpeed(45);
    pinMode(motor_pin1, OUTPUT);
      pinMode(motor_pin2, OUTPUT);  
}


void start(int delay_time)
{
  digitalWrite(motor_pin1, HIGH);
  digitalWrite(motor_pin2, LOW);
  delay(delay_time);
  digitalWrite(motor_pin1, HIGH);
  digitalWrite(motor_pin2, HIGH);
}

void end_motor(int delay_time)
{
  digitalWrite(motor_pin1, LOW);
  digitalWrite(motor_pin2, HIGH);
  delay(delay_time);
  digitalWrite(motor_pin1, HIGH);
  digitalWrite(motor_pin2, HIGH);
}

int start_delay = 1000;
int stop_delay = 700;

void loop() {

  start(start_delay);
  two_at_a_time.move_forword(390);         //  forword
  delay(1000);
  
  end_motor(stop_delay);
  two_at_a_time.move_left(127);      //  left
  delay(400);
  two_at_a_time.move_forword(80); 
  delay(1000);

  start(start_delay);
  
  two_at_a_time.move_forword(233);         //  forword
  delay(1000);
  end_motor(stop_delay);
  
  two_at_a_time.move_left(127);      //  left
  delay(400);
  two_at_a_time.move_forword(80); 
  delay(1000);

  start(start_delay);

  two_at_a_time.move_forword(390);         //  forword
  delay(1000);
  end_motor(stop_delay);
  
  two_at_a_time.move_left(127);      //  left
  delay(400);
  two_at_a_time.move_forword(80); 
  delay(1000);  
  start(start_delay);

  two_at_a_time.move_forword(233);         //  forword
  delay(1000);
  end_motor(stop_delay);
  
  two_at_a_time.move_left(127);      //  left
  delay(400);
  two_at_a_time.move_forword(80); 
  delay(1000);
  //start(start_delay);
  

  two_at_a_time.move_forword(310);         //  forword
  delay(1000);
  //end_motor(stop_delay);
  
  
  two_at_a_time.move_left(127);      //  left
  delay(400);
  two_at_a_time.move_forword(80); 
  delay(1000);
  start(start_delay);
  
  two_at_a_time.move_forword(233);         //  forword
  delay(1000);
  end_motor(stop_delay);

  two_at_a_time.move_left(127);      //  left
  delay(400);
  two_at_a_time.move_forword(80); 
  delay(1000);
  //start(start_delay);

  two_at_a_time.move_forword(155);         //  forword
  delay(1000);
  //end_motor(stop_delay);

  two_at_a_time.move_left(127);      //  left
  delay(400);
  two_at_a_time.move_forword(80); 
  delay(1000);
  start(start_delay);

  two_at_a_time.move_forword(233);         //  forword
  delay(1000);
  end_motor(stop_delay);

  
  //two_at_a_time.move_left(127);      //  left
  //delay(400);
  //two_at_a_time.move_forword(80); 
  //delay(1000);
  //start(start_delay);

  //two_at_a_time.move_forword(210);         //  forword
  //delay(1000);
  //end_motor(stop_delay);


  //two_at_a_time.move_left(127);      //  left
  //delay(400);
  //two_at_a_time.move_forword(80); 
  //delay(1000);
  //start(start_delay);


  //two_at_a_time.move_forword(310);         //  forword
  //delay(1000);
  //end_motor(stop_delay);
  
  while(1)
  {
    
  }

  //headingDegrees = getDirection(); 
  
  //Serial.print("Heading (degrees): "); Serial.println(headingDegrees);
  //erial.print("Reference Heading (degrees): "); Serial.println(REFERENCE_HEADING_DEGREE);
  delay(500);

}
