/*
  ModMotor.h - biblioteca criada para projeto de microcontroladores
  Driver A4899 - Esp32 Dev Mod
  Autor Rodilson/Gabriel
  MIT License
  referência - https://www.filipeflop.com/blog/desenvolvendo-uma-biblioteca-arduino/
*/
 
#include "ModMotor.h"

 
ModMotor::ModMotor(){
}
void ModMotor::init(uint8_t Step, uint8_t Direct, uint8_t Enable)
{
    Serial.println("iniciando driver do motor");
    step = Step;               // pino de step do driver A4988
    direct = Direct;           // pino de direção do driver A4988
    enable = Enable;           // pino de habilitação do driver A4988
    

    pinMode(Step, OUTPUT);
    pinMode(Direct, OUTPUT);
    pinMode(Enable, OUTPUT);
  
    delay(10);
    digitalWrite(step, LOW);
    digitalWrite(direct, LOW);
    digitalWrite(enable, HIGH);
   
    delay(10);
}

/*método de avanço do motor*/
void ModMotor::avanca(uint16_t velocidade)
{
    //uint8_t i;
    Serial.println("Avançando");
    delay(1);
    uint16_t pps = (500/velocidade)/2 ;
    Serial.println(pps);
    digitalWrite(enable, LOW );   //pino de habilitação do driver (LOW habilita o driver).
    digitalWrite(direct, HIGH );   //pino de seleção do sentido( LOW para frente,HIGH para traz).
    for(int i=0 ; i<=200 ; i++)//while(1)
  {
      digitalWrite(step, HIGH);
      delay(pps);          //seta a velocidade em ppm "pulsos por minuto".
      digitalWrite(step, LOW); 
      delay(pps);          //seta a velocidade em ppm "pulsos por minuto".
    
      } //Serial.println(digitalRead(enable));  //pino de habilitação do driver (HIGH desabilita o driver, para economia de energia e estender sua vida útil ).
}
 
void ModMotor::recua(uint16_t velocidade)
{
    //uint8_t j;
    Serial.println("Recuando");
    delay(10);
    uint16_t pps2 = (500/velocidade)/2 ;
    digitalWrite(enable, LOW );    //pino de habilitação do driver (LOW habilita o driver).
    digitalWrite(direct, HIGH );   //pino de seleção do sentido( LOW para frente,HIGH para traz).
    for(int j=0 ; j<=100 ; j++)
    {
    digitalWrite(step, HIGH);
    delay(pps2);          //seta a velocidade em ppm "pulsos por minuto".
    digitalWrite(step, LOW); 
    delay(pps2);          //seta a velocidade em ppm "pulsos por minuto".
    }
    digitalWrite(enable, HIGH );  //pino de habilitação do driver (HIGH desabilita o driver, para economia de energia e estender sua vida útil ).
}
//função para descarregar a bandeja.

void ModMotor::descarga(uint16_t velocidade)
{
    //uint8_t x;
    //uint8_t y;
   
    Serial.println("Descarregando");
    delay(1000);
    uint16_t pps3 = (1000/velocidade)/2 ;
    digitalWrite(enable, LOW );   //pino de habilitação do driver (LOW habilita o driver).
    digitalWrite(direct, LOW );   //pino de seleção do sentido( LOW para frente,HIGH para traz).
    for(int x=0 ; x<=100 ; x++)     // gira 180° para frente
    {
    digitalWrite(step, HIGH);
    delay(pps3);          //seta a velocidade em ppm "pulsos por minuto".
    digitalWrite(step, LOW); 
    delay(pps3);          //seta a velocidade em ppm "pulsos por minuto".
    }
    delay(100);
    Serial.println("180°");
    digitalWrite(direct, HIGH );   //pino de seleção do sentido( LOW para frente,HIGH para traz).
    for(int y=0 ; y<=100 ; y++)      // gira 180° de volta
    {
    digitalWrite(step, HIGH);
    delay(pps3);          //seta a velocidade em ppm "pulsos por minuto".
    digitalWrite(step, LOW); 
       delay(pps3);       //seta a velocidade em ppm "pulsos por minuto".
    }
    Serial.println("0°");
    digitalWrite(enable, HIGH );
    digitalWrite(direct, LOW);
    
}
  
void ModMotor::para()
{
    Serial.println("Parando");
    delay(10);
    digitalWrite(step, LOW);
    digitalWrite(enable, HIGH );
       //pino de habilitação do driver (LOW habilita o driver). 
}
