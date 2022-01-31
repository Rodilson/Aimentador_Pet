/*
  ModMotor.h - biblioteca criada para projeto de microcontroladores
  Driver A4899 - Esp32 Dev Mod
  Autor Rodilson/Gabriel
  MIT License
  referência - https://www.filipeflop.com/blog/desenvolvendo-uma-biblioteca-arduino/
*/
 
#include "ModMotor.h"
 
ModMotor::ModMotor(uint8_t Step, uint8_t Direct, uint8_t Enable)
{
    pinMode(Step, OUTPUT);
    pinMode(Direct, OUTPUT);
    pinMode(Enable, OUTPUT);
    step = Step;               // pino de step do driver A4988
    direct = Direct;           // pino de direção do driver A4988
    enable = Enable;           // pino de habilitação do driver A4988
}
 /*método de avanço do motor*/
void ModMotor::avanca(uint16_t velocidade)
{
    digitalWrite(enable, LOW );   //pino de habilitação do driver (LOW habilita o driver).
    digitalWrite(direct, LOW );   //pino de seleção do sentido( LOW para frente,HIGH para traz).
    for(int i ; i<=100 ; i++)
    {
    digitalWrite(step, HIGH);
    delay(velocidade/2);          //seta a velocidade em ppm "pulsos por minuto".
    digitalWrite(step, LOW); 
    delay(velocidade/2);          
    delay(velocidade/2);          //seta a velocidade em ppm "pulsos por minuto".
    }
    digitalWrite(enable, HIGH );  //pino de habilitação do driver (HIGH desabilita o driver, para economia de energia e estender sua vida útil ).
}
 
void ModMotor::recua(uint16_t velocidade)
{
    digitalWrite(enable, LOW );    //pino de habilitação do driver (LOW habilita o driver).
    digitalWrite(direct, HIGH );   //pino de seleção do sentido( LOW para frente,HIGH para traz).
    for(int i ; i<=100 ; i++)
    {
    digitalWrite(step, HIGH);
    delay(velocidade/2);          //seta a velocidade em ppm "pulsos por minuto".
    digitalWrite(step, LOW); 
    delay(velocidade/2);          
    delay(velocidade/2);          //seta a velocidade em ppm "pulsos por minuto".
    }
    digitalWrite(enable, HIGH );  //pino de habilitação do driver (HIGH desabilita o driver, para economia de energia e estender sua vida útil ).
}
//função para descarregar a bandeja.

void ModMotor::descarga(uint16_t velocidade)
{
    digitalWrite(enable, LOW );   //pino de habilitação do driver (LOW habilita o driver).
    digitalWrite(direct, LOW );   //pino de seleção do sentido( LOW para frente,HIGH para traz).
    for(int i ; i<=100 ; i++)     // gira 180° para frente
    {
    digitalWrite(step, HIGH);
    delay(velocidade/2);          //seta a velocidade em ppm "pulsos por minuto".
    digitalWrite(step, LOW); 
    delay(velocidade/2);          
    delay(velocidade/2);          //seta a velocidade em ppm "pulsos por minuto".
    }
    delay(100);
    digitalWrite(direct, HIGH );   //pino de seleção do sentido( LOW para frente,HIGH para traz).
    for(int i ; i<=100 ; i++)      // gira 180° de volta
    {
    digitalWrite(step, HIGH);
    delay(velocidade/2);          //seta a velocidade em ppm "pulsos por minuto".
    digitalWrite(step, LOW); 
    delay(velocidade/2);          
    delay(velocidade/2);          //seta a velocidade em ppm "pulsos por minuto".
    }
    digitalWrite(enable, HIGH ); 
}
  
void ModMotor::para()
{
   digitalWrite(step, LOW);
   digitalWrite(enable, HIGH );   //pino de habilitação do driver (LOW habilita o driver). 
}