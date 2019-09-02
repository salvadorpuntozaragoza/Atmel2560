#include <avr/io.h>
#include "Timer.h"
#include "UART0.h"
#include "ADC.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define SHORT_PRESSED 1
#define LONG_PRESSED 2
#define NOT_PRESSED 0 
#define SW_INC &PINK
#define SW_DEC &PINK
#define SW_SET &PIND

#define L1 22
#define L2 241


void setAlarmTime();
void setCurrentTime();
void updateClock();
void displayClock();
void initClock(uint8_t horas, uint8_t minutos, uint8_t segundos);
void initPorts();
uint8_t checkBtnInc();
uint8_t checkBtnSet();
void delay(uint16_t msec);
void delay1ms();
uint8_t checkBtnDec();

const struct note ImperialMarch[] PROGMEM ={
	//for the sheet music see:
	//http://www.musicnotes.com/sheetmusic/mtd.asp?ppn=MN0016254
	//this is just a translation of said sheet music to frequencies / time in ms
	//used TEMPO ms for a quart note
	{a, TEMPO},
	{a, TEMPO},
	{a, TEMPO},
	{f, TEMPO*3/4},
	{cH, TEMPO*1/4},

	{a, TEMPO},
	{f, TEMPO*3/4},
	{cH, TEMPO*1/4},
	{a, TEMPO*2},
	//first bit

	{eH, TEMPO},
	{eH, TEMPO},
	{eH, TEMPO},
	{fH, TEMPO*3/4},
	{cH, TEMPO*1/4},

	{gS, TEMPO},
	{f, TEMPO*3/4},
	{cH, TEMPO*1/4},
	{a, TEMPO*2},
	//second bit...

	{aH, TEMPO},
	{a, TEMPO*3/4},
	{a, TEMPO*1/4},
	{aH, TEMPO},
	{gSH, TEMPO/2},
	{gH, TEMPO/2},

	{fSH, TEMPO*1/4},
	{fH, TEMPO*1/4},
	{fSH, TEMPO/2},
	{0,TEMPO/2},
	{aS, TEMPO/2},
	{dSH, TEMPO},
	{dH, TEMPO/2},
	{cSH, TEMPO/2},
	//start of the interesting bit

	{cH, TEMPO*1/4},
	{b, TEMPO*1/4},
	{cH, TEMPO/2},
	{0,TEMPO/2},
	{f, TEMPO*1/4},
	{gS, TEMPO},
	{f, TEMPO*3/4},
	{a, TEMPO*1/4},

	{cH, TEMPO},
	{a, TEMPO*3/4},
	{cH, TEMPO*1/4},
	{eH, TEMPO*2},
	//more interesting stuff (this doesn't quite get it right somehow)

	{aH, TEMPO},
	{a, TEMPO*3/4},
	{a, TEMPO*1/4},
	{aH, TEMPO},
	{gSH, TEMPO/2},
	{gH, TEMPO/2},

	{fSH, TEMPO*1/4},
	{fH, TEMPO*1/4},
	{fSH, TEMPO/2},
	{0,TEMPO/2},
	{aS, TEMPO/2},
	{dSH, TEMPO},
	{dH, TEMPO/2},
	{cSH, TEMPO/2},
	//repeat... repeat

	{cH, TEMPO*1/4},
	{b, TEMPO*1/4},
	{cH, TEMPO/2},
	{0,TEMPO/2},
	{f, TEMPO/2},
	{gS, TEMPO},
	{f, TEMPO*3/4},
	{cH, TEMPO*1/4},

	{a, TEMPO},
	{f, TEMPO*3/4},
	{c, TEMPO*1/4},
	{a, TEMPO*2},
	{87,TEMPO}
	//and we're done
};

PGM_P const song_table[] = {ImperialMarch};

uint8_t hora, min, sec, movimiento, alarmSet;
uint8_t alarmHora = 12, alarmMin = 40;
uint8_t seguir = 1, cambiarHora = 1;
char cadena[10] = "";


int main()
{

	sei();
	UART0_config(0,115200,8,0,1);
	itoa(cadena, UBRR0, 10);

	while(1){
		UART0_puts("uabc");
		UART0_getchar();
	}
	UART0_puts(cadena);
	UART0_puts("System started\n\r");
	Timer0_Ini();
	ADC_init();
	initPorts();
	initClock(12,30,0);

	while(1)
	{

		if(SecFlag() == 1)
		{
			updateClock();
			displayClock();
		}

		if(alarmSet){
		
			if(checkBtnInc()){
				alarmSet = 0;
				resetPlayer();
				alarmMin += 5;
				//UART0_puts("10 sec pasados");
			}

			if(checkBtnSet())
			{
				alarmSet = 0;
				resetPlayer();
			}

			if(checkBtnDec())
			{
				alarmSet = 0;
				resetPlayer();
				alarmMin += 5;
			}
		
		}else{

			switch(checkBtnSet())
			{
				case LONG_PRESSED:
					setAlarmTime();
					break;
				case SHORT_PRESSED:
					setCurrentTime();
					break;
				break;
			}

			if(motionDetected() == 1){
				Timer2_PlayFlash(song_table[0]);
				alarmSet = 1;
				//resetTimer10();
			}		
		}
	}
}

void setAlarmTime()
{
	seguir = 1;
	cambiarHora = 1;

	gotoxy(5,5);
	UART0_puts("Alarma: ");
	itoa(cadena, alarmHora, 10);
	UART0_puts(cadena);
	UART0_putchar(':');

	itoa(cadena, alarmMin, 10);
	UART0_puts(cadena);

	while(seguir)
	{
		switch(checkBtnSet())
		{
			case LONG_PRESSED:
				seguir = 0;
				break;
			case SHORT_PRESSED:
				if(cambiarHora)
					cambiarHora = 0;
				else
					cambiarHora = 1;
				break;
			break;
		}

		if(checkBtnDec())
		{
			if(alarmHora == 0 && cambiarHora)
				alarmHora = 24;
			if(alarmMin == 0 && cambiarHora == 0)
				alarmMin = 60;

			if(cambiarHora)
				alarmHora--;
			else
				alarmMin--;


			gotoxy(5,5);
			clrscr();
			UART0_puts("Set alarma: ");
			itoa(cadena, alarmHora, 10);
			UART0_puts(cadena);
			UART0_putchar(':');

			itoa(cadena, alarmMin, 10);
			UART0_puts(cadena);
		}

		if(checkBtnInc())
		{
			if(cambiarHora)
				alarmHora++;
			else
				alarmMin++;

			if(alarmHora == 24 && cambiarHora)
				alarmHora = 0;
			if(alarmMin == 60 && cambiarHora == 0)
				alarmMin = 0;

			gotoxy(5,5);
			clrscr();
			UART0_puts("Set alarma: ");
			itoa(cadena, alarmHora, 10);
			UART0_puts(cadena);
			UART0_putchar(':');

			itoa(cadena, alarmMin, 10);
			UART0_puts(cadena);
		}
	}
}


void setCurrentTime()
{
	seguir = 1;
	cambiarHora = 1;

	gotoxy(5,5);
	clrscr();
	UART0_puts("Cambiar hora: ");
	itoa(cadena, hora, 10);
	UART0_puts(cadena);
	UART0_putchar(':');

	itoa(cadena, min, 10);
	UART0_puts(cadena);

	while(seguir)
	{
		switch(checkBtnSet())
		{
			case LONG_PRESSED:
				seguir = 0;
				break;
			case SHORT_PRESSED:
				if(cambiarHora)
					cambiarHora = 0;
				else
					cambiarHora = 1;
				break;
			break;
		}

		if(checkBtnDec())
		{
			if(hora == 0 && cambiarHora)
				hora = 24;
			if(min == 0 && cambiarHora == 0)
				min = 60;

			if(cambiarHora)
				hora--;
			else
				min--;


			gotoxy(5,5);
			clrscr();
			UART0_puts("Cambiar hora: ");
			itoa(cadena, hora, 10);
			UART0_puts(cadena);
			UART0_putchar(':');

			itoa(cadena, min, 10);
			UART0_puts(cadena);
		}

		if(checkBtnInc())
		{
			if(cambiarHora)
				hora++;
			else
				min++;

			if(hora == 24 && cambiarHora)
				hora = 0;
			if(min == 60 && cambiarHora == 0)
				min = 0;

			gotoxy(5,5);
			clrscr();
			UART0_puts("Cambiar hora: ");
			itoa(cadena, hora, 10);
			UART0_puts(cadena);
			UART0_putchar(':');

			itoa(cadena, min, 10);
			UART0_puts(cadena);
		}
	}
}

void updateClock()
{

	sec++;
	/*falta minutos , horas etc. etc. */
	if(sec == 60)
	{
		sec = 0;
		min++;
	}

	if(min == 60)
	{
		min = 0;
		hora++;
	}

	if(hora == 24)
	{
		hora = 0;
	}

	if(alarmHora == hora && alarmMin == min && sec == 0)
	{
		Timer2_PlayFlash(song_table[0]);
		alarmSet = 1;
	}

}

void displayClock()
{
	clrscr();
	gotoxy(5,5);
	itoa(cadena,hora,10);
	UART0_puts(cadena);
	UART0_putchar(':');

	itoa(cadena,min,10);
	if(min < 10)
		UART0_putchar('0');
	UART0_puts(cadena);
	UART0_putchar(':');

	if(sec < 10)
		UART0_putchar('0');
	itoa(cadena,sec,10);
	UART0_puts(cadena);
}

void initClock(uint8_t horas, uint8_t minutos, uint8_t segundos)
{
	hora = horas;
	min = minutos;
	sec = segundos;
}

void initPorts()
{

	DDRK = 0x24;
	PORTK = 0x09;
	DDRD = 0x04;
	PORTD = 0x01;

}

uint8_t checkBtnInc()
{
	uint16_t millis = 0;

	if((PINK & 0x01) == 0)
	{
		while(millis < 200){
			millis++;
			delay(1);
		}

		return 1;
	}

	return 0;
}

uint8_t checkBtnDec()
{
	uint16_t millis = 0;

	if((PINK & 0x04) == 0)
	{
		while(millis < 200){
			millis++;
			delay(1);
		}

		return 1;
	}

	return 0;
}

uint8_t checkBtnSet()
{

	uint16_t millis = 0;

	if((PIND & 0x01) == 0)
	{
		cli();
		while(millis < 200){
			millis++;
			delay(1);
		}

		while((PIND & 0x01) == 0){
			millis++;
			delay(1);
		}
		sei();

		if((millis > 0) && (millis < 1000)){
			return SHORT_PRESSED;

			}
		else{
			return LONG_PRESSED;
		}
	}

	return 0;

}

void delay1ms()
{
		__asm__ __volatile__(
			"ldi r24, %0 		\n\t"
			"1: ldi r23, %1 	\n\t"
			"2: dec r23			\n\t"
			"brne 2b 			\n\t"
			"nop 				\n\t"
			"dec r24 			\n\t"
			"brne 1b 			\n\t"
			:: "M" (L1), "M" (L2)
			:"r23", "r24"
		);
}

void delay(uint16_t mseg)
{
	while(mseg > 0)
	{
		delay1ms();
		mseg--;
	}
}
