#include <avr/io.h>
#include <avr/interrupt.h>
//Leave-me-alone-box with electric shock surprise - two toggle switches
//written by Philip Long
//2015

//Function Declaration
    void ADC_init(void); //initialize adc for sensor readings
    void PWM_init(void); // initialize pwm with 16bit timer 1 for servo control
    void interrupt_init(void); // initialize interrupts on int0 and int1
    bool pause (unsigned long startTime, int waitTime ); //initialize pause function for behavior delays
    void update_s1(void); // initialize update function for behavior control  
    void update_s2(void); // initialize update function for behavior control 

    
    
    
      
//Variable declaration 
    static volatile unsigned long timeS1 = 0; 
    static volatile unsigned long timeS2 = 0;
    static unsigned long eventTimeS1 = 0;
    static unsigned long eventTimeS2 = 0;
    static volatile int triggerS1 = 0;
    static volatile int triggerS2 = 0;
    static int servoMax = 2500;
    static int servoMin = 600;
    static volatile int switchCount = 0;
    static volatile int eventNumS1 = 0;
    static volatile int eventNumS2 = 0;
    static int tempS1;
    static int tempS2;
    //servo variables -- 
    //0 = arm inside box, 
    //100 = arm touching active switch, 
    //110 = arm touching inactive switch
    //S1 is lower switch
    static int s1_0 = 1900;
    static int s1_100 = 1390;
    static int s1_110 = 1345;
    static int s2_0 = 2300;
    static int s2_100 = 1860;
    static int s2_110 = 1785;
    static volatile int TempADC1 = 0;
    static volatile int TempADC0 = 0;
    int i;
    



void setup() 
{
 //Initialization Process
   Serial.begin(9600);//initialize serial
   PWM_init(); // initialize PWM for servo controll
   ADC_init(); //initialize adc
   delay(20); // pause to allow adc to set up
   interrupt_init(); // initialize interrupts on int0 and int1 pins
      
     
    
// Servo pin setup
  DDRB |= (1<<PB1)|(1<<PB2); //Set pins to output for servo controll
  ICR1 = 19999; //set top value for 20ms pwm
  OCR1A = s1_0;// set both servos to initial starting positions
  OCR1B = s2_0;
  delay(100); //small delay to allow servos to reach initial position if needed
 
 // shock pin setup, intially set to low - inactive
  DDRD |= (1<<PD7);
  PORTD &=~(1<<PD7);
 
 
 
//INT0 and INT1 Switch set up
  DDRD &=~(1<<PD2)|(1<<PD3);
  PORTD |=(1<<PD2)|(1<<PD3);
  

}
void loop() {
  //Activate shock if switch count is achieved, automatic shut off after 10 seconds if no switch is activated
   if (switchCount >= 9)
   {
     PORTD |= (1<<PD7);
     if (pause(timeS1, 10000))
       {
         if (pause(timeS2, 10000))
         {
           switchCount = 8;
           PORTD &= ~(1<<PD7);
         }
       }
   }

   switch(triggerS1)
   {
     case 1: 
       behavior1_1(); // Behaviour 1: Fast arm action and retreat 
       break;
     case 2: 
       behavior2_1(); // Behaviour 2: Fast arm action, pause, fast retreat
       break;
     case 3: 
       behavior3_1(); // Behaviour 3: fast arm action, repeat button smash, slow retreat
       break;
     case 4: 
       behavior4_1(); // Behaviour 4: slow arm action,pause, close, pause, slow retreat
       break;
     case 5: 
       behavior5_1(); // Behaviour 5: slow arm out, pause slow retreat, pause, slow action, slow retreat
       break;
     case 6: 
       behavior6_1(); // Behaviour 6: Pause, Fast arm action and retreat 
       break;
     case 7: 
       behavior7_1(); // Behaviour 7: Fast arm out, pause, slow action, pause, fast retreat.
       break;
     case 8: 
       behavior1_1(); // Behaviour 1: Fast arm action and retreat 
       break;
     case 9: 
       behavior1_1(); // Behaviour 1: Fast arm action and retreat 
       break;
     default: 
       OCR1A = s1_0;
       break;
   }
   
   switch(triggerS2)
   {
     case 1: 
       behavior1_2(); // Behaviour 1: Fast arm action and retreat 
       break;
     case 2: 
       behavior2_2(); // Behaviour 2: Fast arm action, pause, fast retreat
       break;
     case 3:  
       behavior3_2(); // Behaviour 3: fast arm action, repeat button smash, slow retreat
       break;
     case 4: 
       behavior4_2(); // Behaviour 4: slow arm action,pause, close, pause, slow retreat
       break;
     case 5: 
       behavior5_2(); // Behaviour 5: slow arm out, pause slow retreat, pause, slow action, slow retreat
       break;
     case 6: 
       behavior6_2(); // Behaviour 6: Pause, Fast arm action and retreat 
       break;
     case 7: 
       behavior7_2(); // Behaviour 7: Fast arm out, pause, slow action, pause, fast retreat.
       break;
     case 8: 
       behavior1_2(); // Behaviour 1: Fast arm action and retreat 
       break;
     case 9: 
       behavior1_2(); // Behaviour 1: Fast arm action and retreat 
       break;
     default: 
       OCR1B = s2_0;
     break;
   }
 
}//********************************************************************END MAIN ********************************************




  //Functions Below
  //
  // 
  // 
  // 
  ISR(INT1_vect) // toggle switch 1 interrupt
{
  if (millis() - timeS1 >= 500)
    {
     ADMUX |= (1<<MUX0); //select mux1
     delay(40);
     //Serial.print("Sample Start ADC 1\n");
      for(i = 0; i < 10; i++)
        {  TempADC1 += ADC;
         // Serial.print(TempADC1);
          //Serial.print('\n');
          delay(1);}
     // Serial.print("Sample End\n\n\n");
     if (TempADC1 == 0)
      {
      
        if (triggerS1>0)
          {
             triggerS1 = 1;
          }
        else 
         {
           triggerS1 = random(1,9);
          
          }
      timeS1 = millis();
      switchCount = switchCount+1;
      eventNumS1 = 0;
      }
      TempADC1 = 0;
  }
}


ISR(INT0_vect) // toggle switch 2 interrupt
{
  if (millis() - timeS2 >= 500)
  {
    ADMUX &= ~(1<<MUX0); //select mux0
     delay(40);
    // Serial.print("Sample Start ADC 0\n");
      for(i = 0; i < 10; i++)
        {  TempADC0 += ADC;
        //  Serial.print(TempADC0);
         // Serial.print('\n');
          delay(1);}
     // Serial.print("Sample End\n\n\n");
     if (TempADC0 == 0)
      {
      if (triggerS2>0)
        {
          triggerS2 = 1;
        }
      else
        {   
         triggerS2 = random(1,9);
        }
       timeS2 = millis();
       switchCount = switchCount+1;
       eventNumS2 = 0;
     }
     TempADC0 = 0;
  }
}

bool pause (unsigned long startTime, int waitTime) //controlls behavior timing in milliseconds
{
  if (millis() - startTime >= waitTime)
  {
    return(1);
  }
  else return(0);
}

void update_s1(void) // updates time of last servo event and servo event number for behavior control
{
  eventTimeS1 = millis();
  eventNumS1 += 1;
  tempS1 = 0;
}

void update_s2(void) // updates time of last servo event and servo event number for behavior control
{
  eventTimeS2 = millis();
  eventNumS2 += 1;
  tempS2 = 0;
}
  
  
  
  
  //<<<<<<<<<<<<<<<<<BEHAVIORS>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  //
  //
  //
  //
  //
  
  
  
  
  //************************************BEHAVIOR ONE ******************************** 
 //FAST ARM ACTION, FAST CLOSE 
  void behavior1_1(void)
  {
     if (pause(timeS1, 200) && (eventNumS1 == 0))
       {
           update_s1();
           OCR1A = s1_110;
       }
     if (pause(eventTimeS1, 200) && (eventNumS1 == 1))
       {
           OCR1A = s1_0;
           triggerS1 = 0;
       } 
   }
  
  void behavior1_2(void)
    {
     if (pause(timeS2, 200) && (eventNumS2 == 0))
       {
           update_s2();
           OCR1B = s2_110;
       }
     if (pause(eventTimeS2, 200) && (eventNumS2 == 1))
       {
           OCR1B = s2_0;
           triggerS2 = 0;
       }  
  }
    
    
    
    
    
    //******************************** BEHAVIOR TWO***************************
    //FAST ARM ACTION, PAUSE, FAST CLOSE
   void behavior2_1(void)
    {   
     if (pause(timeS1, 200) && (eventNumS1 == 0))
       {
         OCR1A = s1_110;
         update_s1();
       }
     if (pause(eventTimeS1, 1500) && (eventNumS1 == 1))
       {
         OCR1A = s1_0;
         triggerS1 = 0;
       }  
     
   }
    
   void behavior2_2(void)
    {   
     if (pause(timeS2, 200) && (eventNumS2 == 0))
       {
         OCR1B = s2_110;
         update_s2();
       }
     if (pause(eventTimeS2, 1500) && (eventNumS2 == 1))
       {
         OCR1B = s2_0;
         triggerS2 = 0;
       }  
     
   }
    
    
    //*****************************BEHAVIOR THREE*************************8
    // Behaviour 3: fast arm action, pause, repeat button smash, slow retreat
    
     void behavior3_1(void)
    { 
     if (pause(timeS1, 200) && (eventNumS1 == 0))
       {
         OCR1A = s1_110;
         update_s1();
       }
     if (pause(eventTimeS1, 400) && (eventNumS1 == 1))
       {
         update_s1();
       }
     if (pause(eventTimeS1, 400) && (eventNumS1 == 2))
       {
         OCR1A = (s1_100+100);
         if (pause(eventTimeS1, 800))
         {
           OCR1A = s1_110;
           tempS1 += 1;
           eventTimeS1 = millis();
           if (tempS1 == 3)
           {
             update_s1();
           }
         }
       }  
       if (pause(eventTimeS1, 400) && (eventNumS1 == 3))
       {
         update_s1();
        
       }
     if (pause(eventTimeS1, 30) && (eventNumS1 == 4))
       {
         OCR1A += 10;
         eventTimeS1 = millis();
         if (OCR1A >= s1_0)//check for final servo position to end behavior
         {
           OCR1A = s1_0;
           triggerS1 = 0;
         }
       } 
     }
    
    
    void behavior3_2(void)
    { 
     if (pause(timeS2, 200) && (eventNumS2 == 0))
       {
         OCR1B = s2_110;
         update_s2();
       }
        if (pause(eventTimeS2, 400) && (eventNumS2 == 1))
       {
         update_s2();
       }
     if (pause(eventTimeS2, 400) && (eventNumS2 == 2))
       {
         OCR1B = (s2_100+100);
         if (pause(eventTimeS2, 800))
         {
           OCR1B = s2_110;
           tempS2 += 1;
           eventTimeS2 = millis();
           if (tempS2 == 3)
           {
             update_s2();
           }
         }
       }
       if (pause(eventTimeS2, 400) && (eventNumS2 == 3))
       {
         update_s2();
       }
     if (pause(eventTimeS2, 30) && (eventNumS2 == 4))
       {
         OCR1B += 10;
         eventTimeS2 = millis();
         if (OCR1B >= s2_0)//check for final servo position to end behavior
         {
           OCR1B = s2_0;
           triggerS2 = 0;
         }
       } 
     }
    
    
    
    //*********************************************BEHAVIOR FOUR*****************************8
    //slow arm action to close, pause, slow retreat
    
    void behavior4_1(void)
    {
     
     if (pause(eventTimeS1, 30) && (eventNumS1 == 0))
       {
         OCR1A -= 10;
         eventTimeS1 = millis();
         if (OCR1A <= (s1_110))//check for final servo position to end behavior
         {
           OCR1A = (s1_110);
           update_s1();
         }
       } 
 
     if (pause(eventTimeS1, 600) && (eventNumS1 == 1))
       {
         update_s1();
       }
     if (pause(eventTimeS1, 30) && (eventNumS1 == 2))
       {
         OCR1A += 10;
         eventTimeS1 = millis();
         if (OCR1A >= s1_0)//check for final servo position to end behavior
         {
           OCR1A = s1_0;
           triggerS1 = 0;
         }
       } 
     }
    
     void behavior4_2(void)
    {
      if (pause(eventTimeS2, 30) && (eventNumS2 == 0))
       {
         OCR1B -= 10;
         eventTimeS2 = millis();
         if (OCR1B <= (s2_110))//check for final servo position to end behavior
         {
           OCR1B = (s2_110);
           update_s2();
         }
       } 
 
     if (pause(eventTimeS2, 600) && (eventNumS2 == 1))
       {
         update_s2();
       }
     if (pause(eventTimeS2, 30) && (eventNumS2 == 2))
       {
         OCR1B += 10;
         eventTimeS2 = millis();
         if (OCR1B >= s2_0)//check for final servo position to end behavior
         {
           OCR1B = s2_0;
           triggerS2 = 0;
         }
       } 
     }
    
    
    
    
    //*********************************BEHAVIOR FIVE **********************************
    // slow arm out, pause, slow retreat, pause, slow arm to switch, pause, slow retreat
    
     void behavior5_1(void)
    {
     if (pause(eventTimeS1, 30) && (eventNumS1 == 0))
       {
         OCR1A -= 10;
         eventTimeS1 = millis();
         if (OCR1A <= (s1_100))//check for final servo position to end behavior
         {
           OCR1A = (s1_100);
           update_s1();
         }
       } 
     if (pause(eventTimeS1, 600) && (eventNumS1 == 1))
       {
         update_s1();
       }
     if (pause(eventTimeS1, 30) && (eventNumS1 == 2))
       {
         OCR1A += 10;
         eventTimeS1 = millis();
         if (OCR1A >= s1_0-100)//check for final servo position to end behavior
         {
           OCR1A = s1_0-100;
           update_s1();
         }
       }
       if (pause(eventTimeS1, 600) && (eventNumS1 == 3))
       {
         update_s1();
       }
      if (pause(eventTimeS1, 30) && (eventNumS1 == 4))
       {
         OCR1A -= 10;
         eventTimeS1 = millis();
         if (OCR1A <= (s1_110))//check for final servo position to end behavior
         {
           OCR1A = (s1_110);
           update_s1();
         }
       } 
     if (pause(eventTimeS1, 800) && (eventNumS1 == 5))
       {
         update_s1();
       }
     if (pause(eventTimeS1, 30) && (eventNumS1 == 6))
       {
         OCR1A += 10;
         eventTimeS1 = millis();
         if (OCR1A >= s1_0)//check for final servo position to end behavior
         {
           OCR1A = s1_0;
           triggerS1 = 0;
         }
       } 
     }
    
    
    
    void behavior5_2(void)
    {
    if (pause(eventTimeS2, 30) && (eventNumS2 == 0))
       {
         OCR1B -= 10;
         eventTimeS2 = millis();
         if (OCR1B <= (s2_100))//check for final servo position to end behavior
         {
           OCR1B = (s2_100);
           update_s2();
         }
       } 
     if (pause(eventTimeS2, 600) && (eventNumS2 == 1))
       {
         update_s2();
       }
     if (pause(eventTimeS2, 30) && (eventNumS2 == 2))
       {
         OCR1B += 10;
         eventTimeS2 = millis();
         if (OCR1B >= s2_0-100)//check for final servo position to end behavior
         {
           OCR1B = s2_0-100;
           update_s2();
         }
       }
       if (pause(eventTimeS2, 600) && (eventNumS2 == 3))
       {
         update_s2();
       }
      if (pause(eventTimeS2, 30) && (eventNumS2 == 4))
       {
         OCR1B -= 10;
         eventTimeS2 = millis();
         if (OCR1B <= (s2_110))//check for final servo position to end behavior
         {
           OCR1B = (s2_110);
           update_s2();
         }
       } 
     if (pause(eventTimeS2, 800) && (eventNumS2 == 5))
       {
         update_s2();
       }
     if (pause(eventTimeS2, 30) && (eventNumS2 == 6))
       {
         OCR1B += 10;
         eventTimeS2 = millis();
         if (OCR1B >= s2_0)//check for final servo position to end behavior
         {
           OCR1B = s2_0;
           triggerS2 = 0;
         }
       } 
    }
    
    
    //*****************************************8BEHAVIOR SIX*************************************
    //PAUSE,FAST ARM ACTION, FAST CLOSE 
  void behavior6_1(void)
  {
    if (pause(timeS1, 2000) && (eventNumS1 == 0))
       {
         update_s1();
       }
     if (pause(eventTimeS1, 200) && (eventNumS1 == 1))
       {
           update_s1();
           OCR1A = s1_110;
       }
     if (pause(eventTimeS1, 200) && (eventNumS1 == 2))
       {
           OCR1A = s1_0;
           triggerS1 = 0;
       } 
   }
   
   void behavior6_2(void)
  {
    if (pause(timeS2, 2000) && (eventNumS2 == 0))
       {
         update_s2();
       }
     if (pause(eventTimeS2, 200) && (eventNumS2 == 1))
       {
           update_s2();
           OCR1B = s2_110;
       }
     if (pause(eventTimeS2, 200) && (eventNumS2 == 2))
       {
           OCR1B = s2_0;
           triggerS2 = 0;
       }
  }
   
   
   
   
   
   //***************************************BEHAVIOR SEVEN********************************88
    // Behaviour 7: Fast arm out, pause, slow action, pause, fast retreat.
   
       void behavior7_1(void)
       {
          if (pause(timeS1, 200) && (eventNumS1 == 0))
           {
               update_s1();
               OCR1A = (s1_100+50);
           }
        if (pause(eventTimeS1, 1500) && (eventNumS1 == 1))
           {
               update_s1();
           }
        
        if (pause(eventTimeS1, 30) && (eventNumS1 == 2))
           {
               OCR1A -= 10;
               eventTimeS1 = millis();
             if (OCR1A <= (s1_110))//check for final servo position to end behavior
               {
                   OCR1A = (s1_110);
                   update_s1();
               }
           } 
         if (pause(eventTimeS1, 1000) && (eventNumS1 == 3))
           {
               OCR1A = s1_0;
               triggerS1 = 0;
           } 
       }
   
   
   void behavior7_2(void)
       {
         if (pause(timeS2, 200) && (eventNumS2 == 0))
           {
               update_s2();
               OCR1B = (s2_100+50);
           }
        if (pause(eventTimeS2, 1500) && (eventNumS2 == 1))
           {
               update_s2();
           }
        
        if (pause(eventTimeS2, 30) && (eventNumS2 == 2))
           {
               OCR1B -= 10;
               eventTimeS2 = millis();
             if (OCR1B <= (s2_110))//check for final servo position to end behavior
               {
                   OCR1B = (s2_110);
                   update_s2();
               }
           } 
         if (pause(eventTimeS2, 1000) && (eventNumS2 == 3))
           {
               OCR1B = s2_0;
               triggerS2 = 0;
           } 
       }
   
   
   
   
   
   
   
   
   
    
    
 // __________________________SETUP FUNCTIONS_____________________________   
    
    
void ADC_init(void) //ADC Set Up
{
  ADCSRA = 0; //clear old settings
  ADCSRB = 0; //ADC0 source, single conversion mode and clear old settings
  ADMUX = 0; // clear old settings and select ADC0
  ADCSRA = (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); // set ADC prescaler to 128
  ADMUX = (1<<REFS0); //use internal Vcc AREF, remember to ensure capacitor is attached to AREF pin
  ADMUX |= (1<<MUX0); //select ADC1
  ADCSRA |= (1<<ADATE); //Auto Trigger
  ADCSRA |= (1<<ADEN); //enable ADC
  ADCSRA |= (1<<ADSC);
  delay(20);
}

void PWM_init(void)
{
  TCCR1A = 0; // clear TCCR1A
  TCCR1B = 0; // clear TCCR1B
  TCCR1A |= (1<<COM1A1)|(1<<COM1B1); // Channel A and B - Clear on up count, set on down count
  TCCR1B |= (1<<WGM13)|(1<<CS11); //PWM Phase and frequency correct, ICR1 set to top value and set prescaler to 8
}

void interrupt_init(void)
{
  EICRA |= (1<<ISC11);
  EICRA |= (1<<ISC01);// set int0 and int1 to trigger on rising edge part 1
  EICRA |= (1<<ISC00); // part 2
  EICRA |= (1<<ISC10); //part 3
  EIMSK |= (1<<INT0)|(1<<INT1); //enable interrupts
  sei(); // enable interrupts globally

}


