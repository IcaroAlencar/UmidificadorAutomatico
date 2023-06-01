#include <16F877A.h>
#device ADC=16
#fuses HS,NOWDT,NOPROTECT,NOLVP  

#use delay(crystal=20000000)

#use fast_io(B)
#define DHT11_PIN PIN_B4    // Pino do sensor DHT11
#define OUT PIN_B1          // Pino do Rele
#define LED PIN_D2          // Pino do Led Indicador de Leitura

#ifndef lcd_enable
   #define lcd_enable     pin_D0      // pino enable do LCD
   #define lcd_rs         pin_D1      // pino rs do LCD
   #define lcd_d4         pin_D4      // pino de dados d4 do LCD
   #define lcd_d5         pin_D5      // pino de dados d5 do LCD
   #define lcd_d6         pin_D6      // pino de dados d6 do LCD
   #define lcd_d7         pin_D7      // pino de dados d7 do LCD
#endif

#include "mod_lcd.c"

char message1[] = "Temp = 00.0 C  ";
char message2[] = "Umid = 00.0%  ";
short Time_out;

int8 teste = 0; // Usado para testes
short int isLigado = 0;

unsigned int8 T_byte1, T_byte2, RH_byte1, RH_byte2, CheckSum ;

// Envia um sinal para inciar a comunica��o entre o pic e o sensor DHT11
void sinalInicial(){
   output_drive(DHT11_PIN);   // ajustando o pino de conex�o como sa�da 
   output_low(DHT11_PIN);     // ajustando a sa�da do pino de conex�o para baixo 
   delay_ms(18);
   output_high(DHT11_PIN);    // ajustando a sa�da do pino de conex�o para alto
   delay_us(20);
   output_float(DHT11_PIN);   // ajustadno o pino de conex�o para o modo de entrada
}

// Recebe uma resposta do sensor DHT11
short obtendoResposta(){
   delay_us(40);
   if(!input(DHT11_PIN)){        // lendo e testando se a conex�o do pino est� em baixa
      delay_us(80);
      if(input(DHT11_PIN)){      // lendo e testando se a conex�o do pino est� em alta
         delay_us(50);
         return 1;               // Tudo Certo retorna 1
      }else{
         teste = 1;              // Erro 1
      }
   }else{
      teste = 2;                 // Erro 2
   }
}

// Faz a leitura dos dados enviados pelo sensor DHT11
unsigned int8 lendoDados(){
   unsigned int8 i, k, _data = 0;        // k � usado para contar a dura��o da leitura de 1 bit
   if(Time_out)
      break;
   for(i = 0; i < 8; i++){
      k = 0;
      while(!input(DHT11_PIN)){           // Aguarde at� que o pino DHT11 seja levantado
         k++;
         if(k > 100){
            Time_out = 1;
            break;                        // se demorar muito timeOut retorna com o valor 1 mostrando que houve um erro
         }
         delay_us(1);
      }
      delay_us(30);
      if(!input(DHT11_PIN))
         bit_clear(_data, (7 - i));        // Limpa o bit (7 - i)
      else{
         bit_set(_data, (7 - i));          // ajusta o bit (7 - i)
         while(input(DHT11_PIN)){          // Aguarde at� que o pino DHT11 fique baixo
            k++;
            if(k > 100){
               Time_out = 1;
               break;
            }
            delay_us(1);
         }
      }
  }
  return _data;
}

void main(){

   lcd_ini();                                   // Inicializa LCD
   delay_ms(10); 
   
   printf (lcd_escreve,"\f Iniciando..."); 
   delay_ms(1000);
   
   output_drive(OUT);                           // ajustando o pino de conex�o como sa�da 
   output_low(LED);                             // Desligando o led de leitura
  
   while(TRUE){
      output_high(LED);                         // Ligando o led de leitura
      printf (lcd_escreve,"\f Lendo...");
      delay_ms(1000);
      
      Time_out = 0;
      sinalInicial();                           // Enviando um sinal para o sensor.
      
      if(obtendoResposta()){                     // Se o sensor reponder
      
         RH_byte1    = lendoDados();                 // lendo o byte1 da umidade
         RH_byte2    = lendoDados();                 // lendo o byte2 da umidade
         T_byte1     = lendoDados();                 // lendo o byte1 da temperatura
         T_byte2     = lendoDados();                 // lendo o byte2 da temperatura
         Checksum    = lendoDados();                 // lendo o checksum
         
         if(Time_out){                                // se a leitura demorar muito
            printf(lcd_escreve,"\f");                 // LCD clear
            printf(lcd_escreve,"Abacou seu tempo");   // Escreve a mensagem de erro
            delay_ms(1000);
         }
         else{
            if(CheckSum == ((RH_Byte1 + RH_Byte2 + T_Byte1 + T_Byte2) & 0xFF)){ // se a soma de todos os dados forem iguais ao codigo verificador 
               
               // Forma de estruturar os dados de inteiro para string
               // pega o inteiro divide por 10 pra pegar apenas a casa decimal, depois soma 48 pra pega o valor da tabela ascii
               
               // Temperatura
               message1[7]  = T_Byte1/10  + 48;  // pega o inteiro divide por 10 pra pegar apenas a casa decimal, depois soma 48 pra pega o valor da tabela ascii
               message1[8]  = T_Byte1%10  + 48;
               message1[10] = T_Byte2     + 48;
               message1[11] = 223;                   // Simbolo de Graus �
               
               // Umidade
               message2[7]  = RH_Byte1/10 + 48;
               message2[8]  = RH_Byte1%10 + 48;
               message2[10] = RH_Byte2    + 48;
              
               printf(lcd_escreve,"\f%s", message1);
               delay_ms(10);
               printf(lcd_escreve,"\n%s", message2);
               delay_ms(10);
               
               // Verifica se a umidade esta abaixo de 70%, se for verdadeiro liga o rele, se nao desliga o rele
               if(RH_Byte1 <= 70 && isLigado == 0){
                  output_toggle(OUT); 
                  isLigado = 1;
               }
               if(RH_Byte1 >= 80 && isLigado == 1){
                  output_toggle(OUT);
                  isLigado = 0;
               }
               
               output_low(LED);
               
               // Espera 60 segudos para fazer a leitura novamente
               for(int8 i = 0; i < 60; i++){
                  delay_ms(1000);
               }
            }
            else{
               printf(lcd_escreve,"\f");  
               printf(lcd_escreve,"Erro na Validacao!");
               delay_ms(1000);
            }
         }
      }
      else {
         printf(lcd_escreve,"\f");
         printf(lcd_escreve,"Sem resposta do");
         printf(lcd_escreve,"\nsensor");
         delay_ms(1000);
      }
      
   }
}
