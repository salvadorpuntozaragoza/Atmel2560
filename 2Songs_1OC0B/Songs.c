#include "UART0.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "Timer.h"

const struct note Mario[] PROGMEM={
	//INTRO
	{e, TEMPO_600/4},
	{e, TEMPO_600/4},
	{0, TEMPO_600/4},
	{e, TEMPO_600/4},
	{0, TEMPO_600/4},
	{c, TEMPO_600/4},
	{e, TEMPO_600/4},
	{0, TEMPO_600/4},
	{g, TEMPO_600/4},
	{0, TEMPO_600/4},
	{0, TEMPO_600/2},
	{gL, TEMPO_600/4},
	{0, TEMPO_600/4},
	{0, TEMPO_600/2},
	//PARTE A
	{c, TEMPO_600/4},
	{0, TEMPO_600/4},
	{0, TEMPO_600/4},
	{gL, TEMPO_600/4},
	{0, TEMPO_600/2},
	{eL, TEMPO_600/4},
	{0, TEMPO_600/4},
	{0, TEMPO_600/4},
	{aL, TEMPO_600/4},
	{0, TEMPO_600/4},
	{bL, TEMPO_600/4},
	{0, TEMPO_600/4},
	{aSL, TEMPO_600/4},
	{aL, TEMPO_600/4},
	{0, TEMPO_600/4},
	{gL, TEMPO_600/3},
	{e, TEMPO_600/3},
	{g, TEMPO_600/3},
	{a, TEMPO_600/4},
	{0, TEMPO_600/4},
	{f, TEMPO_600/4},
	{g, TEMPO_600/4},
	{0, TEMPO_600/4},
	{e, TEMPO_600/4},
	{0, TEMPO_600/4},
	{c, TEMPO_600/4},
	{d, TEMPO_600/4},
	{bL, TEMPO_600/4},
	{0, TEMPO_600/2},
	{c, TEMPO_600/4},
	{0, TEMPO_600/4},
	{0, TEMPO_600/4},
	{gL, TEMPO_600/4},
	{0, TEMPO_600/2},
	{eL, TEMPO_600/4},
	{0, TEMPO_600/4},
	{0, TEMPO_600/4},
	{aL, TEMPO_600/4},
	{0, TEMPO_600/4},
	{bL, TEMPO_600/4},
	{0, TEMPO_600/4},
	{aSL, TEMPO_600/4},
	{aL, TEMPO_600/4},
	{0, TEMPO_600/4},
	{gL, TEMPO_600/3},
	{e, TEMPO_600/3},
	{g, TEMPO_600/3},
	{a, TEMPO_600/4},
	{0, TEMPO_600/4},
	{f, TEMPO_600/4},
	{g, TEMPO_600/4},
	{0, TEMPO_600/4},
	{e, TEMPO_600/4},
	{0, TEMPO_600/4},
	{c, TEMPO_600/4},
	{d, TEMPO_600/4},
	{bL, TEMPO_600/4},
	{0, TEMPO_600/2},
	//PARTE B
	{0, TEMPO_600/2},
	{g, TEMPO_600/4},
	{fS,TEMPO_600/4},
	{e, TEMPO_600/4},
	{dS, TEMPO_600/4},
	{0, TEMPO_600/4},
	{e, TEMPO_600/4},
	{0, TEMPO_600/4},
	{gSL, TEMPO_600/4},
	{aL, TEMPO_600/4},
	{c, TEMPO_600/4},
	{0, TEMPO_600/4},
	{aL, TEMPO_600/4},
	{c, TEMPO_600/4},
	{d,TEMPO_600/4},
	{0, TEMPO_600/2},
	{g, TEMPO_600/4},
	{fS, TEMPO_600/4},
	{e, TEMPO_600/4},
	{dS, TEMPO_600/4},
	{0, TEMPO_600/4},
	{e,TEMPO_600/4},
	{0, TEMPO_600/4},
	{cH, TEMPO_600/4},
	{0, TEMPO_600/4},
	{cH, TEMPO_600/4},
	{cH, TEMPO_600/4},
	{0, TEMPO_600/4},
	{0,TEMPO_600/2},
	{0, TEMPO_600/2},
	{g, TEMPO_600/4},
	{fS, TEMPO_600/4},
	{e, TEMPO_600/4},
	{dS, TEMPO_600/4},
	{0, TEMPO_600/4},
	{e,TEMPO_600/4},
	{0, TEMPO_600/4},
	{gSL, TEMPO_600/4},
	{aL, TEMPO_600/4},
	{c, TEMPO_600/4},
	{0, TEMPO_600/4},
	{aL, TEMPO_600/4},
	{c, TEMPO_600/4},
	{d,TEMPO_600/4},
	{0, TEMPO_600/2},
	{dS, TEMPO_600/4},
	{0, TEMPO_600/4},
	{0, TEMPO_600/4},
	{d, TEMPO_600/4},
	{0,TEMPO_600/2},
	{c, TEMPO_600/4},
	{0, TEMPO_600/4},
	{0, TEMPO_600/2},
	{0,TEMPO_600},
	{0, TEMPO_600/2},
	{g, TEMPO_600/4},
	{fS, TEMPO_600/4},
	{e, TEMPO_600/4},
	{dS, TEMPO_600/4},
	{0, TEMPO_600/4},
	{e,TEMPO_600/4},
	{0, TEMPO_600/4},
	{gSL, TEMPO_600/4},
	{aL, TEMPO_600/4},
	{c, TEMPO_600/4},
	{0, TEMPO_600/4},
	{aL, TEMPO_600/4},
	{c, TEMPO_600/4},
	{d,TEMPO_600/4},
	{0, TEMPO_600/2},
	{g, TEMPO_600/4},
	{fS, TEMPO_600/4},
	{e, TEMPO_600/4},
	{dS, TEMPO_600/4},
	{0, TEMPO_600/4},
	{e,TEMPO_600/4},
	{0, TEMPO_600/4},
	{cH, TEMPO_600/4},
	{0, TEMPO_600/4},
	{cH, TEMPO_600/4},
	{cH, TEMPO_600/4},
	{0, TEMPO_600/4},
	{0,TEMPO_600/2},
	{0, TEMPO_600/2},
	{g, TEMPO_600/4},
	{fS, TEMPO_600/4},
	{e, TEMPO_600/4},
	{dS, TEMPO_600/4},
	{0, TEMPO_600/4},
	{e,TEMPO_600/4},
	{0, TEMPO_600/4},
	{gSL, TEMPO_600/4},
	{aL, TEMPO_600/4},
	{c, TEMPO_600/4},
	{0, TEMPO_600/4},
	{aL, TEMPO_600/4},
	{c, TEMPO_600/4},
	{d,TEMPO_600/4},
	{0, TEMPO_600/2},
	{dS, TEMPO_600/4},
	{0, TEMPO_600/4},
	{0, TEMPO_600/4},
	{d, TEMPO_600/4},
	{0,TEMPO_600/2},
	{c, TEMPO_600/4},
	{0, TEMPO_600/4},
	{0, TEMPO_600/2},
	{0,TEMPO_600},
	//PARTE C
	{c, TEMPO_600/4},
	{c, TEMPO_600/4},
	{0, TEMPO_600/4},
	{c, TEMPO_600/4},
	{0, TEMPO_600/4},
	{c, TEMPO_600/4},
	{d, TEMPO_600/4},
	{0, TEMPO_600/4},
	{e, TEMPO_600/4},
	{c, TEMPO_600/4},
	{0, TEMPO_600/4},
	{aL, TEMPO_600/4},
	{gL, TEMPO_600/4},
	{0,TEMPO_600/4},
	{0,TEMPO_600/2},
	{c, TEMPO_600/4},
	{c, TEMPO_600/4},
	{0, TEMPO_600/4},
	{c, TEMPO_600/4},
	{0, TEMPO_600/4},
	{c, TEMPO_600/4},
	{d, TEMPO_600/4},
	{e,TEMPO_600/4},
	{0,TEMPO_600*2},
	{c, TEMPO_600/4},
	{c, TEMPO_600/4},
	{0, TEMPO_600/4},
	{c, TEMPO_600/4},
	{0, TEMPO_600/4},
	{c, TEMPO_600/4},
	{d, TEMPO_600/4},
	{0, TEMPO_600/4},
	{e, TEMPO_600/4},
	{c, TEMPO_600/4},
	{0, TEMPO_600/4},
	{aL, TEMPO_600/4},
	{gL, TEMPO_600/4},
	{0, TEMPO_600/4},
	{0,TEMPO_600/2},
	{e, TEMPO_600/4},
	{e, TEMPO_600/4},
	{0, TEMPO_600/4},
	{e, TEMPO_600/4},
	{0, TEMPO_600/4},
	{c, TEMPO_600/4},
	{e, TEMPO_600/4},
	{0,TEMPO_600/4},
	{g, TEMPO_600/4},
	{0, TEMPO_600/4},
	{0, TEMPO_600/2},
	{gL, TEMPO_600/4},
	{0, TEMPO_600/4},
	{0, TEMPO_600/2},
	//PARTE D
	{e, TEMPO_600/4},
	{c, TEMPO_600/4},
	{0, TEMPO_600/4},
	{gL, TEMPO_600/4},
	{0, TEMPO_600/2},
	{gSL, TEMPO_600/4},
	{0,TEMPO_600/4},
	{aL, TEMPO_600/4},
	{f, TEMPO_600/4},
	{0, TEMPO_600/4},
	{f, TEMPO_600/4},
	{aL, TEMPO_600/4},
	{0, TEMPO_600/4},
	{0,TEMPO_600/2},
	{bL, TEMPO_600/3},
	{a, TEMPO_600/3},
	{a, TEMPO_600/3},
	{a, TEMPO_600/3},
	{g, TEMPO_600/3},
	{f,TEMPO_600/3},
	{e, TEMPO_600/4},
	{c, TEMPO_600/4},
	{0, TEMPO_600/4},
	{aL, TEMPO_600/4},
	{gL, TEMPO_600/4},
	{0, TEMPO_600/4},
	{0,TEMPO_600/2},
	{e, TEMPO_600/4},
	{c, TEMPO_600/4},
	{0, TEMPO_600/4},
	{gL, TEMPO_600/4},
	{0, TEMPO_600/2},
	{gSL, TEMPO_600/4},
	{0,TEMPO_600/4},
	{aL, TEMPO_600/4},
	{f, TEMPO_600/4},
	{0, TEMPO_600/4},
	{f, TEMPO_600/4},
	{aL, TEMPO_600/4},
	{0, TEMPO_600/4},
	{0,TEMPO_600/2},
	{bL, TEMPO_600/4},
	{f, TEMPO_600/4},
	{0, TEMPO_600/4},
	{f, TEMPO_600/4},
	{f, TEMPO_600/3},
	{e, TEMPO_600/3},
	{d,TEMPO_600/3},
	{c, TEMPO_600/4},
	{eL, TEMPO_600/4},
	{0, TEMPO_600/4},
	{eL, TEMPO_600/4},
	{cL, TEMPO_600/4},
	{0, TEMPO_600/4},
	{0, TEMPO_600/2},
	{87,TEMPO}
};

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


PGM_P const song_table[] ={
	Mario,
	ImperialMarch
};

uint8_t song1Playing, song2Playing; //Banderas que indican el estado de las canciones


int main(){
	UART0_config(0,115200,8,0,1); //Com = 0, Baud = 115200, 8 bits, no paridad, 1 stop bit
	sei();
	
	Timer0_Ini();
    
	while(1)
	{
		
		if (UART0_available())
		{
			// Play Song
			switch(UART0_getch())
			{
				case '1':
					if(song1Playing == 0) // si no se esta reproduciendo la cancion 1, entonces se reproduce
					{
						song1Playing = 1;
						playSong1(song_table[0]);
					}
					else // si se esta reprocudiendo se deja de reproducir
					{
						song1Playing = 0;
						resetPlayer1();
					}
					break;

				case '2':
					if(song2Playing == 0)
					{
						song2Playing = 1;
						playSong2(song_table[1]);
					}
					else
					{
						song2Playing = 0;
						resetPlayer2();
					}
					break;

				case '0':
					song1Playing = 0;
					song2Playing = 0;
					resetPlayer1();
					resetPlayer2();
					break;
			}
		}//UART0_available
	}//while
}
