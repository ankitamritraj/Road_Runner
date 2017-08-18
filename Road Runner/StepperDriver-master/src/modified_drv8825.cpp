#include <Arduino.h>


// used internally by the library to mark unconnected pins
#define PIN_UNCONNECTED -1
#define IS_CONNECTED(pin) (pin != PIN_UNCONNECTED)

/*
 * calculate the step pulse in microseconds for a given rpm value.
 * 60[s/min] * 1000000[us/s] / microsteps / steps / rpm
 */
#define STEP_PULSE(steps, microsteps, rpm) (60*1000000L/steps/microsteps/rpm)

inline void microWaitUntil(unsigned long target_micros){
    yield();
    while (micros() < target_micros);
}
#define DELAY_MICROS(us) microWaitUntil(micros() + us)

/*
 * Basic Stepper Driver class.
 * Microstepping level should be externally controlled or hardwired.
 */
class BasicStepperDriver {
protected:
    int motor_steps;
    int rpm = 60;
    int dir_pin;
    int step_pin;
    int enable_pin = PIN_UNCONNECTED;

    // current microstep level, must be < getMaxMicrostep()
    // for 1:16 microsteps is 16
    unsigned microsteps = 1;
    // step pulse duration (microseconds), depends on rpm and microstep level
    unsigned long step_pulse;

    void setDirection(int direction);
    void init(void);
    void calcStepPulse(void);

    // tWH(STEP) pulse duration, STEP high, min value (us)
    static const int step_high_min = 1;
    // tWL(STEP) pulse duration, STEP low, min value (us)
    static const int step_low_min = 1;
    // tWAKE wakeup time, nSLEEP inactive to STEP (us)
    static const int wakeup_time = 0;

    // Get max microsteps supported by the device
    virtual unsigned getMaxMicrostep();

private:
    // microstep range (1, 16, 32 etc)
    static const unsigned MAX_MICROSTEP = 128;

public:
    /*
     * Basic connection: DIR, STEP are connected.
     */
    BasicStepperDriver(int steps, int dir_pin, int step_pin);
    BasicStepperDriver(int steps, int dir_pin, int step_pin, int enable_pin);
    /*
     * Set current microstep level, 1=full speed, 32=fine microstepping
     * Returns new level or previous level if value out of range
     */
    unsigned setMicrostep(unsigned microsteps);
    /*
     * Move the motor a given number of steps.
     * positive to move forward, negative to reverse
     */
    void move(long steps);
    /*
     * Rotate the motor a given number of degrees (1-360)
     */
    void rotate(long deg);
    inline void rotate(int deg){
        rotate((long)deg);
    };
    /*
     * Rotate using a float or double for increased movement precision.
     */
    void rotate(double deg);
    /*
     * Set target motor RPM (1-200 is a reasonable range)
     */
    void setRPM(unsigned rpm);
    /*
     * Turn off/on motor to allow the motor to be moved by hand/hold the position in place
     */
    void enable(void);
    void disable(void);
};

/*
 * Basic connection: only DIR, STEP are connected.
 * Microstepping controls should be hardwired.
 */
BasicStepperDriver::BasicStepperDriver(int steps, int dir_pin, int step_pin)
:motor_steps(steps), dir_pin(dir_pin), step_pin(step_pin)
{
    init();
}

BasicStepperDriver::BasicStepperDriver(int steps, int dir_pin, int step_pin, int enable_pin)
:motor_steps(steps), dir_pin(dir_pin), step_pin(step_pin), enable_pin(enable_pin)
{
    init();
}

void BasicStepperDriver::init(void){
    pinMode(dir_pin, OUTPUT);
    digitalWrite(dir_pin, HIGH);

    pinMode(step_pin, OUTPUT);
    digitalWrite(step_pin, LOW);

    if IS_CONNECTED(enable_pin){
        pinMode(enable_pin, OUTPUT);
        digitalWrite(enable_pin, HIGH); // disable
    }

    setMicrostep(1);
    setRPM(60); // 60 rpm is a reasonable default

    enable();
}


void BasicStepperDriver::calcStepPulse(void){
    step_pulse = STEP_PULSE(rpm, motor_steps, microsteps);
}

/*
 * Set target motor RPM (1-200 is a reasonable range)
 */
void BasicStepperDriver::setRPM(unsigned rpm){
    this->rpm = rpm;
    calcStepPulse();
}

/*
 * Set stepping mode (1:microsteps)
 * Allowed ranges for BasicStepperDriver are 1:1 to 1:128
 */
unsigned BasicStepperDriver::setMicrostep(unsigned microsteps){
    for (unsigned ms=1; ms <= this->getMaxMicrostep(); ms<<=1){
        if (microsteps == ms){
            this->microsteps = microsteps;
            break;
        }
    }
    calcStepPulse();
    return this->microsteps;
}

/*
 * DIR: forward HIGH, reverse LOW
 */
void BasicStepperDriver::setDirection(int direction){
    digitalWrite(dir_pin, (direction<0) ? LOW : HIGH);
}

/*
 * Move the motor a given number of steps.
 * positive to move forward, negative to reverse
 */
void BasicStepperDriver::move(long steps){
    if (steps >= 0){
        setDirection(1);
    } else {
        setDirection(-1);
        steps = -steps;
    }
    /*
     * We currently try to do a 50% duty cycle so it's easy to see.
     * Other option is step_high_min, pulse_duration-step_high_min.
     */
    unsigned long pulse_duration = step_pulse/2;
    while (steps--){
        digitalWrite(step_pin, HIGH);
        unsigned long next_edge = micros() + pulse_duration;
        microWaitUntil(next_edge);
        digitalWrite(step_pin, LOW);
        microWaitUntil(next_edge + pulse_duration);
    }
}

/*
 * Move the motor a given number of degrees (1-360)
 */
void BasicStepperDriver::rotate(long deg){
    long steps = deg * motor_steps * (long)microsteps / 360;
    move(steps);
}
/*
 * Move the motor with sub-degree precision.
 * Note that using this function even once will add 1K to your program size
 * due to inclusion of float support.
 */
void BasicStepperDriver::rotate(double deg){
    long steps = deg * motor_steps * microsteps / 360;
    move(steps);
}

/*
 * Enable/Disable the motor by setting a digital flag
 */
void BasicStepperDriver::enable(void){
    if IS_CONNECTED(enable_pin){
        digitalWrite(enable_pin, LOW);
    }
}

void BasicStepperDriver::disable(void){
    if IS_CONNECTED(enable_pin){
        digitalWrite(enable_pin, HIGH);
    }
}

unsigned BasicStepperDriver::getMaxMicrostep(){
    return BasicStepperDriver::MAX_MICROSTEP;
}



class A4988 : public BasicStepperDriver {
protected:
    static const uint8_t MS_TABLE[];
    int ms1_pin = PIN_UNCONNECTED;
    int ms2_pin = PIN_UNCONNECTED;
    int ms3_pin = PIN_UNCONNECTED;
    void init(void);
    // tA STEP minimum, HIGH pulse width (1us)
    static const int step_high_min = 1;
    // tB STEP minimum, LOW pulse width (1us)
    static const int step_low_min = 1;
    // wakeup time, nSLEEP inactive to STEP (1000us)
    static const int wakeup_time = 1000;
    // also 200ns between ENBL/DIR/MSx changes and STEP HIGH

    // Get the microstep table
    virtual const uint8_t* getMicrostepTable();
    virtual size_t getMicrostepTableSize();

    // Get max microsteps supported by the device
    unsigned getMaxMicrostep() override;

private:
    // microstep range (1, 16, 32 etc)
    static const unsigned MAX_MICROSTEP = 16;

public:
    /*
     * Basic connection: only DIR, STEP are connected.
     * Microstepping controls should be hardwired.
     */
    A4988(int steps, int dir_pin, int step_pin);
    A4988(int steps, int dir_pin, int step_pin, int enable_pin);
    
    /*
     * Fully wired. All the necessary control pins for A4988 are connected.
     */
    A4988(int steps, int dir_pin, int step_pin, int ms1_pin, int ms2_pin, int ms3_pin);
    A4988(int steps, int dir_pin, int step_pin, int enable_pin, int ms1_pin, int ms2_pin, int ms3_pin);
    unsigned setMicrostep(unsigned microsteps);
};

const uint8_t A4988::MS_TABLE[] = {0b000, 0b001, 0b010, 0b011, 0b111};

/*
 * Basic connection: only DIR, STEP are connected.
 * Microstepping controls should be hardwired.
 */
A4988::A4988(int steps, int dir_pin, int step_pin)
:BasicStepperDriver(steps, dir_pin, step_pin)
{}

A4988::A4988(int steps, int dir_pin, int step_pin, int enable_pin)
:BasicStepperDriver(steps, dir_pin, step_pin, enable_pin)
{}

/*
 * Fully wired.
 * All the necessary control pins for A4988 are connected.
 */
A4988::A4988(int steps, int dir_pin, int step_pin, int ms1_pin, int ms2_pin, int ms3_pin)
:BasicStepperDriver(steps, dir_pin, step_pin),
    ms1_pin(ms1_pin), ms2_pin(ms2_pin), ms3_pin(ms3_pin)
{}

A4988::A4988(int steps, int dir_pin, int step_pin, int enable_pin, int ms1_pin, int ms2_pin, int ms3_pin)
:BasicStepperDriver(steps, dir_pin, step_pin, enable_pin),
ms1_pin(ms1_pin), ms2_pin(ms2_pin), ms3_pin(ms3_pin)
{}

void A4988::init(void){
    BasicStepperDriver::init();

    if (!IS_CONNECTED(ms1_pin) || !IS_CONNECTED(ms2_pin) || !IS_CONNECTED(ms3_pin)){
        return;
    }

    pinMode(ms1_pin, OUTPUT);
    pinMode(ms2_pin, OUTPUT);
    pinMode(ms3_pin, OUTPUT);
}

/*
 * Set microstepping mode (1:divisor)
 * Allowed ranges for A4988 are 1:1 to 1:16
 * If the control pins are not connected, we recalculate the timing only
 */
unsigned A4988::setMicrostep(unsigned microsteps){
    BasicStepperDriver::setMicrostep(microsteps);

    if (!IS_CONNECTED(ms1_pin) || !IS_CONNECTED(ms2_pin) || !IS_CONNECTED(ms3_pin)){
        return this->microsteps;
    }

    const uint8_t* ms_table = this->getMicrostepTable();
    size_t ms_table_size = this->getMicrostepTableSize();

    int i = 0;
    while (i < ms_table_size){
        if (this->microsteps & (1<<i)){
            uint8_t mask = ms_table[i];
            digitalWrite(ms3_pin, mask & 4);
            digitalWrite(ms2_pin, mask & 2);
            digitalWrite(ms1_pin, mask & 1);
            break;
        }
        i++;
    }
    return this->microsteps;
}

const uint8_t* A4988::getMicrostepTable(){
    return A4988::MS_TABLE;
}

size_t A4988::getMicrostepTableSize(){
    return sizeof(A4988::MS_TABLE);
}

unsigned A4988::getMaxMicrostep(){
    return A4988::MAX_MICROSTEP;
}

class DRV8825 : public A4988 {
protected:
    static const uint8_t MS_TABLE[];
    // tWH(STEP) pulse duration, STEP high, min value (1.9us)
    static const int step_high_min = 2;
    // tWL(STEP) pulse duration, STEP low, min value (1.9us)
    static const int step_low_min = 2;
    // tWAKE wakeup time, nSLEEP inactive to STEP (1000us)
    static const int wakeup_time = 1700;
    // also 650ns between ENBL/DIR/MODEx changes and STEP HIGH

    // Get the microstep table
    const uint8_t* getMicrostepTable() override;
    size_t getMicrostepTableSize() override;

    // Get max microsteps supported by the device
    unsigned getMaxMicrostep() override;

private:
    // microstep range (1, 16, 32 etc)
    static const unsigned MAX_MICROSTEP = 32;

public:
    DRV8825(int steps, int dir_pin, int step_pin);
    DRV8825(int steps, int dir_pin, int step_pin, int enable_pin);
    DRV8825(int steps, int dir_pin, int step_pin, int mode0_pin, int mode1_pin, int mode2_pin);
    DRV8825(int steps, int dir_pin, int step_pin, int enable_pin, int mode0_pin, int mode1_pin, int mode2_pin);
};

const uint8_t DRV8825::MS_TABLE[] = {0b000, 0b001, 0b010, 0b011, 0b100, 0b111};

DRV8825::DRV8825(int steps, int dir_pin, int step_pin)
:A4988(steps, dir_pin, step_pin)
{}

DRV8825::DRV8825(int steps, int dir_pin, int step_pin, int enable_pin)
:A4988(steps, dir_pin, step_pin, enable_pin)
{}

/*
 * A4988-DRV8825 Compatibility map: MS1-MODE0, MS2-MODE1, MS3-MODE2
 */
DRV8825::DRV8825(int steps, int dir_pin, int step_pin, int mode0_pin, int mode1_pin, int mode2_pin)
:A4988(steps, dir_pin, step_pin, mode0_pin, mode1_pin, mode2_pin)
{}

DRV8825::DRV8825(int steps, int dir_pin, int step_pin, int enable_pin, int mode0_pin, int mode1_pin, int mode2_pin)
:A4988(steps, dir_pin, step_pin, enable_pin, mode0_pin, mode1_pin, mode2_pin)
{}

const uint8_t* DRV8825::getMicrostepTable()
{
    return (uint8_t*)DRV8825::MS_TABLE;
}

size_t DRV8825::getMicrostepTableSize()
{
    return sizeof(DRV8825::MS_TABLE);
}

unsigned DRV8825::getMaxMicrostep(){
    return DRV8825::MAX_MICROSTEP;
}


#define MOTOR_STEPS 200

// All the wires needed for full functionality
#define DIR 8
#define STEP 9
#define ENBL 10

// 2-wire basic config, microstepping is hardwired on the driver
// BasicStepperDriver stepper(DIR, STEP);

// microstep control for DRV8834
//#define M0 10
//#define M1 11
//DRV8834 stepper(MOTOR_STEPS, DIR, STEP, M0, M1);
// DRV8834 stepper(MOTOR_STEPS, DIR, STEP, ENBL, M0, M1);

// microstep control for A4988
// #define MS1 10
// #define MS2 11
// #define MS3 12
// A4988 stepper(MOTOR_STEPS, DIR, STEP, MS1, MS2, MS3);

//microstep control for DRV8825
// same pinout as A4988, different pin names, supports 32 microsteps
 #define MODE0 11
 #define MODE1 12
 #define MODE2 13
 DRV8825 stepper(MOTOR_STEPS, DIR, STEP, MODE0, MODE1, MODE2);

void setup() {
    /*
     * Set target motor RPM.
     * These motors can do up to about 200rpm.
     * Too high will result in a high pitched whine and the motor does not move.
     */
    Serial.begin(9600);
    Serial.println("in the setup");
    stepper.setRPM(120);
    
}

void loop() {
    delay(1000);

    /*
     * Moving motor at full speed is simple:
     */
    stepper.setMicrostep(1); // make sure we are in full speed mode

    // these two are equivalent: 180 degrees is 100 steps in full speed mode
    stepper.move(100);

    Serial.println("moving 100 steps ");
    
    //stepper.rotate(180);

    // one full reverse rotation
    //stepper.move(-100);
    //stepper.rotate(-180);
    //Serial.println("here");

    /*
     * Microstepping mode: 1,2,4,8,16 or 32(DRV8834 only)
     * Mode 1 is full speed.
     * Mode 32 is 32 microsteps per step.
     * The motor should rotate just as fast (set RPM),
     * but movement precision is increased.
     */
    //stepper.setMicrostep(8);

    // 180 degrees now takes 100 * 8 microsteps
    //stepper.move(100*8);
    //stepper.rotate(180);

    // as you can see, using degrees is easier
    //stepper.move(-100*8);
    //stepper.rotate(-180);

    //delay(5000);
}