/*
  ModMotor.h - biblioteca criada para projeto de microcontroladores
  Driver A4899 - Esp32 Dev Mod
  Autor Rodilson/Gabriel
  MIT License
*/
 
// guarda de inclusão
#ifndef ModMotor_h
#define ModMotor_h
 
#include <Arduino.h>
 
class ModMotor
{
    public:
        ModMotor( uint8_t Step, uint8_t Direct, uint8_t Enable);
        void avanca(uint16_t velocidade);
        void recua(uint16_t velocidade);
        void descarga(uint16_t velocidade);
        void para();    
 
    private:
        
        uint8_t  step;
        uint8_t  direct;
        uint8_t  enable;
        uint16_t  velocidade;
};
 
#endif