#include <stdio.h>
#include <mega16.h>
#include <delay.h>
#include <alcd.h>

enum {S0,S1,S2,S11,S12,S21,S22} state=S0;
int temp=0 , threshold =27 , speed = 100;
int row=0,column=-1,position=0;       
char St[30];
flash char shift[4]={0xFE,0xFD,0xFB,0xF7};
flash char layout[16]={'7','8','9','/',
                       '4','5','6','*',
                       '1','2','3','-',
                       'C','0','=','+'};
char keypad(void);
char ch;
char keypad(void);
//int row , column=10, position=0 ;

void Statemachine(char input)
{
    switch(state)
    {
        case S0:
            if(input == '1') state=S1;
            if(input == '2') state=S2;
        break;
        case S1:
            if(input == 'C') state=S0;
            if(input == '+'){state=S11; speed = speed +10;} 
            if(input == '-'){state=S12; speed = speed -10;}
        break;
        case S11:
            if(input == 'C') state=S0;
            if(input == '+'){state=S11; speed = speed +10;} 
            if(input == '-'){state=S12; speed = speed -10;}
        break;  
        case S12:
            if(input == 'C'){state=S0;}
            if(input == '+'){state=S11; speed = speed +10;} 
            if(input == '-'){state=S12; speed = speed -10;}
        break;
        case S2:
            if(input == 'C'){state=S0;}
            if(input == '+'){state=S21; threshold = threshold +1;} 
            if(input == '-'){state=S22; threshold = threshold -1;}
        break;
        case S21:
            if(input == 'C'){state=S0;}
            if(input == '+'){state=S21; threshold = threshold +1;} 
            if(input == '-'){state=S22; threshold = threshold -1;}
        break;  
        case S22:
            if(input == 'C'){state=S0;}
            if(input == '+'){state=S21; threshold = threshold +1;} 
            if(input == '-'){state=S22; threshold = threshold -1;}
        break;         
    }    
    }


// External Interrupt 2 service routine
interrupt [EXT_INT2] void ext_int2_isr(void)
{
    ch = keypad();
    PORTD=0XF0; 
    Statemachine(ch); 
}

// Voltage Reference: AREF pin
#define ADC_VREF_TYPE ((0<<REFS1) | (0<<REFS0) | (0<<ADLAR))

unsigned int read_adc(unsigned char adc_input)
{
ADMUX=adc_input | ADC_VREF_TYPE;
// Delay needed for the stabilization of the ADC input voltage
delay_us(10);
// Start the AD conversion
ADCSRA|=(1<<ADSC);
// Wait for the AD conversion to complete
while ((ADCSRA & (1<<ADIF))==0);
ADCSRA|=(1<<ADIF);
return ADCW;
}

void main(void)
{

DDRA=(0<<DDA7) | (0<<DDA6) | (0<<DDA5) | (0<<DDA4) | (0<<DDA3) | (0<<DDA2) | (0<<DDA1) | (0<<DDA0);
PORTA=(0<<PORTA7) | (0<<PORTA6) | (0<<PORTA5) | (0<<PORTA4) | (0<<PORTA3) | (0<<PORTA2) | (0<<PORTA1) | (0<<PORTA0);
DDRB=(0<<DDB7) | (0<<DDB6) | (0<<DDB5) | (0<<DDB4) | (1<<DDB3) | (0<<DDB2) | (0<<DDB1) | (0<<DDB0);
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);
DDRC=(0<<DDC7) | (0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0); 
DDRD=(0<<DDD7) | (0<<DDD6) | (0<<DDD5) | (0<<DDD4) | (1<<DDD3) | (1<<DDD2) | (1<<DDD1) | (1<<DDD0);
PORTD=(1<<PORTD7) | (1<<PORTD6) | (1<<PORTD5) | (1<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 125.000 kHz
// Mode: Fast PWM top=0xFF
// OC0 output: Non-Inverted PWM
// Timer Period: 2.048 ms
// Output Pulse(s):
// OC0 Period: 2.048 ms Width: 0 us
TCCR0=(1<<WGM00) | (1<<COM01) | (0<<COM00) | (1<<WGM01) | (0<<CS02) | (1<<CS01) | (0<<CS00);
TCNT0=0x00;
OCR0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer1 Stopped
// Mode: Normal top=0xFFFF
// OC1A output: Disconnected
// OC1B output: Disconnected
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (0<<CS10);
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer2 Stopped
// Mode: Normal top=0xFF
// OC2 output: Disconnected
ASSR=0<<AS2;
TCCR2=(0<<PWM2) | (0<<COM21) | (0<<COM20) | (0<<CTC2) | (0<<CS22) | (0<<CS21) | (0<<CS20);
TCNT2=0x00;
OCR2=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (0<<OCIE0) | (0<<TOIE0);

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// INT2: On
// INT2 Mode: Falling Edge
GICR|=(0<<INT1) | (0<<INT0) | (1<<INT2);
MCUCR=(0<<ISC11) | (0<<ISC10) | (0<<ISC01) | (0<<ISC00);
MCUCSR=(0<<ISC2);
GIFR=(0<<INTF1) | (0<<INTF0) | (1<<INTF2);


ACSR=(1<<ACD) | (0<<ACBG) | (0<<ACO) | (0<<ACI) | (0<<ACIE) | (0<<ACIC) | (0<<ACIS1) | (0<<ACIS0);
ADMUX=ADC_VREF_TYPE;
ADCSRA=(1<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (0<<ADIE) | (0<<ADPS2) | (0<<ADPS1) | (1<<ADPS0);
SFIOR=(0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);

TWCR=(0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWEN) | (0<<TWIE);
lcd_init(20);

// Global enable interrupts
#asm("sei")
while (1)
      {
      temp = read_adc(3)/2.05;
      sprintf(St," Temp : %d",temp);   
      lcd_gotoxy(0,3);
      lcd_puts(St);   
      delay_ms(200);
      if (temp >= threshold){OCR0 = speed;}
      if (temp < threshold){OCR0 = 0;}  
      
      switch (state)
      {
            case S0 :
            lcd_clear(); 
            lcd_gotoxy(0,0);
            lcd_puts(" 1 : Speed");
            lcd_gotoxy(0,1);
            lcd_puts(" 2 : Threshold");
            break;
            case S1 :
            lcd_clear(); 
            lcd_gotoxy(0,0);
            lcd_puts(" + : Speed Up");
            lcd_gotoxy(0,1);
            lcd_puts(" - : Speed Down");
            lcd_gotoxy(0,2);
            sprintf(St,"Speed : %d                " , OCR0);
            lcd_puts(St);
            break;
            case S2 :   
            lcd_clear();
            lcd_gotoxy(0,0);
            lcd_puts(" + : Threshold Up");
            lcd_gotoxy(0,1);
            lcd_puts(" - : Threshold Down");
            lcd_gotoxy(0,2);
            sprintf(St,"Threshold : %d                " , threshold);
            lcd_puts(St);
            break;
            case S11 :  
            lcd_clear();
            lcd_gotoxy(0,0);
            lcd_puts(" + : Speed Up");
            lcd_gotoxy(0,1);
            lcd_puts(" - : Speed Down");
            lcd_gotoxy(0,2);
            sprintf(St,"Speed : %d                " , OCR0);
            lcd_puts(St);
            break;
            case S12 :  
            lcd_clear();
            lcd_gotoxy(0,0);
            lcd_puts(" + : Speed Up");
            lcd_gotoxy(0,1);
            lcd_puts(" - : Speed Down");
            lcd_gotoxy(0,2);
            sprintf(St,"Speed : %d                " , OCR0);
            lcd_puts(St);
            break;
            case S21 :  
            lcd_clear();
            lcd_gotoxy(0,0);
            lcd_puts(" + : Threshold Up");
            lcd_gotoxy(0,1);
            lcd_puts(" - : Threshold Down");
            lcd_gotoxy(0,2);
            sprintf(St,"Threshold : %d                " , threshold);
            lcd_puts(St);
            break;
            case S22 :  
            lcd_clear();
            lcd_gotoxy(0,0);
            lcd_puts(" + : Threshold Up");
            lcd_gotoxy(0,1);
            lcd_puts(" - : Threshold Down");
            lcd_gotoxy(0,2);
            sprintf(St,"Threshold : %d                " , threshold);
            lcd_puts(St);
            break;
      }

      }
}
char keypad(void)
{
 for (row=0;row<4;row++)
    {
    PORTD = shift[row];
    if(PIND.4==0){column=0;}
    if(PIND.5==0){column=1;}
    if(PIND.6==0){column=2;}
    if(PIND.7==0){column=3;}
    if(column !=-1){ 
        position = row*4 + column;
        //lcd_putchar(layout[position]);
        column=-1;
        while(PIND.4==0){}
        while(PIND.5==0){}
        while(PIND.6==0){}
        while(PIND.7==0){} 
        return layout[position];
    }
    delay_ms(50);
 
    }

}
