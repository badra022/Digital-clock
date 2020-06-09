/*
 *  Application.c
 *  Facebook username : ahmed mohamed
 *  Full name : ahmed mohamed mohamed
 *  Created on: Dec 7, 2019
 *  Author: Ahmed badra
 */

#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

/*global time variables*/
unsigned char Seconds;
unsigned char Minutes;
unsigned char Hours;

/*timer1
 *FOC1A & FOC1B set to one
 *init TCNT1
 *set value in ICR1 to compare
 *WGM13 & WGM12 set to 1 for CTC mode
 *CS10 & CS11 set to 1 for prescaler 64
 *Fcpu = 1 Mhz
 *for 1 sec need to interrupt at 15625 count
 *in TIMSK -> OCIE1A set to 1 to enable Timer1 compare match interrupt A
 *enable Global interrupt
 *voila timer1 is counting and waiting for flag to interrupt it in 1 sec
 */

void Timer1_CTC_init(void)
{
	/*function definition*/
	TCCR1A = (1<<FOC1A) | (1<<FOC1B);
	TCCR1B |= (1<<WGM13) | (1<<WGM12) | (1<<CS10) | (1<<CS11);
	TIMSK |= (1<<OCIE1A);
	TCNT1 = 0;
	/*counting for 15624 for 1 sec delay*/
	ICR1 = 15624;
}

/*
 * Timer0
 * CTC Mode operation
 * OCR0 = 78 for 5 msec count
 * at Fcpu = 1 Mhz
 * TCNT0 = 0
 * FOC0 set to 1 for non-PWM
 * WGM01 ar TCCR0 set to 1 for CTC
 * CS00 & CS01 both are 1's for 64 prescaler
 * at TIMSK -> OCIE0 set to 1 for compare match interrupt enable
 */

void Timer0_CTC_init(void)
{
	/*function definition*/
	TCCR0 = (1<<FOC0) | (1<<WGM01) | (1<<CS00) | (1<<CS01);
	TIMSK = (1<<OCIE0);
	TCNT0 = 0;
	OCR0 = 78;
}

void INT0_init(void)
{
	/*function definition*/
	GICR |= (1<<6);
	MCUCR |= (1<<ISC01);
	DDRD &=~(1<<2);
	PORTD |= (1<<2);
}

void INT1_init(void)
{
	/*function definition*/
	GICR |= (1<<7);
	MCUCR |= (1<<ISC11);
	DDRD &=~(1<<3);
	PORTD |= (1<<3);
}

void INT2_init(void)
{
	/*function definition*/
	GICR |= (1<<5);
	MCUCSR |= (1<<ISC2);
	DDRB &=~(1<<2);
	PORTB |= (1<<2);
}

int main(void)
{
	/*initilization code*/
	/*setting PA0 to PA5 & PC0 to PC3 o\p pins*/
	DDRA |= 0x3F;
	DDRC |= 0x1F;
	/*init the pins*/
	PORTC &=0xF0;
	PORTC |=(1<<PC4);
	PORTA &=0xC0;
	INT0_init();
	INT1_init();
	INT2_init();
	Timer0_CTC_init();
	Timer1_CTC_init();
	/*global interrupt enable*/
	SREG |= (1<<7);
	/*starting incrementing timer0 for seconds , minutes , hours*/
	TCCR1B |= (1<<CS10) | (1<<CS11);
	/*start with variables ((((((((((((edit here)))))))))))*/
	Seconds = 0;
	Minutes = 0;
	Hours = 0;
	/*------------------------------------------------*/
	/*super loop*/
	while(1)
	{
		/*Application code*/
	}
}

//--------------------------------------------------
/*interrupt service routines*/
//--------------------------------------------------

ISR(INT2_vect)
{
	/*interrupt service routine of external INT0*/
	/* continue the counting */
	TCCR1B |= (1<<CS10) | (1<<CS11);
}
ISR(INT1_vect)
{
	/*interrupt service routine of external INT0*/
	/*stopping the counting */
	TCCR1B &= ~((1<<CS10) | (1<<CS11));
}
ISR(INT0_vect)
{
	/*interrupt service routine of external INT0*/
	/*initiate the clock */
	Seconds = 0;
	Minutes = 0;
	Hours   = 0;
}
ISR(TIMER0_COMP_vect)
{
	/*interrupt service routine for timer0 interrupt*/
	/*each 5 msec Timer0 display current time variables for 3 milliseconds*/
	for(int i =0 ; i<6 ; i++)
	{
		/*enabling the displaying 7-segment*/
		PORTA = (PORTA &0xC0) | (1<<i);
		if(i == 0) PORTC = ((Seconds % 10)&0x0F) | (PORTC &0xF0);
		if(i == 1) PORTC = ((Seconds / 10)&0x0F) | (PORTC &0xF0);
		if(i == 2) PORTC = ((Minutes % 10)&0x0F) | (PORTC &0xF0);
		if(i == 3) PORTC = ((Minutes / 10)&0x0F) | (PORTC &0xF0);
		if(i == 4) PORTC = ((Hours % 10)&0x0F) | (PORTC &0xF0);
		if(i == 5) PORTC = ((Hours / 10)&0x0F) | (PORTC &0xF0);
		/*delay to be able to see the display*/
		_delay_ms(3);
	}
}
ISR(TIMER1_COMPA_vect)
{
	/*interrupt servive routine of TIMER1 interrupt*/
	/*each 1 Sec TIMER1 update time Variables*/
	if(Seconds == 59)
	{
		Seconds = 0;
		if(Minutes == 59)
		{
			Minutes = 0;
			if(Hours == 23)
			{
				Hours = 0;
			}
			else
			{
				Hours++;
			}
		}
		else
		{
				Minutes++;
		}
	}
	else
	{
		Seconds++;
	}
}
