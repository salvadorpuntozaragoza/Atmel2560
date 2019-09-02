#include <avr/io.h>
#include <avr/interrupt.h>
#include "UART0.h"
#include "Timer.h"

char ADCstr[20] = "";
uint16_t i;
uint16_t lectura, startReadings = 0;
uint16_t promedio = 0;

extern void delay1();

void ADC_init()
{
	ADMUX |= (1 << REFS0) | (1 << ADLAR) | (1 << MUX0); //Referencia = VCC, Left adujsted = ON, ADC1
	ADCSRA |= (1 << ADEN) | (5 << ADPS0); // ADC enabled, pre-escaler = 32, Freq = 500kHz
	ADCSRB |= (0 << ADTS0); 
}

uint16_t ADC_read()
{
	uint16_t temp;
	ADCSRA |= (1 << ADSC);
	while(ADCSRA & (1 << ADSC))
		;
	temp = (ADCH << 2);

	return temp;
}

uint16_t getPromedio()
{
	uint16_t promedio = 0;
	for(i = 0 ; i < 10 ; i++){
		promedio = promedio + ADC_read();
		delay1();
	}
	
	promedio = promedio / 10;

	return promedio;
}

uint8_t motionDetected()
{
	lectura = ADC_read();
	delay1();

	if(SecFlag())
		startReadings = 1;

	if((lectura < (getPromedio() - 20) || lectura > (getPromedio() + 20)) && startReadings == 1)
		return 1;
	else 
		return 0;
}