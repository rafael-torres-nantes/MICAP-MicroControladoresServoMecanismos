//GUILHERME BRANDAO E RAFAEL TORRES
#include "def_pinos.h"
#include "config.c"
#include "stdio.h"
#include "fonte.c"

#define CS1 P2_0
#define CS2 P2_1
#define RS P2_2
#define RW P2_3
#define E P2_4
#define RST P2_5
#define ESC 0
#define DB P4
#define CO 0
#define DA 1
#define ESQ 0
#define DIR 1
#define NOP4() NOP(); NOP(); NOP(); NOP()
#define NOP8() NOP4(); NOP4()
#define NOP12() NOP8(); NOP4()
#define lig 1
#define des 0

void delay_ms(unsigned int t){
	TMOD|=0x01;
	TMOD &= ~0X02;
	for(; t>0; t--){
		TR0=0;
		TF0=0;
		TL0=0x58;
		TL0=0x9e;
		TR0=1;
		while(TF0==0);
	}
}


void delay_us (unsigned int t){
		TMOD &= ~0x03;
		TMOD |= 0x02;
		TL0 = 0x00;
		TH0 = 0xE7;
		for(t; t>0; t--){
			while(TF0==0);
			TF0 = 0;
		}
}

unsigned char le_glcd(__bit cd, __bit cs){
	unsigned char byte;
	RW=1;
	CS1=cs;
	CS2=!cs;
	RS=cd;
	NOP4(); //espera 160ns
	E=1;
	NOP8(); //espera 320 ns
	SFRPAGE=CONFIG_PAGE;
	byte=DB;
	SFRPAGE=LEGACY_PAGE;
	NOP4(); //espera 160ns
	E=0;
	NOP12(); //espera 480ns
	return(byte);
}


void esc_glcd(unsigned char byte,__bit cd,__bit cs){
	while(le_glcd(CO,cs) & 0x80);
		RW = des;
		CS1 = cs;
		CS2 = !cs;
		RS = cd;
		SFRPAGE = CONFIG_PAGE;
		DB = byte;
		SFRPAGE = LEGACY_PAGE;
		NOP4();
		E = lig;
		NOP12();
		E = des;
		SFRPAGE = CONFIG_PAGE;
		DB = 0xFF;
		SFRPAGE = LEGACY_PAGE;
		NOP12();
}


void espacamento_glcd(unsigned char e, __bit cs){
	unsigned char i = 0;
	for(;i<e;i++)
		esc_glcd(0x00,DA,cs);
}

void glcd_caracter(unsigned int indexFonte, __bit cs){
	unsigned char i = 0;
	for(;i<5;i++)
		esc_glcd(fonte[indexFonte][i],DA,cs);
}


void Ini_glcd(void){
	E = 0;
	RST = 1;
	CS1 = 1;
	CS2 = 1;
	SFRPAGE = CONFIG_PAGE;
	DB = 0xFF;
	while(le_glcd(CO,0) & 0x10);
		while(le_glcd(CO,1) & 0x10);
			esc_glcd(0x3F,CO,ESQ); //ligar controlador da esquerda
			esc_glcd(0x3F,CO,DIR); //ligar controlador da direita
			esc_glcd(0x40,CO,ESQ); // Y=0
			esc_glcd(0xB8,CO,ESQ); // X=0 (página)
			esc_glcd(0xC0,CO,ESQ); // Z=0
			esc_glcd(0x40,CO,DIR); // Y=0
			esc_glcd(0xB8,CO,DIR); // X=0 (página)
			esc_glcd(0xC0,CO,DIR); // Z=0

}

void conf_Y(unsigned char y, __bit cs){
	y &= 0x7F;
	y |= 0x40;
	esc_glcd(y,CO,cs);
}

void conf_pag(unsigned char pag, __bit cs){
	pag &= 0x07;
	pag |= 0xB8;
	esc_glcd(pag,CO,cs);
}

void limpa_glcd(__bit cs){
	unsigned char i, j;
	for(i=0;i<8;i++){
		conf_pag(i,cs);
		conf_Y(0,cs);
		for(j=0;j<64;j++)
			esc_glcd(0x00,DA,cs);
	}
}

void putchar(char c){
	static int cont;
	__bit aux;
	unsigned int i = 0;
	if(c < 9){
		conf_pag(c-1,ESQ);
		conf_Y(0,ESQ);
		conf_pag(c-1,DIR);
		conf_Y(0,DIR);
		cont=0;
	}
	else{
		if (cont < 8)
			aux = ESQ;
		else
			aux = DIR;
		for(;i<5;i++)
			esc_glcd(fonte[c-32][i],DA,aux);
		espacamento_glcd(3,aux);
		cont++;
	}
}

void main(void){
	unsigned char i = 0;
	Init_Device();
	SFRPAGE=LEGACY_PAGE;
	Ini_glcd();
	limpa_glcd(ESQ);
	limpa_glcd(DIR);
	printf_fast_f("\x01Guilherme");
	printf_fast_f("\x02& Rafael Torres");


	while(1);
}