#include "pulseSensor.h"

//******************************************************************************
int InputPin;           // Analog input pin for PulseSensor.
int BlinkPin;           // pin to blink in beat, or -1.
int FadePin;            // pin to fade on beat, or -1.

// Pulse detection output variables.
// Volatile because our pulse detection code could be called from an Interrupt
volatile int BPM;                // int that holds raw Analog in 0. updated every call to readSensor()
volatile int Signal;             // holds the latest incoming raw data (0..1023)
volatile int IBI;                // int that holds the time interval (ms) between beats! Must be seeded!
volatile int Pulse;          // "True" when User's live heartbeat is detected. "False" when not a "live beat".
volatile int QS;             // The start of beat has been detected and not read by the Sketch.
volatile int FadeLevel;          // brightness of the FadePin, in scaled PWM units. See FADE_SCALE
volatile int threshSetting;      // used to seed and reset the thresh variable
volatile int amp;                         // used to hold amplitude of pulse waveform, seeded (sample value)
volatile unsigned long lastBeatTime;      // used to find IBI. Time (sampleCounter) of the previous detected beat start.

// Variables internal to the pulse detection algorithm.
// Not volatile because we use them only internally to the pulse detection.
unsigned long sampleIntervalMs;  // expected time between calls to readSensor(), in milliseconds.
int rate[10];                    // array to hold last ten IBI values (ms)
unsigned long sampleCounter;     // used to determine pulse timing. Milliseconds since we started.
int P;                           // used to find peak in pulse wave, seeded (sample value)
int T;                           // used to find trough in pulse wave, seeded (sample value)
int thresh;                      // used to find instant moment of heart beat, seeded (sample value)
int firstBeat;               // used to seed rate array so we startup with reasonable BPM
int secondBeat;              // used to seed rate array so we startup with reasonable BPM

//******************************************************************************
// initialize the ADC module.
void ADC_Init() {
  //GPIO_RCGCGPIO |= 0x10; // enable portE(AIN0 is on PE3)
  SYSCTL_RCGCGPIO_R |= 0x10;
  //SYSCTL_RCGCADC_R |= 0x1; // enable ADC module 0 (provide the clock)
  SYSCTL_RCGCADC_R |= 0x1;
  while ((SYSCTL_RCGCADC_R & 0x0000001) == 0) {};
  
  //GPIOE_AFSEL_R |= 0x8; // enable alternate function (TEMP)
  GPIO_PORTE_AFSEL_R |= 0xE;
  GPIO_PORTE_DEN_R &= ~0x8; // disable digital function
  GPIO_PORTE_AMSEL_R |= 0x8; // enable analog function
  
  ADC0_ACTSS_R &= ~0x8; // disable SS3 during configuration
  ADC0_EMUX_R |= ~0xF000;//0x5000; // software trigger conversion (Timer)
  ADC0_SSMUX3_R |= 0x0;
  
  // set ADC CTL
  ADC0_SSCTL3_R |= 0x6; // take one sample at a time, set flag at 1st sample
  ADC0_SSCTL3_R |= (1<<3); // Read from Internal Temperature Sensor
  
  ADC0_ACTSS_R |= 0x8; // enable ADC0 sequencer 3
  //ADC_IM_R |= (1<<3); //Masking interrupt for SS3
  ADC0_IM_R |= (1<<3);
  
  ADC0_ISC_R |= (1<<3); // clear completion flag once before start
  //NVIC_EN0 |= (1<<17); // enable interrupt 17 for ADC
  NVIC_EN0_R |= (1<<17);
}

//******************************************************************************

void pulseSensor_Init() {
  // Initialize (seed) the pulse detector
  for (int i = 0; i < 10; ++i) {
    rate[i] = 0;
  }
  QS = 0; //false;
  BPM = 0;
  IBI = 600;                  // 600ms per beat = 100 Beats Per Minute (BPM)
  Pulse = 0; //false;
  sampleCounter = 0;
  lastBeatTime = 0;
  P = 2047;                  // peak at 1/2 the input range of 0..4095
  T = 2047;                   // trough at 1/2 the input range.
  threshSetting = 2047;       // used to seed and reset the thresh variable (4095/2)
  thresh = 2047;              // threshold a little above the trough
  amp = 410;                  // beat amplitude 1/10 of input range.
  firstBeat = 1; //true;      // looking for the first beat
  secondBeat = 0; //false;    // not yet looking for the second beat in a row
}

// ADC handler is on whenever timer handler begin heart beat sampling
// sample every 2 ms
void ADC0_Handler( void ) {
  Signal = ADC0_SSFIFO3_R;
  
  sampleCounter += 2;         // keep track of the time in ms with this variable
  int N = sampleCounter - lastBeatTime;      // monitor the time since the last beat to avoid noise
  // printf("%d\n", N);
  
  //  find the peak and trough of the pulse wave
  if (Signal < thresh && N > (IBI / 5) * 3) { // avoid dichrotic noise by waiting 3/5 of last IBI
    if (Signal < T) {                        // T is the trough
      T = Signal;                            // keep track of lowest point in pulse wave
    }
  }

  if (Signal > thresh && Signal > P) {       // thresh condition helps avoid noise
    P = Signal;                              // P is the peak
  }                                          // keep track of highest point in pulse wave
    
  //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
  // signal surges up in value every time there is a pulse
  if (N > 250) {                             // avoid high frequency noise, 
    // printf(".1\n");
    if ( (Signal > thresh) && (Pulse == 0) && (N > (IBI / 5) * 3) ) {
      // printf(".2\n");
      Pulse = 1;                          // set the Pulse flag when we think there is a pulse
      IBI = sampleCounter - lastBeatTime;    // measure time between beats in mS
      lastBeatTime = sampleCounter;          // keep track of time for next pulse
      
      if (secondBeat) {                     // if this is the second beat, if secondBeat == TRUE
         // printf("sec\n");
        secondBeat = 0;                  // clear secondBeat flag
        for (int i = 0; i <= 9; i++) {       // seed the running total to get a realisitic BPM at startup
          rate[i] = IBI;
        }
      }

      if (firstBeat) {                       // if it's the first time we found a beat, if firstBeat == TRUE
        // printf("1st\n");
        firstBeat = 0;                    // clear firstBeat flag
        secondBeat = 1;                   // set the second beat flag
        // IBI value is unreliable so discard it
        return;
      }


      // keep a running total of the last 10 IBI values
      int runningTotal = 0;                  // clear the runningTotal variable

      for (int i = 0; i <= 8; i++) {          // shift data in the rate array
        rate[i] = rate[i + 1];                // and drop the oldest IBI value
        runningTotal += rate[i];              // add up the 9 oldest IBI values
      }

      rate[9] = IBI;                          // add the latest IBI to the rate array
      runningTotal += rate[9];                // add the latest IBI to runningTotal
      runningTotal /= 10;                     // average the last 10 IBI values
      BPM = 60000 / runningTotal;             // how many beats can fit into a minute? that's BPM!
      QS = 1;                              // set Quantified Self flag (we detected a beat)
      // printf("BPM: %d\n", BPM);
    }
  }

  if (Signal < thresh && Pulse == 1) {  // when the values are going down, the beat is over
    // printf("reset pulse\n");
    Pulse = 0;                         // reset the Pulse flag so we can do it again
    amp = P - T;                           // get amplitude of the pulse wave
    thresh = amp / 2 + T;                  // set thresh at 50% of the amplitude
    P = thresh;                            // reset these for next time
    T = thresh;
  }

  if (N > 2500) {                          // if 2.5 seconds go by without a beat
    // printf("n>2500\n");
    thresh = threshSetting;                // set thresh default
    P = 2047;                               // set P default
    T = 2047;                               // set T default
    lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date
    firstBeat = 1;                      // set these to avoid noise
    secondBeat = 0;                    // when we get the heartbeat back
  } 
  ADC0_ISC_R |= (1<<3); // clear completion flag
}