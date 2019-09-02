#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "UART0.h"
#include "Timer.h"

/*	Definir el macro que calcula los ticks en base
	a al parametro de frecuencia (f). */
#define TICKS(f) (16000000 / 512 / f)

uint16_t mSecCnt1, mSecCnt2; //contadores independientes para cada cancion
uint16_t songEnd = 87; //Todas las canciones terminan con una nota de freq = 87

//Estas variables controlan el estado de las canciones
//play1 indica cuando se debe tocar la siguiente nota
//playing1 indica que la cancion se esta reproduciendo
//silencio1 indica que la nota ha alcanzado su tempo y por lo tanto deberia ejecutarse un silencio de 10ms
//idx1 es usado para desplazarse atraves del arreglo de notas al que se esta apuntando

//para la cancion 1
uint8_t play1 = 0, playing1 = 0, silencio1 = 0;
uint16_t volatile idx1 = 0, freq1, delay1;

//para la cancion 2
uint8_t play2 = 0, playing2 = 0, silencio2 = 0;
uint16_t volatile idx2 = 0, freq2, delay2;

//apuntadores a memoria FLASH, contienen la direccion de las notas de las canciones, siempre apuntan a la direccion inicial del arreglo de canciones
PGM_P pointer1;
PGM_P pointer2;

//Timer que genera una interrupcion cada 1ms
void Timer0_Ini ( void ){
	/* 	Permanece igual, ocasionando una interrupción 
		cada 1 ms en modo CTC. */
	OCR0A = 0xFA - 1;
	TCNT0 = 0x00; /* Inicializar valor para el timer0 */
	TCCR0A = (2 << WGM00);  /*inicializa timer0 en modo CTC */
	/* Inicializar con fuente de osc. Int. */
	TCCR0B = 0x03; /* con Prescalador 64 */
	TIMSK0 = 0x02; /* habilita interrupcion del Timer0 OC0A */
	DDRH |= (1 << PH6); //Habilitar puertos de salida para controlar 0C2A y OC2B
	DDRB |= (1 << PB4);
}


ISR(TIMER0_COMPA_vect){ 
	/* 	Código para actualizar bandera de segundos */
	mSecCnt1++;
	mSecCnt2++;


	if(mSecCnt1 == 2000)
		mSecCnt1 = 0;

	if(mSecCnt2 == 2000)
		mSecCnt2 = 0;

	/*	Agregar las instrucciones necesarias para reproducir
		la siguiente nota en el arreglo dependiendo de la duración, 
		e insertar los silencios entre cada nota. */
		
		//Este if revisa el estado de la bandera play, si esta en uno genera la frecuencia de la nota y asigna el tempo a la variable delay
		//--------------------------------------------------------------------------------------------------------------------------------------
		if(play1 == 1 && silencio1 == 0){
			play1 = 0;
			playing1 = 1;
			freq1 = (pgm_read_word(pointer1 + idx1));
			Timer2_Freq_GenA(TICKS(freq1));
			mSecCnt1 = 0;
			idx1 += 2;
			delay1 = (pgm_read_word(pointer1 + idx1));
		}
	
		if(play2 == 1 && silencio2 == 0){
			play2 = 0;
			playing2 = 1;
			freq2 = (pgm_read_word(pointer2 + idx2));
			Timer2_Freq_GenB(TICKS(freq2));
			mSecCnt2 = 0;
			idx2 += 2;
			delay2 = (pgm_read_word(pointer2 + idx2));
		}


		//Este if revisa si el tempo de la nota que se esta reproduciendo ya fue alcanzado, si no se esta en el silencio entre notas y la cancion
		//se esta reproduciendo, genera un silencio que durara 10ms
		//---------------------------------------------------------------------------------------------------------------------------------------
		if(mSecCnt1 == delay1 && silencio1 == 0 && playing1 == 1){
			Timer2_Freq_GenA(0);
			mSecCnt1 = 0;
			silencio1 = 1;
			playing1 = 0;
			idx1 += 2;
		}

		if(mSecCnt2 == delay2 && silencio2 == 0 && playing2 == 1){
			Timer2_Freq_GenB(0);
			mSecCnt2 = 0;
			silencio2 = 1;
			playing2 = 0;
			idx2 += 2;
		}


		//Este if se encarga de que el silencio entre notas dure 10ms
		//---------------------------------------------------------------------------------------------------------------------------------------
		if(mSecCnt1 == 10 && silencio1 == 1){
			silencio1 = 0;
			play1 = 1; 
		}

		if(mSecCnt2 == 10 && silencio2 == 1){
			silencio2 = 0;
			play2 = 1; 
		}


		//Si este if se cumple significa que se ha llegado al final de la cancion, por lo tanto el indice se reseta a 0 y se cicla la cancion
		//No existe ninguna nota musical con una freq de 87Hz por lo que no hay probabilidad de que se interrupma una cancion
		//----------------------------------------------------------------------------------------------------------------------------------------
		if(freq1 == songEnd)
			idx1 = 0;

		if(freq2 == songEnd)
			idx2 = 0;


}


//El timer2 se configuro en modo normal por lo que sigue contando hasta desbordarse
//por lo que cada ves que se tiene un match entre el TCNT2 y OCR2A/OCR2B estos ultimos 
//solo aumentan su valor para no esperar a que el timer2 de toda la vuelta
ISR(TIMER2_COMPA_vect)
{
	OCR2A += TICKS(freq1) - 1; // Genera nuevo tope para OCR2A
}

ISR(TIMER2_COMPB_vect)
{
	OCR2B += TICKS(freq2) - 1; // Genera nuevo tope para OCR2B
}


void Timer2_Freq_GenA(uint8_t ticks){
	/* 	Si "ticks" es mayor que 0 entonces, inicializa y habilita el Generador 
		de Frecuencia del Timer2 con el tope dado por "ticks".
		De lo contrario se requiere deshabilitar el Generador, generando de 
		esta forma el silencio (0 lógico). */

		if(ticks > 0)
		{
			TCCR2A |= (1 << COM2A0); //Toggle on compare match en OC2A
			OCR2A = TCNT2 + ticks - 1; //aqui el TCNT2 nunca se detiene por lo que para asignar el nuevo valor de OCR2A
			TIMSK2 |= (1 << OCIE2A);	//se toma el valor en el que esta actualmente el TCNT2 y se le suma ticks - 1
			TCCR2B |= (5 << CS20);
		}
		else
			//TCCR2B = 0;
			TCCR2A &= ~(1 << COM2A0);// Para generar el silencio entre notas se desactiva el toggle on compare lo que deshabilita 
										// OC2A/OC2B
}

void Timer2_Freq_GenB(uint8_t ticks){
	/* 	Si "ticks" es mayor que 0 entonces, inicializa y habilita el Generador 
		de Frecuencia del Timer2 con el tope dado por "ticks".
		De lo contrario se requiere deshabilitar el Generador, generando de 
		esta forma el silencio (0 lógico). */

		if(ticks > 0)
		{
			TCCR2A |= (1 << COM2B0);
			OCR2B = TCNT2 + ticks - 1;
			TIMSK2 |= (1 << OCIE2B);
			TCCR2B |= (5 << CS20);
		}
		else
			//TCCR2B = 0;
			TCCR2A &= ~(1 << COM2B0);

}

//Establece las condiciones iniciales para empezar a reproducir la cancion
void playSong1(PGM_P ptr)
{
	idx1 = 0;
	pointer1 = ptr + idx1;

	play1 = 1;
	silencio1 = 0;
	playing1 = 0;
	mSecCnt1 = 0;
}

void playSong2(PGM_P ptr){
	idx2 = 0;
	pointer2 = ptr + idx2 ;

	play2 = 1;
	silencio2 = 0;
	playing2 = 0;
	mSecCnt2 = 0;
}

//Establece las condiciones necesarias para que se deje de reproducir la cancion
void resetPlayer1()
{
	play1 = 0;
	silencio1 = 0;
	playing1 = 0;
	Timer2_Freq_GenA(0);
}

void resetPlayer2()
{
	play2 = 0;
	silencio2 = 0;
	playing2= 0;
	Timer2_Freq_GenB(0);
}