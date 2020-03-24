//  VARIABLES
int pulsePin = 0;                 // Pulse Sensor purple wire connected to analog pin 0
int blinkPin = 12;                // pin to blink led at each beat
int fadePin = 9;                  // pin to do fancy classy fading blink at each beat
int fadeRate = 0;                 // used to fade LED on with PWM on fadePin
// these variables are volatile because they are used during the interrupt service routine!
volatile int BPM;                   // used to hold the pulse rate
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // holds the time between beats, the Inter-Beat Interval
volatile boolean Pulse = false;     // true when pulse wave is high, false when it’s low
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.
volatile int thresh = 512;
volatile boolean upable = true;
   
void setup(){
  pinMode(blinkPin,OUTPUT);         // pin that will blink to your heartbeat!
  pinMode(fadePin,OUTPUT);          // pin that will fade to your heartbeat!
  Serial.begin(115200);             // we agree to talk fast!
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS
   // UN-COMMENT THE NEXT LINE IF YOU ARE POWERING The Pulse Sensor AT LOW VOLTAGE,
   // AND APPLY THAT VOLTAGE TO THE A-REF PIN
   //analogReference(EXTERNAL);
}
void loop(){
    sendDataToProcessing('S', Signal);     // send Processing the raw Pulse Sensor data
    if (QS == true){                       // Quantified Self flag is true when arduino finds a heartbeat
          upable = true;
          fadeRate = 0;                  // Set ‘fadeRate’ Variable to 255 to fade LED with pulse
          // fadeRate = 255;
          //sendDataToProcessing('B', BPM);   // send heart rate with a ‘B’ prefix
          //sendDataToProcessing('Q', IBI);   // send time between beats with a ‘Q’ prefix
          QS = false;                      // reset the Quantified Self flag for next time
          sendDataToProcessing('T', thresh);
    }
    lightUpAndDown();
    // ledFadeFireFlyToBeat();
    delay(3);                             //  take a break
}

void ledFadeToBeat(){
    fadeRate -= 15;                         //  set LED fade value
    fadeRate = constrain(fadeRate,0,255);   //  keep LED fade value from going into negative numbers!
    analogWrite(fadePin,fadeRate);          //  fade LED
}

// bright like fire-Fly
void ledFadeFireFlyToBeat(){
    if(fadeRate > 200 ) { 
      fadeRate -= 20;
    } else if ( fadeRate > 150 ) { 
      fadeRate -= 10;
    } else if ( fadeRate > 100 ) { 
      fadeRate -= 5;
    } else if ( fadeRate > 50 ) { 
      fadeRate -= 3;
    } else if ( fadeRate > 0 ) { 
      fadeRate -= 1;
    }    
    fadeRate = constrain(fadeRate,0,255);   //  keep LED fade value from going into negative numbers!
    analogWrite(fadePin,fadeRate);          //  fade LED
}

// bright up and down
void lightUpAndDown() {
  if (upable==true) {
    fadeRate += 5;
    fadeRate = constrain(fadeRate,0,255); 
    analogWrite(fadePin,fadeRate);
    if (fadeRate >  254) {
      upable = false;
    }
  } else {
    ledFadeFireFlyToBeat();
  }
}
 
void sendDataToProcessing(char symbol, int data ){
    // Serial.print(symbol);                // symbol prefix tells Processing what type of data is coming
    Serial.println(data);                // the data to send culminating in a carriage return
}
