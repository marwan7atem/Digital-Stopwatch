#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

unsigned char hours1=0,hours0=0,minutes1=0,minutes0=0,seconds1=0,seconds0=0;
unsigned char flag1=0,flag2=0,flag3=0,flag4=0,flag5=0,flag6=0;

void display()
{
	PORTA|=(1<<5);
	PORTC=(PORTC&0XF0)|(seconds0&0X0F);
	_delay_ms(2);
	PORTA&=~(1<<5);

	PORTA|=(1<<4);
	PORTC=(PORTC&0XF0)|(seconds1&0X0F);
	_delay_ms(2);
	PORTA&=~(1<<4);

	PORTA|=(1<<3);
	PORTC=(PORTC&0XF0)|(minutes0&0X0F);
	_delay_ms(2);
	PORTA&=~(1<<3);

	PORTA|=(1<<2);
	PORTC=(PORTC&0XF0)|(minutes1&0X0F);
	_delay_ms(2);
	PORTA&=~(1<<2);

	PORTA|=(1<<1);
	PORTC=(PORTC&0XF0)|(hours0&0X0F);
	_delay_ms(2);
	PORTA&=~(1<<1);

	PORTA|=(1<<0);
	PORTC=(PORTC&0XF0)|(hours1&0X0F);
	_delay_ms(2);
	PORTA&=~(1<<0);

}

void Timer1(void)
{
	TCCR1A=(1<<3);
	TCCR1B=(1<<3)|(1<<0)|(1<<2);
	TCNT1=0;
	OCR1A=15625;
	TIMSK|=(1<<4);
	SREG|=(1<<7);


}

ISR(TIMER1_COMPA_vect)
{
	                                       //Count Up Mode
	if(PORTD&(1<<4))
	{
	if(hours0==9)
		{
			hours1++;
			hours0=0;
			minutes0=0;
			minutes1=0;
			seconds0=0;
		    seconds1=0;
		}
	else if((minutes0==9&&minutes1==5)&&(seconds0==9&&seconds1==5))
		{
			hours0++;
			seconds0=0;
			seconds1=0;
			minutes0=0;

		}
	else if((minutes0==9&&minutes1!=5)&&(seconds0==9&&seconds1==5))
		{
			minutes1++;
			seconds0=0;
			seconds1=0;
			minutes0=0;
		}
	 else if(seconds0==9&&seconds1==5)
	{
		minutes0++;
		seconds0=0;
		seconds1=0;
	}
	else if(seconds0==9&&seconds1!=5)
	{
		seconds1++;
		seconds0=0;
	}


	else
	{
		seconds0++;
	}
	}
	                                   //Count Down Mode
	else if(PORTD&(1<<5))
	{
		if(seconds0!=0)
				{
					seconds0--;
				}
				else if(seconds1!=0)
				{
					seconds1--;
					seconds0=9;
				}
				else if(minutes0!=0)
				{
					minutes0--;
					seconds1=5;
				}
				else if(minutes1!=0)
				{
					minutes1--;
					minutes0=9;
				}
				else if(hours0!=0)
				{
					hours0--;
					minutes1=5;
				}
				else if(hours1!=0)
				{
					hours1--;
					hours0=9;
				}
				else if(seconds0==0&&seconds1==0&&minutes0==0&&minutes1==0&&hours0==0&&hours1==0)
				{
					seconds0=0;
					seconds1=0;
					minutes0=0;
					minutes1=0;
					hours0=0;
					hours1=0;
					PORTD|=(1<<0);
				}
				else
				{

				}
	}

}

void INT0_Init(void)
{
	DDRD&=~(1<<2);
	PORTD|=(1<<2);
	//Trigger INT0 with the falling edge
	MCUCR|=(1<<ISC01);
	MCUCR&=~(1<<ISC00);
	GICR|=(1<<INT0);  // Enable external interrupt pin INT0
	SREG|=(1<<7); //Enable interrupts by setting I-bit
}
ISR(INT0_vect)
{
	hours0=0;
	hours1=0;
	minutes0=0;
	minutes1=0;
	seconds0=0;
	seconds1=0;


}

void INT1_Init(void)
{
	DDRD&=~(1<<3);
	MCUCR|=(1<<ISC11)|(1<<ISC10); // Trigger INT1 with the raising edge
	GICR|=(1<<INT1); //Enable external interrupt pin INT1
	SREG|=(1<<7); //Enable interrupts by setting I-bit
}
ISR(INT1_vect)
{
	// stop the timer
	TCCR1B&=~(1<< CS10);
	TCCR1B&=~(1<< CS11);
	TCCR1B&=~(1<< CS12);
	PORTD&=~(1<<0); //To Stop The Buzzer Press on Pause
}

void INT2_Init(void)
{
	DDRB&=~(1<<2);
	PORTB|=(1<<2);
	MCUCSR &= ~(1<<ISC2); //Trigger INT2 with the falling edge
	GICR|=(1<<INT2); //Enable external interrupt pin INT2
	SREG|=(1<<7); //Enable interrupts by setting I-bit
}
ISR(INT2_vect)
{
	//resume the timer
	TCCR1B|=(1<<CS10);
	TCCR1B&=~(1<< CS11);
	TCCR1B|=(1<<CS12);
}

int main(void)
{
	DDRA|=0X3F;
	DDRC|=0X0F;
	DDRD|=0X31;
	DDRD&=0XC2;
	DDRB&=(1<<7);
	DDRB|=(1<<7);
	PORTB|=(1<<7);
	PORTD|=(1<<4);
	PORTD&=~(1<<5);
	DDRB&=~(1<<1);
	PORTB|=(1<<1);

	DDRB&=~(1<<0);
	PORTB|=(1<<0);

	DDRB&=~(1<<4);
	PORTB|=(1<<4);

	DDRB&=~(1<<3);
	PORTB|=(1<<3);

	DDRB&=~(1<<6);
	PORTB|=(1<<6);

	DDRB&=~(1<<5);
	PORTB|=(1<<5);

	Timer1();
	INT0_Init();
	INT1_Init();
	INT2_Init();

	while(1)
	{
		display();

		if((!(PINB & (1<<7)))&& (!(TCCR1B&(1<<0)))){
						_delay_ms(30);
						if((!(PINB & (1<<7)))&& (!(TCCR1B&(1<<0)))){
								   PORTD ^=(1<<5);
								    PORTD ^=(1<<4);
								    while((!(PINB & (1<<7)))&& (!(TCCR1B&(1<<0)))){}
								    display();
							}
						}

		if(!(PINB&1<<6))
			{
				_delay_ms(30);

			if(!(PINB&1<<6))
			{

			   if(flag1==0&&seconds0!=9)
				 {
				   seconds0++;
				   flag1=1;
				  }
				 else if(flag1==0&&seconds0==9)
				 {
					 seconds1++;
					 seconds0=0;
					 flag1=1;
				 }
				 else if(flag1==0&&seconds1!=5)
				 {
					seconds1++;
					flag1=1;

				 }

				 while(!(PINB&1<<6))
				  {
					 display();

				  }
			}
			}
		else
						 {
							 flag1=0;
						  }

			 if(!(PINB&(1<<4)))
			 {
				 _delay_ms(30);
			 if(!(PINB&(1<<4)))
			{

			    if(flag2==0&&minutes0!=9)
				   {
						minutes0++;
						flag2=1;
					}
				else if(flag2==0&&minutes0==9)
				{
					minutes1++;
					minutes0=0;
					flag2=1;
				}
				else if(flag2==0&&minutes1!=5)
				{
					minutes1++;
					flag2=1;

				}

				while(!(PINB&(1<<4)))
				{
					display();
				}

			}
			 }
			 else
			 				{
			 					flag2=0;

			 				}

			 if(!(PINB&1<<1))
			 {
				 _delay_ms(30);
			 if(!(PINB&1<<1))
			 {
				 if(flag3==0&&hours0!=9)
				 {
				 	hours0++;
				 	flag3=1;
				 }
				 else if(flag3==0&&hours0==9)
				 {
					 hours1++;
					 hours0=0;
					 flag3=1;
				 }
				 else if(flag3==0&&hours1!=5)
				 {
				 	 hours1++;
				 	 flag3=1;
				 }

				 while(!(PINB&(1<<1)))
				 {
				 	display();
				 }

			 }
			 }
			 else
			 				 {
			 					 flag3=0;
			 				 }

			 if(!(PINB&1<<5))
			 	{
			 		_delay_ms(30);
			 	if(!(PINB&1<<5))
			 	{

			 	   if(flag4==0&&seconds0!=0)
			 		 {
			 		   seconds0--;
			 		   flag4=1;
			 		  }

			 		 else if(flag4==0&&seconds1!=0)
			 		 {
			 			seconds1--;
			 			seconds0=9;
			 			flag4=1;

			 		 }
			 		 else
			 		 {

			 		 }

			 		 while(!(PINB&1<<5))
			 		  {
			 			 display();

			 		  }

			 	}
			 	}
			 else
			 			 		 {
			 			 			 flag4=0;


			 			 		  }

			 	if(!(PINB&(1<<3)))
			 	{
			 		_delay_ms(30);
			 	 if(!(PINB&(1<<3)))
			 	{

			 	    if(flag5==0&&minutes0!=0)
			 		   {
			 				minutes0--;
			 				flag5=1;
			 			}

			 		else if(flag5==0&&minutes1!=0)
			 		{
			 			minutes1--;
			 			minutes0=9;
			 			flag5=1;

			 		}
			 		else
			 		{

			 		}

			 		while(!(PINB&(1<<3)))
			 		{
			 			display();
			 		}
			 	}
			 	}
			 	else
			 				 		 {
			 				 			 flag5=0;
			 				 		  }

			 	if(!(PINB&1<<0))
			 	{
			 		_delay_ms(30);

			 	 if(!(PINB&1<<0))
			 	 {
			 		 if(flag6==0&&hours0!=0)
			 		 {
			 		 	hours0--;
			 		 	flag6=1;
			 		 }

			 		 else if(flag6==0&&hours1!=0)
			 		 {
			 		 	 hours1--;
			 		 	 hours0=9;
			 		 	 flag6=1;
			 		 }
			 		 else
			 		 {

			 		 }

			 		 while(!(PINB&(1<<0)))
			 		 {
			 		 	display();
			 		 }
			 	 }
			 	}
			 	else
			 				 		 {
			 				 			 flag6=0;
			 				 		 }
	}
}





