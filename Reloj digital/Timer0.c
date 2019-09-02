#include <avr/interrupt.h>
#include <inttypes.h>

#define FOSC 16000000

static volatile uint8_t SecFlag;
static volatile uint32_t mSecCnt = 0;

void Timer0_Ini ( void ){
	mSecCnt = 0;
	OCR0A = 0xF9;
	TCNT0 = 0x00; /* Inicializar valor para el timer0 */
	TCCR0A = (2 << WGM00);  /*inicializa timer0 en modo CTC */
	/* Inicializar con fuente de osc. Int. */
	TCCR0B = 0x03; /* con Prescalador 64 */
	TIMSK0 = 0x02; /* habilita interrupcion del Timer0 */
	//sei(); /* habilita interrupciones (global) */
}

uint8_t Timer0_SecFlag ( void ){
	if( SecFlag ){
		SecFlag=0;
		return 1;
	}
	else{
		return 0;
	}
}

ISR (TIMER0_OVF_vect)
{
	mSecCnt++;
}

ISR (TIMER0_COMPA_vect)
{
	static uint16_t aux;
	mSecCnt++;
	aux++;
	if(aux == 1000)
	{
		aux = 0;
		SecFlag = 1;
	}
}

uint32_t millis( void )
{

	return mSecCnt;

}
