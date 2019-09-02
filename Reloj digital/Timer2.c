#include <avr/interrupt.h>
#include <avr/io.h>
#include <inttypes.h>

static uint8_t Flag;

void Timer2_Ini( uint8_t baseT ){
	/* Configurar Timer2 para usar cristal externo según */
	/* secuencia dada por la hoja de especificaciones */
	//XTAL 32k, 1 sec

	uint8_t count = 31 * baseT;

	//#1
	TIMSK2 = 0; //DISABLE INTERRUPTS
	//#2
	ASSR = 1 << AS2; //
	//#3 
	TCNT2 = 0; //CLEAR COUNT
	OCR2A = count; // COUNT LIMIT
	TCCR2A = 2 << WGM20; // CTC MODE

	//#4 
	while(ASSR & ((1<<TCN2UB) | (1<<OCR2AUB) | (1<<TCR2AUB)));

	//#5
	TIFR2 = 7; // CLEAR FLAGS

	//#6
	TIMSK2 = 1 << OCIE2A;

	//PONER ESTE AL ULTIMO SIEMPRE
	sei();
	TCCR2B = 7 << CS20; // PS = 1024
}

uint8_t Timer2_Flag ( void ){
	if( Flag ){
		Flag=0;
		return 1;
	}
	else{
		return 0;
	}
}

/* Rutina de Servicio de Interrupción para atender la interrupción */
/* por Comparación del Timer2 (TCNT2 y OCR2 son iguales) */
ISR(SIG_OUTPUT_COMPARE2A){
	Flag = 1; /* Activa Bandera para indicar los 1 segundos */
} /* fin de la rutina de servicio de Interrupción (RSI) */
