

#include "msp.h"
#include <stdio.h>
enum states {MAIN_MENU_PRINT , MAIN_MENU, DOOR_PRINT, DOOR , MOTOR_PRINT , MOTOR, LIGHTS_PRINT, LIGHTS_DUTY_CYCLE_RED, LIGHTS_DUTY_CYCLE_GREEN, LIGHTS_DUTY_CYCLE_BLUE, LIGHTS, RED_LED, GREEN_LED, BLUE_LED};



/**
 * Prelab 7
 * Gabrielle Green
 * EGR 226 902
 *
 * The purpose of this code is to read an input from the keypad as a percent value,
 * convert this value to a decimal, and then use that value to set the pwm
 * for the motor using the systick timer.
 */

void TimerA_Init_DC(float speed3);
volatile int duty_cycle = 30000; // period * 0.4
volatile int period1 = 75000; //(1/50 Hz)*3000000 = clocks:maximum
volatile int period2 = 7499;
void Keypad_percent(void);

void Initialize_Pins(); //function to initialize pins

void SysTick_Init(); //function to initialize systick timer
int Pin_Number(); //function to put pin number in string
int Read_Keypad(); //function to read in number from keypad
void SysTick_Delay(uint16_t delayms); //function to use systick timer to delay in ms
float Control_Speed(int percent3);

void LCD_main(void);
void LCD_door(void);
void LCD_init(void); //function to initialize LCD
void delay_micro(unsigned microsec); //generate delay in microseconds
void delay_ms(unsigned ms); //generate delay in milliseconds
void PulseEnablePin(void); //pulse enable
void pushNibble(uint8_t nibble); //push nibble onto pins
void pushByte(uint8_t byte); //push byte onto pins
void commandWrite(uint8_t command); //send command to LCD
void dataWrite(uint8_t data); //send data to LCD

void TimerA_Init_SERVO(int i);

int Input_Number();
void LCD_duty_cycle(void);

void LCD_light();
float Keypad_percent1(void);
void TimerA_Init_RED(float speed3);
void TimerA_Init_GREEN(float speed3);
void TimerA_Init_BLUE(float speed3);
float Control_Speed2(int percent3);


void main(void){
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    enum states state = MAIN_MENU_PRINT;
    int NUM = 0;
    int j;
    float percent_value;

    Initialize_Pins();
    SysTick_Init();



        while(1)
        {
    switch(state)
    {
    case MAIN_MENU_PRINT:

            LCD_main();
            state = MAIN_MENU;
            break;


    case MAIN_MENU:
            NUM = Input_Number();


            if (NUM == 1)
            {
               state = DOOR_PRINT;
            }

            if (NUM == 2)
            {
                state = MOTOR_PRINT;
            }

            if (NUM==3)
            {
                state = LIGHTS_PRINT;
            }

            break;
   case DOOR_PRINT:
            LCD_door();
            state = DOOR;
            break;
   case DOOR:
       NUM = Input_Number();
       if (NUM == 1)
       {
           j = 2250;
           TimerA_Init_SERVO(j);
           P1 -> OUT |= BIT6;
           P1 -> OUT &= ~(BIT7);
           state = MAIN_MENU_PRINT;
       }

       if (NUM == 2)
       {
           j = 3000;
           TimerA_Init_SERVO(j);
           P1 -> OUT |= BIT7;
           P1 -> OUT &= ~(BIT6);
           state = MAIN_MENU_PRINT;
       }
       break;
    case MOTOR_PRINT:
            LCD_duty_cycle();
            state = MOTOR;
            break;
    case MOTOR:
            Keypad_percent();
            state = MAIN_MENU_PRINT;
            break;
     case LIGHTS_PRINT:
         LCD_light();
         state = LIGHTS;
            break;
     case LIGHTS_DUTY_CYCLE_RED:
         LCD_duty_cycle();
         state = RED_LED;
         break;
     case LIGHTS_DUTY_CYCLE_GREEN:
         LCD_duty_cycle();
         state = GREEN_LED;
         break;
     case LIGHTS_DUTY_CYCLE_BLUE:
         LCD_duty_cycle();
         state = BLUE_LED;
         break;

     case LIGHTS:
         NUM = Input_Number();
                    if (NUM == 1)
                    {
                       state = LIGHTS_DUTY_CYCLE_RED;
                    }

                    if (NUM == 2)
                    {
                        state = LIGHTS_DUTY_CYCLE_GREEN;
                    }

                    if (NUM==3)
                    {
                        state = LIGHTS_DUTY_CYCLE_BLUE;
                    }
         break;
     case RED_LED:
         percent_value = Keypad_percent1();
         TimerA_Init_RED(percent_value);
         state = MAIN_MENU_PRINT;
         break;
     case GREEN_LED:
         percent_value = Keypad_percent1();
         TimerA_Init_GREEN(percent_value);
         state = MAIN_MENU_PRINT;
         break;
     case BLUE_LED:
         percent_value = Keypad_percent1();
         TimerA_Init_BLUE(percent_value);
         state = MAIN_MENU_PRINT;
         break;
    }

        }
}

void TimerA_Init_DC(float speed3)
{
    int speed4;

    //initializes P2.7 to be used for Timer A
    P6->SEL0 |= BIT6; //set as Timer A
    P6->SEL1 &= ~(BIT6); //set as Timer A
    P6->DIR |= BIT6; //set as output

    speed4 = speed3;



    TIMER_A2->CCR[0] = period1;
    TIMER_A2->CCR[3] = speed4;

    TIMER_A2->CCTL[3] = 0b0000000011100000;
    TIMER_A2->CTL = 0b0000001001010100;
}

//purpose of function is to initialize pins
void Initialize_Pins()
{
    //intializes all pins in port 4 to be used for keypad
     P4->SEL0 &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6); //Port 4 GPIO
     P4->SEL1 &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6); //Port 4 GPIO
     P4->DIR &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6); //Set as inputs
     P4->REN |= (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6); //enable resistor
     P4->OUT |= (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6); //drive to 1

    //Initialize LCD Screen

     //Initialize P3.5 RS
     P6->SEL0 &= ~(BIT1); //sets up P4.0 GPIO
     P6->SEL1 &= ~(BIT1); //sets up P4.0 as GPIO
     P6->DIR |= BIT1; //sets up P4.0 as an output

     //Initialize P3.0 E
     P6->SEL0 &= ~(BIT0); //sets up P4.1 GPIO
     P6->SEL1 &= ~(BIT0); //sets up P4.1 as GPIO
     P6->DIR |= BIT0; //sets up P4.1 as an output

     //Initialize P5.0 DB4
     P5->SEL0 &= ~(BIT4); //sets up P4.4 GPIO
     P5->SEL1 &= ~(BIT4); //sets up P4.4 as GPIO
     P5->DIR |= BIT4; //sets up P4.4 as an output

     //Initialize P5.1 DB5
     P5->SEL0 &= ~(BIT5); //sets up P4.5 GPIO
     P5->SEL1 &= ~(BIT5); //sets up P4.5 as GPIO
     P5->DIR |= BIT5; //sets up P4.5 as an output

     //Initialize P5.2 DB6
     P5->SEL0 &= ~(BIT6); //sets up P4.6 GPIO
     P5->SEL1 &= ~(BIT6); //sets up P4.6 as GPIO
     P5->DIR |= BIT6; //sets up P4.6 as an output

     //Initialize P5.4 DB7
     P5->SEL0 &= ~(BIT7); //sets up P4.7 GPIO
     P5->SEL1 &= ~(BIT7); //sets up P4.7 as GPIO
     P5->DIR |= BIT7; //sets up P4.7 as an output

     //LED pins
     P1->SEL0 &= ~(BIT6 | BIT7); //sets up P4.7 GPIO
     P1->SEL1 &= ~(BIT6 | BIT7); //sets up P4.7 as GPIO
     P1->DIR |= (BIT6 | BIT7); //sets up P4.7 as an output
     P2->REN |= (BIT6 | BIT7);
     P2->OUT &= ~(BIT6 | BIT7);

}

//purpose of function is to read in value pressed on keypad

int Read_Keypad(void)
{
    uint8_t row_bits = 0x00;
    int num = -1;
    int i;

    for(i=4;i<=6;i++) //loop through columns on P4.4, P4.5, P4.6
    {
        P4->DIR |= (0x01 << i); //sets current column as output
        P4->OUT &= ~(0x01 << i); //sets current column output to 0
        SysTick_Delay(20);

        row_bits = P4->IN & 0x0F; //reads all rows

           if (row_bits != 0x0F) //if a button is pressed (not equal to 1111)
           {
               SysTick_Delay(20);



                   if(row_bits == 0x0E) // if row 0 is pressed then row_bits = 1110
                   {
                       num = ((i-4)+1);
                   }
                   if(row_bits == 0x0D) // if row 1 is pressed then row_bits = 1101
                   {
                       num = ((i-4)+1)+(1*3);
                   }
                   if(row_bits == 0x0B) // if row 2 is pressed then row_bits = 1011
                   {
                       num = ((i-4)+1)+(2*3);
                   }
                   if(row_bits == 0x07) // if row e is pressed then row_bits = 0111
                   {
                       num = ((i-4)+1)+(3*3);
                   }

           }
           while(((P4->IN)& 0x0F) != 0x0F){} //stays in loop until button is pressed
           P4->OUT |= (0x01 << i); //drive columns to high
           P4->DIR &= ~(0x01 << i); //set columns back to inputs

    }

    return (num);
}
 //purpose of function is to put numbers pressed on keypad into string, returns as integer
int Pin_Number()
{

    int three_digit_percent[4] = {0,0,0,'\0'}; //initializes string
    int i;
    int temp=0;
    int percent = 0;
    char percent1[4];

  //  printf("\nEnter a 3 digit percent number:\n");

    while(temp != 12)
{

        temp = Read_Keypad(); //saves button pressed



      if(temp !=(-1) && temp != (12))
      {
          if (temp == 11)
          {
              temp = 0;
          }
             three_digit_percent[2]=three_digit_percent[1];
             three_digit_percent[1]=three_digit_percent[0];
             three_digit_percent[0]=temp; //saves numbers pressed into character string
      }



}



   percent = ((three_digit_percent[2])*100)+((three_digit_percent[1])*10)+(three_digit_percent[0]); //saves percent as integer

   sprintf(percent1, "%d", percent);

   delay_ms(100);
   commandWrite(0x96); //To set DDRAM address, DB7 = 1, DB6-DB0 = address, which is 0x40: 0100 0000, so set to 1100 0000
   delay_ms(100);

   if(percent == 100)
   {
       for(i=0;i<3;i++)
       {
           delay_ms(100);
           dataWrite(percent1[i]);
           delay_ms(100);
       }

       delay_ms(100);
       dataWrite('%');
       delay_ms(100);
   }
   else
   {
   for(i=0;i<2;i++)
   {
       delay_ms(100);
       dataWrite(percent1[i]);
       delay_ms(100);
   }
   delay_ms(100);
   dataWrite('%');
   delay_ms(100);
   }

   if(percent > 100)
   {
       printf("\nPlease enter a value between 1-100");
       return(0);
   }



return (percent); //returns percent as integer
}

//purpose of function is to delay a specific number of ms
void SysTick_Delay(uint16_t delayms)
{


    SysTick->LOAD = ((delayms*3000)-1);
    SysTick->VAL = 0;


    while((SysTick->CTRL & 0x00010000) == 0); //Bit 16 means complete

}

//purpose of function is to initialize systick timer
void SysTick_Init()
{
    SysTick->CTRL=0x00; //counter off
    SysTick->VAL=0x00; //decimal 3000 (1 ms)
    SysTick->LOAD=0x00; //Resets the counter
    SysTick->CTRL=0x05; //turns counter on
}

float Control_Speed(int percent3)
{

    float percent2;
    float speed2;

        percent2 = percent3;

        delay_ms(100);

        percent2 = percent2 / 100;

        delay_ms(100);

        speed2 = period1 * percent2;

        delay_ms(100);

        return(speed2);

}
float Control_Speed2(int percent3)
{

    float percent2;
    float speed2;

        percent2 = percent3;

        delay_ms(100);

        percent2 = percent2 / 100;

        delay_ms(100);

        speed2 = period2 * percent2;

        delay_ms(100);

        return(speed2);

}

/*The purpose of this function is to initialize the LCD by going through the
 * initialization sequence outlined in Figure 4 of the prelab
 *
 * From figure 4 given in prelab
 */

void LCD_init(void)
{

    commandWrite(3);
    delay_ms(100);
    commandWrite(3);
    delay_micro(200);
    commandWrite(3);
    delay_ms(100);

    commandWrite(2);
    delay_micro(100);
    commandWrite(2);
    delay_micro(100);

    commandWrite(8);
    delay_micro(100);
    commandWrite(0x0F);
    delay_micro(100);
    commandWrite(1);
    delay_micro(100);
    commandWrite(6);
    delay_ms(10);

}

//The purpose of this function is to delay the desired number of microseconds

void delay_micro(unsigned microsec)
{
    SysTick->LOAD = ((microsec*3)-1);
    SysTick->VAL = 0;

    while((SysTick->CTRL & 0x00010000) == 0); //Bit 16 means complete
}

//The purpose of this function is to delay the desired number of milliseconds

void delay_ms(unsigned ms)
{
    SysTick->LOAD = ((ms*3000)-1);
    SysTick->VAL = 0;

    while((SysTick->CTRL & 0x00010000) == 0); //Bit 16 means complete
}


/*The purpose of this function is to pulse the enable so that bits
 * can be pushed to pins
 *
 * From flowchart (figure 6) given in prelab
 */

void PulseEnablePin(void)
{
    P6->OUT &= ~(BIT0); //Set E = 0
    delay_micro(10); //delay 10 microseconds
    P6->OUT |= BIT0; //Set E = 1
    delay_micro(10); //delay 10 microseconds
    P6->OUT &= ~(BIT0); //Set E = 0
    delay_micro(10); //delay 10 microseconds
}

/*The purpose of this function is to push a nibble (4 bits)
 * onto DB4-DB7
 *
 * From flowchart (figure 5) given in prelab
 */

void pushNibble(uint8_t nibble)
{
    P5-> OUT &= ~(BIT4); //clear bits DB4-DB7
    P5-> OUT &= ~(BIT5);
    P5-> OUT &= ~(BIT6);
    P5-> OUT &= ~(BIT7);

    P5-> OUT |= ((nibble<< 4) & (BIT4 | BIT5 | BIT6 | BIT7)); //save nibble to pins
    PulseEnablePin(); //call pulse function so sent to LCD
}

/*The purpose of this function is to push a byte (8 bits)
 * onto DB4-DB7 using pushNibble()
 *
 * From flowchart (figure 5) given in prelab
 */

void pushByte(uint8_t byte)
{
    uint8_t byte_ = 0x00;

   //send most significant digits
    byte_ = ((byte & 0xF0) >> 4);
    pushNibble(byte_);

    //send least significant digits
    byte_ = (byte & 0x0F);
     pushNibble(byte_);
    delay_micro(100);
}

/*The purpose of this function is to write one byte of command using
 * the pushByte function with command parameter
 */

void commandWrite(uint8_t command)
{
    P6->OUT &= ~(BIT1); //Set RS to 0
    pushByte(command);
}

/*The purpose of this function is to write one byte of data using
 * the pushByte function with data parameter
 */

void dataWrite(uint8_t data)
{
    P6->OUT |= BIT1; //Set RS to 1
    pushByte(data);
}

void LCD_main(void)
{
    LCD_init();

        delay_ms(50);

        int i;


        char menu[] = "Menu";
        char door[] = "1. Door";
        char motor[] = "2. Motor";
        char lights[] = "3. Lights";

        delay_ms(100);
        commandWrite(0x86); //To set DDRAM address, DB7 = 1, DB6-DB0 = address, which is 0x40: 0100 0000, so set to 1100 0000
        delay_ms(100);

        for(i=0;i<4;i++)
        {
            delay_micro(100);
            dataWrite(menu[i]);
            delay_micro(100);
        }


        delay_ms(100);
        commandWrite(0xC0); //To set DDRAM address, DB7 = 1, DB6-DB0 = address, which is 0x40: 0100 0000, so set to 1100 0000
        delay_ms(100);

        for(i=0;i<7;i++)
        {
            delay_micro(100);
            dataWrite(door[i]);
            delay_micro(100);
        }

        delay_ms(100);
        commandWrite(0x90); //To set DDRAM address, DB7 = 1, DB6-DB0 = address, which is 0x40: 0100 0000, so set to 1100 0000
        delay_ms(100);

        for(i=0;i<8;i++)
        {
            delay_micro(100);
            dataWrite(motor[i]);
            delay_micro(100);
        }

        delay_ms(100);
        commandWrite(0xD0); //To set DDRAM address, DB7 = 1, DB6-DB0 = address, which is 0x40: 0100 0000, so set to 1100 0000
        delay_ms(100);

        for(i=0;i<9;i++)
        {
            delay_micro(100);
            dataWrite(lights[i]);
            delay_micro(100);
        }
}

void Keypad_percent(void)
{
    int percent5;
    float speed;

    delay_ms(100);
    percent5 = Pin_Number();

    printf("%d", percent5);
    delay_ms(100);
    speed = Control_Speed(percent5);

    printf("%f", speed);
    delay_ms(100);
    TimerA_Init_DC(speed);
}

void LCD_door(void)
{
   delay_ms(100);
    commandWrite(0x01);
   delay_ms(100);

    LCD_init();

    delay_ms(100);

        int i;


        char door_menu[] = "Door Menu";
        char opened[] = "1. Opened";
        char closed[] = "2. Closed";
        //char lights[] = "3. Lights";

        delay_ms(100);
        commandWrite(0x83); //To set DDRAM address, DB7 = 1, DB6-DB0 = address, which is 0x40: 0100 0000, so set to 1100 0000
        delay_ms(100);

        for(i=0;i<9;i++)
        {
            delay_micro(100);
            dataWrite(door_menu[i]);
            delay_micro(100);
        }


        delay_ms(100);
        commandWrite(0xC0); //To set DDRAM address, DB7 = 1, DB6-DB0 = address, which is 0x40: 0100 0000, so set to 1100 0000
        delay_ms(100);

        for(i=0;i<9;i++)
        {
            delay_micro(100);
            dataWrite(opened[i]);
            delay_micro(100);
        }

        delay_ms(100);
        commandWrite(0x90); //To set DDRAM address, DB7 = 1, DB6-DB0 = address, which is 0x40: 0100 0000, so set to 1100 0000
        delay_ms(100);

        for(i=0;i<9;i++)
        {
            delay_micro(100);
            dataWrite(closed[i]);
            delay_micro(100);
        }


}

int Input_Number()
{
    int input_number[2] = {0,'\0'}; //initializes string
        int i;
        int temp=0;
        int num1 = 0;

        while(temp != 12)
    {

            temp = Read_Keypad(); //saves button pressed



          if(temp !=(-1) && temp != (12))
          {
                 input_number[0]=temp; //saves numbers pressed into character string
          }

    }
       num1 = (input_number[0]); //saves percent as integer

       printf("%d", num1);


    return (num1); //returns percent as integer
}

void LCD_duty_cycle(void)
{
    LCD_init();

        delay_ms(50);

        int i;


        char enter[] = "Enter";
        char percent[] = " %";
        char duty[] = "Duty";
        char cycle[] = " Cycle:";

        delay_ms(100);
        commandWrite(0x84); //To set DDRAM address, DB7 = 1, DB6-DB0 = address, which is 0x40: 0100 0000, so set to 1100 0000
        delay_ms(100);

        for(i=0;i<5;i++)
        {
            delay_micro(100);
            dataWrite(enter[i]);
            delay_micro(100);
        }

        for(i=0;i<2;i++)
        {
            delay_micro(100);
            dataWrite(percent[i]);
            delay_micro(100);
        }

        delay_ms(100);
        commandWrite(0xC2); //To set DDRAM address, DB7 = 1, DB6-DB0 = address, which is 0x40: 0100 0000, so set to 1100 0000
        delay_ms(100);

        for(i=0;i<4;i++)
        {
            delay_micro(100);
            dataWrite(duty[i]);
            delay_micro(100);
        }

        for(i=0;i<7;i++)
        {
            delay_micro(100);
            dataWrite(cycle[i]);
            delay_micro(100);
        }
}

void LCD_light()
{
    LCD_init();

            delay_ms(50);

            int i;


            char red[] = "1. Red Light";
            char green[] = "2. Green Light";
            char blue[] = "3. Blue Light";
            char main[] = "Lights Menu";

            delay_ms(100);
            commandWrite(0x83); //To set DDRAM address, DB7 = 1, DB6-DB0 = address, which is 0x40: 0100 0000, so set to 1100 0000
            delay_ms(100);

            for(i=0;i<11;i++)
            {
                delay_micro(100);
                dataWrite(main[i]);
                delay_micro(100);
            }

            delay_ms(100);
            commandWrite(0xC0); //To set DDRAM address, DB7 = 1, DB6-DB0 = address, which is 0x40: 0100 0000, so set to 1100 0000
            delay_ms(100);

            for(i=0;i<12;i++)
            {
                delay_micro(100);
                dataWrite(red[i]);
                delay_micro(100);
            }

            delay_ms(100);
            commandWrite(0x90); //To set DDRAM address, DB7 = 1, DB6-DB0 = address, which is 0x40: 0100 0000, so set to 1100 0000
            delay_ms(100);

            for(i=0;i<14;i++)
            {
                delay_micro(100);
                dataWrite(green[i]);
                delay_micro(100);
            }

            delay_ms(100);
            commandWrite(0xD0); //To set DDRAM address, DB7 = 1, DB6-DB0 = address, which is 0x40: 0100 0000, so set to 1100 0000
            delay_ms(100);


            for(i=0;i<13;i++)
            {
                delay_micro(100);
                dataWrite(blue[i]);
                delay_micro(100);
            }

}

void TimerA_Init_SERVO(int i)
{

        //initializes P2.7 to be used for Timer A
        P2->SEL0 |= BIT7; //set as Timer A
        P2->SEL1 &= ~(BIT7); //set as Timer A
        P2->DIR |= BIT7; //set as output

        TIMER_A0->CCR[0] = 60000;
        TIMER_A0->CCR[4] = i;

        TIMER_A0->CCTL[4] = 0b0000000011100000;
        TIMER_A0->CTL = 0b0000001001010100;
}

float Keypad_percent1(void)
{
    int percent5;
    float speed;

    delay_ms(100);
    percent5 = Pin_Number();

    printf("%d", percent5);
    delay_ms(100);
    speed = Control_Speed2(percent5);

    printf("%f", speed);
    delay_ms(100);
   return(speed);
}

void TimerA_Init_RED(float speed3)
{
    int speed4;

    //initializes P2.7 to be used for Timer A
    P2->SEL0 |= BIT4; //set as Timer A
    P2->SEL1 &= ~(BIT4); //set as Timer A
    P2->DIR |= BIT4; //set as output

    speed4 = speed3;



    TIMER_A0->CCR[0] = period1;
    TIMER_A0->CCR[1] = speed4;

    TIMER_A0->CCTL[1] = 0b0000000011100000;
    TIMER_A0->CTL = 0b0000001001010100;
}
void TimerA_Init_GREEN(float speed3)
{
    int speed4;

    //initializes P2.7 to be used for Timer A
    P2->SEL0 |= BIT5; //set as Timer A
    P2->SEL1 &= ~(BIT5); //set as Timer A
    P2->DIR |= BIT5; //set as output

    speed4 = speed3;



    TIMER_A0->CCR[0] = period1;
    TIMER_A0->CCR[2] = speed4;

    TIMER_A0->CCTL[2] = 0b0000000011100000;
    TIMER_A0->CTL = 0b0000001001010100;
}
void TimerA_Init_BLUE(float speed3)
{
    int speed4;

    //initializes P2.7 to be used for Timer A
    P2->SEL0 |= BIT6; //set as Timer A
    P2->SEL1 &= ~(BIT6); //set as Timer A
    P2->DIR |= BIT6; //set as output

    speed4 = speed3;



    TIMER_A0->CCR[0] = period1;
    TIMER_A0->CCR[3] = speed4;

    TIMER_A0->CCTL[3] = 0b0000000011100000;
    TIMER_A0->CTL = 0b0000001001010100;
}
