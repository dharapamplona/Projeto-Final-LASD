/*
 * GccApplication3.c
 
 * Author : Dhara Pamplona
 */ 

#include <avr/io.h>
#include "i2c.h" // biblioteca necessária para comunicção com LCD
#include "LCD_I2C.h" // biblioteca que permite a programação do LCD
#define F_CPU 16000000UL //Frequência de trabalho da CPU
#include <util/delay.h>
#include <avr/interrupt.h>
#define tam_vetor 3 

unsigned char leitura_ADC_string[tam_vetor]; // recebe o tamanho 3 que é quantidade de digitos maxima 
unsigned char dutycycle_string[tam_vetor]; // recebe o tamanho 3 que é quantidade de digitos maxima

uint16_t leitura_ADC = 0; // variavel que está sendo associada ao sensor de temperatura
uint8_t dutycycle = 0;  // responsavel por receber o valor da temperatura e mostrar a % de quão quente está a placa 


ISR(ADC_vect)
{
	leitura_ADC = ADC; // ADC vai ta constantemente recebendo os valores de enviados por leitura_ADC
}


//Conversão de inteiro para string

void int2string(unsigned int valor,unsigned char *disp)
{
	// funçaõ que converte o valor para char 
	
	unsigned int unidade = valor %10;
	unsigned int dezena  = (valor /10)%10;
	unsigned int centena = (valor/100)%10;
	*(disp + 3) = 0 ; // 0 é nada em char 
	*(disp + 2) = unidade +  48 ; // soma zero(que em char é representado por 48) ao valor valor
	*(disp + 1) = dezena  +  48 ; // soma zero(que em char é representado por 48) ao valor valor
	*(disp + 0) = centena +  48 ; // soma zero(que em char é representado por 48) ao valor valor
	
}

 // essa função é responsavel por travar o valor em 100 %
 
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
	
	i2c_init(); // inicializa o protocolo de comunicação
	i2c_start(); //att 
	i2c_write(0x70); // determinação de endereço de protocolo de comunicção
	lcd_init(); //inicializa o lcd
	
	
	lcd_cmd(0x80); // escolhendo o local que será escrita uma mensagem
	lcd_msg("Temperatura:"); // mensagem 
	
	lcd_cmd(0x8F); // escolhendo o local que será escrita uma mensagem
	lcd_msg("C"); // mensagem
	
	lcd_cmd(0xCF); // escolhendo o local que será escrita uma mensagem
	lcd_msg("%");// mensagem
	
	lcd_cmd(0xC0); // escolhendo o local que será escrita uma mensagem
	lcd_msg("Vel. Motor:");// mensagem
	
    while (1) 
    {
		
		dutycycle = limitante ((leitura_ADC*1.554878049),255); 
		
		int2string (leitura_ADC*0.487,leitura_ADC_string);
		lcd_cmd(0x8C); // escolhendo o local que será escrita uma mensagem 
		lcd_msg(leitura_ADC_string);// mensagem
		
		int2string (dutycycle*0.3921568627,dutycycle_string);
		lcd_cmd(0xCC); // escolhendo o local que será escrita uma mensagem
		lcd_msg(dutycycle_string);// mensagem

		OCR0A = dutycycle ; // o temp que ele passa em nivel logico alto 
		
		
    }
}

/* 1,5v = 307 (máximo)
tensão sensor de temperatura  = leitura_adc 
 
 leitura_adc = (tensão s .t * 307)/1.5
 
 tensao de temp = leitura_adc *1.5/307
 
 para se obter a temperatura do sensor podemos utilizar o fator de escala que relaciona tensão com temperatura em ºC
 
  tensao  = leitura_adc *1.5/307
  
  graus*10*10^-3 = leitura_adc *1.5/307 
  graus = (leitura_adc*1.5/307)/*10*10^-3 
  graus = leitura_adc*0.4875993485
  
  
  
  */