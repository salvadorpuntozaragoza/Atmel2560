#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>

#define BUFFER_SIZE 64
#define FOSC 16000000

typedef struct{
	unsigned char buffer[BUFFER_SIZE];
	volatile unsigned char in_idx;
	volatile unsigned char out_idx;
} ring_buffer_t;

ring_buffer_t colaRx;
ring_buffer_t colaTx;

unsigned char clear[] = "\033[1J";
unsigned char clear2[] = "\033[0J";
unsigned char gotox[] = "\033[1C";
unsigned char gotoy[] = "\033[1B";
unsigned char green[] = "\033[1;47;32m";
unsigned char yellow[] = "\033[1;47;33m";
unsigned char blue[] = "\033[1;47;34m";
unsigned char clr[] = "\033[0;0f";

void UART0_config(uint8_t com, uint32_t baudrate, uint8_t size, uint8_t parity, uint8_t stop){
	
	uint8_t *ptr = &UBRR0H + (8*com);//DIRECCION BASE DEL UBBR0H
	uint8_t *ptr2= &UCSR0A + (8*com);// DIRECCION BASE DEL UCSR0A

	*ptr = (FOSC/16/baudrate - 1) >> 8; //UBRR HIGH
	*ptr--;
	*ptr = (FOSC/16/baudrate - 1); //UBRR LOW


	*ptr2 &= ~(3<<MPCM0); //VELOCIDAD NORMAL SIN MPCM

	*ptr2++;
	*ptr2 |= (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0); //HABILITAR RECEPCION Y TRANSMISION
	*ptr2 &= ~(1 << UCSZ02);

	*ptr2++;
	*ptr2 &= ~(3<<UMSEL00); //MODO ASINCRONO
	*ptr2 |= ((2 | parity) << UPM00) | ((stop >> 1) << USBS0);
	if(parity == 0)
		*ptr2 &= ~(3<<UPM00);
	*ptr2 |= ((size - 5) & 0x03) << UCSZ00;
	*ptr2 &= ~(1<<UCPOL0); 
}

void UART0_AutoBaudRate()
{
	uint16_t counter;
	TCNT0 = 0x00;	//Inicializar contador en 0
	TCCR0A = 0x00;	//Timer modo Normal
	while((PINE & 0x01));
	TCCR0B = 0x02;	//Activar timer con pre-escalador 8
	while(!(PINE & 0x01));
	counter = TCNT0;
	UBRR0 = counter * 0.5;
}

void UART0_putchar(unsigned char data){
	if(colaTx.in_idx == 64)
		colaTx.in_idx = 0;	

	while(colaTx.in_idx+1 == colaTx.out_idx);

	colaTx.buffer[colaTx.in_idx++] = data;

	UCSR0B |= (1 << UDRIE0);

}

void UART0_puts (unsigned char *str )
{
	while( *str )
		UART0_putchar( *str++ );
}

//---------------------Transmision-------------------------------

uint8_t UART0_available(){
	uint8_t available = 0;
	
	if(colaRx.in_idx != colaRx.out_idx)
		available = 1;

	return available;
}

unsigned char UART0_getchar(void ){
	//UCSR0B |= (1 << RXCIE0);
	if(colaRx.out_idx == 64)
		colaRx.out_idx = 0;

	while(!UART0_available());

	return colaRx.buffer[colaRx.out_idx++];
}

unsigned char UART0_getch()
{
	if(colaRx.out_idx == 64)
		colaRx.out_idx = 0;

	return colaRx.buffer[colaRx.out_idx++];
}

void gotoxy(int x, int y)
{

	UART0_puts(clr);

	while(x > 0)
	{
		UART0_puts(gotox);
		x--;
	}

	while(y > 0)
	{
		UART0_puts(gotoy);
		y--;
	}

} 

void clrscr(void)
{
	//UART0_puts(clear);
	UART0_puts(clr);
	UART0_puts(clear2);
}

void setColor(uint8_t color)
{

	if(color == 1)
		UART0_puts(yellow);
	if(color == 2)
		UART0_puts(green);
	if(color == 3)
		UART0_puts(blue);

}

//-----------------------------------------------------------------

void UART0_gets (unsigned char *str)
{
	int contador = 0;
	unsigned char dato = 0;
	
	while(dato != 13)
	{

		dato = UART0_getchar();
		colaRx.in_idx = colaRx.out_idx = 0;

		if(dato != 13 && dato != 8 && contador <= 20)
		{
			UART0_putchar(dato);
			*str = dato;
			contador++;
			*str++;
		}


		if(dato == 8 && contador > 0 )
		{
			UART0_putchar(8);
			UART0_putchar(' ');
			UART0_putchar(8);
			*str--;
			contador--;
		}


		if(dato == 13)
			*str = 0;

	}
}

void itoa (char *str, uint16_t number, unsigned char base)
{

	int cociente = 100;
	int residuo = 0;
	int contador = 0;
	int aux[20];
	int *ptr = &aux[0];

	while(cociente > 0)
	{
		residuo = number % base;
		cociente = number / base;

		if(residuo < 10)
			*ptr = residuo + 0x30;

		if(residuo > 9)
			*ptr = residuo + 0x37;
		
		number = cociente;
		contador++;
		*ptr++;
	

	}

	*ptr--;

	while(contador > 0)
	{
		*str = *ptr;
		*str++;
		*ptr--;
		contador--;
	}

	*str = 0;

}

unsigned int atoi( char *str)
{
	unsigned char dato = *str;
	int contador = 0;
	unsigned char aux;
	unsigned int num = 0;

	while (dato >= '0' && dato <= '9')
	{	
		*str++;
		contador++;
		dato = *str;
	}

	*str = 0;

	while(contador > 0)
	{
		*str--;
		contador--;
	}

	while(*str != 0)
	{
		num = num * 10;
		aux = *str;
		num = (num + (aux - 0x30));
		*str++;
	}

	return num;
}


ISR(USART0_UDRE_vect){
	if(colaTx.out_idx == 64)
		colaTx.out_idx = 0;

	if(colaTx.in_idx != colaTx.out_idx)
		UDR0 = colaTx.buffer[colaTx.out_idx++];

	if(colaTx.in_idx == colaTx.out_idx)
		UCSR0B &= ~(1 << UDRIE0);
}

ISR(USART0_TX_vect)
{
	//if(colaTx.in_idx == colaTx.out_idx)
	//	UCSR0B &= ~(1 << UDRIE0);	
}

ISR(USART0_RX_vect){
	UCSR0B &= ~(1 << RXCIE0);	

	while(colaRx.in_idx + 1 == colaRx.out_idx);
	
	colaRx.buffer[colaRx.in_idx++] = UDR0;

	if(colaRx.in_idx == 64)
		colaRx.in_idx = 0;

	UCSR0B |= 1 << RXCIE0;

}
