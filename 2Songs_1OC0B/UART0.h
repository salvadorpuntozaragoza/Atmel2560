#include <avr/io.h>
#define BUFFER_SIZE 64

void UART0_config(uint8_t com, uint32_t baudrate, uint8_t size, uint8_t parity, uint8_t stop);
void UART0_putchar(unsigned char dato);
unsigned char UART0_getchar();
void UART0_puts(char *str);
void UART0_gets(char *str);
void UART0_AutoBaudRate();
unsigned char UART0_getch();
uint8_t UART0_available();

void itoa(char *str, uint16_t number, unsigned char base);
unsigned int atoi(char *str);

void gotoxy(int x, int y);
void clrscr(void);
void setColor(uint8_t color);

typedef struct{
	unsigned char buffer[BUFFER_SIZE];
	volatile unsigned char in_idx;
	volatile unsigned char out_idx;
} ring_buffer_t;


extern ring_buffer_t colaRx;
extern ring_buffer_t colaTx; 
