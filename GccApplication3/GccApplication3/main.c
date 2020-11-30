/*
 * GccApplication3.c
 *
 * Created: 29/11/2020 17:15:06
 * Author : Dhara Pamplona
 */ 

#include <avr/io.h>
#include "i2c.h"
#include "LCD_I2C.h"
#define F_CPU 16000000UL //Frequência de trabalho da CPU
#include <util/delay.h>
#include <avr/interrupt.h>
#define tam_vetor 3

unsigned char leitura_ADC_string[tam_vetor];
unsigned char dutycycle_string[tam_vetor];

uint16_t leitura_ADC = 0;
uint8_t dutycycle = 0; 


ISR(ADC_vect)
{
	leitura_ADC = ADC;
}


//Conversão de inteiro para string



void int2string(unsigned int valor,unsigned char *disp)
{
	
	
	unsigned int unidade = valor %10;
	unsigned int dezena  = (valor /10)%10;
	unsigned int centena = (valor/100)%10;
	*(disp + 2) = unidade +  48 ;
	*(disp + 1) = dezena  +  48 ;
	*(disp + 0) = centena +  48 ;
	
}

uint8_t limitante(uint16_t numero, uint8_t maxi)
{
	if (numero <= maxi){
		return numero;
	}
	else {
		return maxi;
	}	
}


int main(void)
{
	
    DDRD = 0xFF; //Porta D como saída
    DDRC = 0x00; //Porta C como entrada
    PORTC = 0xFE; //Desabilita o pullup do PC0
	
	ADMUX = 0b01000000; // refência definida por har.
	
	ADCSRA = 0b11101111; //habilita o AD, habilita interrupção, modo de conversão contínua, prescaler = 128
	ADCSRB = 0x00; //modo de conversão contínua
	DIDR0 = 0b00001110; //habilita pino PC0 como entrada do ADC0
	TCCR0A = 0b10100011; //pwm rapido, modo de comparaçao, modo de funcionamento
	TCCR0B = 0b00000101; //1024, frequencia de 61hz
	
	sei();
	
	i2c_init(); // inicializa 
	i2c_start(); //att 
	i2c_write(0x70); // 
	lcd_init(); //
	
	
	lcd_cmd(0x80);
	lcd_msg("Temp: ");
	lcd_cmd(0x89);
	lcd_msg("*C"); 
	lcd_cmd(0xCC);
	lcd_msg("%");
	
    while (1) 
    {
		
		dutycycle = limitante ((leitura_ADC*1.554878049),255);
		
		int2string (leitura_ADC*0.487,leitura_ADC_string);
		lcd_cmd(0x86);
		lcd_msg(leitura_ADC_string);
		
		int2string (dutycycle*0.3921568627,dutycycle_string);
		lcd_cmd(0xCD);
		lcd_msg(dutycycle_string);

		
		
		
    }
}
/*
 void int2string(unsigned int valor, unsigned char *disp)
{
for(uint8_t n=0; n<tam_vetor; n++)
disp[n] = 0 + 48; //limpa vetor para armazenagem dos digitos
disp += (tam_vetor-1);
do
{
*disp = (valor%10) + 48; //pega o resto da divisão por 10
valor /=10; //pega o inteiro da divisão por 10
disp--;
}while (valor!=0);
}*/

//1,5v = 307 (máximo)
//duty = leitura_adc * 0.83 *0.3921568627