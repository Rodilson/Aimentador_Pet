#include <ModMotor.h>
#include "HX711.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//definição dos pinos usados no projeto
#define DT 19
#define SCK 18

#define start 23
#define ipause 15
#define voltar 16

#define step 35
#define step2 25
#define dir 34
#define dir2 33
#define enable 32
#define enable2 26
 
float peso;
int pulso;

// *******************************
//Botões
#define botao_pra_baixo
#define botao_pra_cima
#define botao_de_selecao


int cnt;
int *cont = &cnt;

//typedef enum telas {Item1 = 1, Item2 , Iten3};
typedef enum telas
{
  Tela0,
  Tela1,
  Tela2,
  Tela3,
  Tela_Menu
} telas_t;
//typedef enum jobs 
typedef enum jobs
{
  job0,
  job1,
  job2,
  job3,
  job_Menu
} jobs_t;
// declaração de ponteiros
telas tl = Tela_Menu;
telas *Tela_de_destino = &tl;
telas posicao_do_menu = Tela_Menu;
telas *Opcao = &posicao_do_menu;
telas jb = Tela_Menu;
telas *job_ativo = &jb;

//variaveis para debounce

#define TEMPO_DEBOUNCE 300 //ms
unsigned long timestamp_ultimo_acionamento = 0;

//criação de instancias
ModMotor motor;
ModMotor motor2;
HX711 balanca;
LiquidCrystal_I2C lcd(0x27, 16, 2);
// Protótipo de Interrupcao
void IRAM_ATTR interruptnew();

//Manipuladoes de tarefas  taskHandle
TaskHandle_t task_modo0;
TaskHandle_t task_modo1;
TaskHandle_t task_modo2;
TaskHandle_t task_modo3;
TaskHandle_t task_Menu;

//protótipos de funções do sistema
void vMenu(void *pvParameters);
void vModo0(void *pvParameters);
void vModo1(void *pvParameters);
void vModo2(void *pvParameters);
void vModo3(void *pvParameters);
void tela0(void *pvParameters);
void telaMenu(void *pvParameters);
void tela0(void *pvParameters);

void menu(void *pvParameters)
{
  uint8_t retornar = 0;
  uint8_t btn = 0;
  uint8_t ulm_leit = 0;
  *Opcao = Tela0;
  telas posicao_anterior_do_menu = Tela1;
  Serial.println("iniciando menu");

  while (retornar == 0)
  {
    Serial.println("tela");
    Serial.println(analogRead(13));
    delay(100);

    if (posicao_anterior_do_menu != *Opcao)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Menu de Jobs");
      lcd.setCursor(1, 1);
      lcd.print("opcao");
      lcd.setCursor(8, 1);
      lcd.print(*Opcao);
      Serial.println("tela menu");
      
      posicao_anterior_do_menu = *Opcao;
    }
    //código responsável por ler botões, encoder e etc pra mudar a posição selecionada no menu
    //Serial.println(analogRead(13));
    if (analogRead(13) >= 700 && analogRead(13) <= 900 && (ulm_leit - millis() >= 100))
    {
      btn = 10;
      ulm_leit = millis();
    }
    else if (analogRead(13) >= 1900 && analogRead(13) <= 2050 && (ulm_leit - millis() >= 100))
    {
      btn = 20;
      ulm_leit = millis();
    }

    if (btn == 10 && *Opcao == Tela_Menu)
    {
      *Opcao = Tela0;
      Serial.println("posiçâo tela0");
      
      delay(500);
      btn = 0;
    }
    else if (btn == 10 && *Opcao == Tela0)
    {
      *Opcao = Tela1;
      Serial.println("posiçâo tela1");
      delay(500);
      btn = 0;
    }
    else if (btn == 10 && *Opcao == Tela1)
    {
      *Opcao = Tela2;
      Serial.println("posiçâo tela2");
      delay(500);
      btn = 0;
    }
    else if (btn == 10 && *Opcao == Tela2)
    {
      *Opcao = Tela3;
      Serial.println("posição tela3");
      delay(500);
      btn = 0;
    }
    else if (btn == 20 && *Opcao == Tela3)
    {
      *Opcao = Tela2;
      Serial.println("posição tela2");
      delay(500);
      btn = 0;
    }
    else if (btn == 20 && *Opcao == Tela2)
    {
      *Opcao = Tela1;
      Serial.println("posição tela1");
      delay(500);
      btn = 0;
    }
    else if (btn == 20 && *Opcao == Tela1)
    {
      *Opcao = Tela0;
      Serial.println("posição tela0");
      delay(500);
      btn = 0;
    }
    else if (btn == 20 && *Opcao == Tela0)
    {
      *Opcao = Tela_Menu;
      Serial.println("sair");
      delay(500);
      btn = 0;
    }
    // botão pra selecionar o item em destaque no menu
    if (analogRead(13) >= 0 && analogRead(13) <= 200)
    {
      retornar = 1;
      *Tela_de_destino = *Opcao;
      Serial.println(*Opcao);
    }
    //voltar
    else if (analogRead(13) >= 2900 && analogRead(13) <= 3100)
    {
      retornar = 1;
      *Tela_de_destino = *job_ativo;
      telas job_atual = *job_ativo;
      
      switch (job_atual)
      {
      case Tela0 /* constant-expression */:
        vTaskResume(task_modo0);
        break;

      case Tela1 /* constant-expression */:
        vTaskResume(task_modo1); /* code */
        break;

      case Tela2 /* constant-expression */:
        vTaskResume(task_modo2); /* code */
        break;
      case Tela3 /* constant-expression */:
        vTaskResume(task_modo3); /* code */
        break;
      case Tela_Menu /* constant-expression */:
        break;
      default:
        break;
      }
    }
    delay(100);
  }
  //vTaskDelete(NULL);
  delay(20);
}
void vMenu(void *pvParameters)
{

 // *job_ativo = job_Menu;
  Serial.println("opa");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Vmenu");
  lcd.setCursor(1, 1);
  lcd.print("opcao");
  lcd.setCursor(8, 1);
  lcd.print(*Opcao);
  menu(Tela_de_destino);
  Serial.println("o que houve");
  }

void gestorCiclo()
{

  //declare aqui as suas variáveis locais
  telas job_atual = *job_ativo;
  Serial.println("menu iniciado");
  switch (job_atual)
  {
  case Tela0 /* constant-expression */:
    vTaskSuspend(task_modo0); /* code */
    break;

  case Tela1 /* constant-expression */:
    vTaskSuspend(task_modo1); /* code */
    break;

  case Tela2 /* constant-expression */:
    vTaskSuspend(task_modo2); /* code */
    break;
  case Tela3 /* constant-expression */:
    vTaskSuspend(task_modo3); /* code */
    break;
  case Tela_Menu /* constant-expression */:
    break;
  default:
    break;
  }
}

void vModo0(void *pvParameters)
{
  *job_ativo = Tela0;
  while (true)
  {
    tela0(Tela_de_destino);
    Serial.println("operação em manual");
    delay(500);
    balanca.power_up();
    delay(1);
    Serial.print("Valor da Leitura:  ");
    peso = balanca.get_units(10); //recebe a média de 5 leituras
    Serial.println(peso, 3);     //exibe leitura em float com 3 casas decimais
    Serial.println(digitalRead(15));
    balanca.power_down();
    delay(100);

    pulso = digitalRead(start);

    //controla motor
    if (pulso == LOW)
    {
      // attachInterrupt(15,)
      balanca.power_up();
      delay(1);
      if (peso < 50.00)
      {
        while (peso < 50.00)
        {
          motor.avanca(100);
          peso = balanca.get_units(5);
          lcd.setCursor(1, 0);
          lcd.print("Modo Manual ");
          lcd.setCursor(0, 1);
          lcd.print("Peso: ");
          lcd.setCursor(8, 1);
          lcd.print(peso,3);

          delay(2);
          // motor.recua(50);
          //pulso = 0;
        }
        motor.recua(100);

        delay(100);
      }
      balanca.power_down();
      delay(1);
    }
  }
}
void vModo1(void *pvParameters)
{
  gestorCiclo();
  *job_ativo = Tela1;
  unsigned long t1 = 10;
  int fimDeCiclo = 0;

  while (1)
  {
    for (unsigned long i = *cont; i <= t1; i++)
    {
      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("Modo 1 ");
      lcd.setCursor(0, 1);
      lcd.print("Tempo:");
      lcd.setCursor(9, 1);
      lcd.print(i);
      *cont = i;
      delay(1000);
    }
    *cont = 0;
    lcd.clear();
    delay(10);
    lcd.setCursor(2, 0);
    lcd.print("iniciando");
    lcd.setCursor(3, 1);
    lcd.print("dispensa");
    balanca.power_up();
    Serial.println("iniciando dispensa");
    peso = balanca.get_units(5);
    Serial.println(peso);
    delay(1);
    if (peso < 50.00)
    {
      while (peso < 50.00)
      {
        motor.avanca(100);
        peso = balanca.get_units(5);
        lcd.setCursor(2, 0);
        lcd.print("ALIMENTANDO");
        lcd.setCursor(0, 1);
        lcd.print("Peso: ");
        lcd.print(peso, 3);
        Serial.println(peso);
        delay(2);
      }
      //motor.recua(100);
      motor2.descarga(100);
      motor.para();
      motor2.para();
      delay(100);
    }
    balanca.power_down();
    delay(1);
    fimDeCiclo++;
  }
  vTaskDelete(NULL);
}
void vModo2(void *pvParameters)
{
  gestorCiclo();
  *job_ativo = Tela2;
  unsigned long t1 = 240;
  int fimDeCiclo = 0;
  while (1)
  {
    for (unsigned long i = *cont; i <= t1; i++)
    {
     lcd.clear();
      lcd.setCursor(5, 1);
      lcd.print("Modo 2 ");
      lcd.setCursor(0, 1);
      lcd.print("Tempo:");
      lcd.setCursor(9, 1);
      lcd.print(i);
      Serial.print("tempo: ");
      Serial.println(i);
      *cont = i;
      delay(1000);
    }
    *cont = 0;
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("iniciando");
    lcd.setCursor(5, 1);
    lcd.print("dispensa");
    balanca.power_up();
    Serial.println("iniciando dispensa");
    delay(1);
    if (peso < 50.00)
    {
      while (peso < 50.00)
     {
      while (peso < 50.00)
      {
        motor.avanca(100);
        peso = balanca.get_units(5);
        lcd.setCursor(2, 0);
        lcd.print("TNK_Solution");
        lcd.setCursor(0, 1);
        lcd.print("Peso: ");
        lcd.print(peso, 3);
        Serial.println(peso);
        delay(2);
      }
      motor.recua(100);
      motor2.descarga(100);
      motor.para();
      delay(100);
    }
    balanca.power_down();
    delay(1);
    fimDeCiclo++;
  }
  vTaskDelete(NULL);
}
}
void vModo3(void *pvParameters)
{
  //gestorCiclo();
  *job_ativo = Tela3;
  unsigned long t1 = 480;
  int fimDeCiclo = 0;
  while (1)
  {
    for (unsigned long i = *cont; i <= t1; i++)
    {
     lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("Modo 3 ");
      lcd.setCursor(0, 1);
      lcd.print("Tempo:");
      lcd.setCursor(9, 1);
      lcd.print(i);
      Serial.print("tempo: ");
      Serial.println(i);
      *cont = i;
      delay(1000);
    }
    *cont = 0;
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("iniciando");
    lcd.setCursor(5, 1);
    lcd.print("dispensa");
    balanca.power_up();
    Serial.println("iniciando dispensa");
    delay(1);
    if (peso < 50.00)
    {
      while (peso < 50.00)
      {
        motor.avanca(100);
        peso = balanca.get_units(5);
        lcd.setCursor(2, 0);
        lcd.print("TNK_Solution");
        lcd.setCursor(0, 1);
        lcd.print("Peso: ");
        lcd.print(peso, 3);
        Serial.println(peso);
        delay(2);
      }
      motor.recua(100);
      motor.descarga(100);
      motor.para();
      delay(100);
    }
    balanca.power_down();
    delay(1);
    fimDeCiclo++;
  }
  vTaskDelete(NULL);
}

void telaMenu(void *pvParameters)
{
  Serial.println("tela menu");
  delay(500);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.println("Menu de opcoes ");
  lcd.setCursor(1, 1);
  lcd.print("opcao");
  lcd.setCursor(8, 1);
  lcd.print(*Opcao);
}
void tela0(void *pvParameters)
{
 
  Serial.println("TELA 00 EXECUTANDO");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Modo Manual");
  lcd.setCursor(0, 1);
  lcd.print("aperte o play ");

  delay(10);
  
}
void tela1(void *pvParameters)
{
  //declare aqui as suas variáveis locais
  //uint8_t retornar = 0;
  //uint8_t  modo1_ativo = true;
  /*while (retornar == 0)
  { */
  //Escreva aqui o seu código do que vai acontecer nesta tela, como mostrar ou receber dados, acender um LED, etc

  Serial.println("TELA 01 saindo");
 //xTaskCreate(vModo1, "vModo1", 10000, NULL, 0, NULL);

  delay(100);
  //Aqui é a condição de retorno/saída da tela. Pode ser um botão pressionado, uma interrupção, um comando pelo terminal.
  /*if (digitalRead(16) == 1)
    {
      retornar = 1;
      delay(20);
    }
  }
  //ao sair desta tela, retorna para o menu. Você pode mudar isto nesta linha.
  *Tela_de_destino = Tela1; */
}
void tela2(void *pvParameters)
{
  //declare aqui as suas variáveis locais
  uint8_t retornar = 0;
  while (retornar == 0)
  {
    //Escreva aqui o seu código do que vai acontecer nesta tela, como mostrar ou receber dados, acender um LED, etc

    Serial.println("TELA 02 EXECUTANDO");
    delay(100);
    //Aqui é a condição de retorno/saída da tela. Pode ser um botão pressionado, uma interrupção, um comando pelo terminal.
    if (digitalRead(16) == 1)
      retornar = 1;
    delay(20);
  }
  //ao sair desta tela, retorna para o menu. Você pode mudar isto nesta linha.
  *Tela_de_destino = Tela2;
}
void tela3(void *pvParameters)
{
  uint8_t retornar = 0;
  while (retornar == 0)
  {
    //Escreva aqui o seu código do que vai acontecer nesta tela, como mostrar ou receber dados, acender um LED, etc

    Serial.println("TELA 03 EXECUTANDO");
    delay(100);
    //Aqui é a condição de retorno/saída da tela. Pode ser um botão pressionado, uma interrupção, um comando pelo terminal.
    if (digitalRead(16) == 1)
      retornar = 1;
    delay(20);
  }
  //ao sair desta tela, retorna para o menu. Você pode mudar isto nesta linha.
  *Tela_de_destino = Tela3;
}



void setup()
{
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  // pino 23 start
  pinMode(start, INPUT_PULLUP);
  //pino 15 pause( chama interrupção)
  pinMode(ipause, INPUT_PULLUP);
  //pino 16 voltar
  pinMode(voltar, INPUT_PULLDOWN);

  //inicializa a balança
  balanca.begin(DT, SCK);
  //inicializa o motor
  motor.init(step, dir, enable);
  motor2.init(step2, dir2, enable2);
  attachInterrupt(ipause, interruptnew, FALLING);

  Serial.begin(115200);
  //Serial.println("");
  Serial.print("Leitura da Tara:  ");
  Serial.println(balanca.read()); // Aguada o termino de verificação do peso
  Serial.println("Aguarde!");
  Serial.println("Iniciando ...");
  balanca.set_scale(-436111 / 1000); // Utiliza uma balanca padrão de verificação
  delay(200);
  balanca.tare();

  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("SISTEMAS");
  lcd.setCursor(0, 1);
  lcd.print("MICROCONTROLADOS");
  delay(1000);

  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("TARA");
  lcd.setCursor(0, 1);
  lcd.print("Peso: ");
  peso = balanca.get_units(5);
  lcd.setCursor(8, 1);
  lcd.print(peso, 3);
  delay(10000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Menu de opcoes ");
  lcd.setCursor(1, 1);
  lcd.print("opcao");
  lcd.setCursor(8, 1);
  lcd.print(*Opcao);

  delay(1000);
  lcd.clear();
  //xTaskCreate(vMenu, "vMenu", 10000, NULL, 0, &task_Menu);
  vMenu(Tela_de_destino);
}

void loop()
{
  
  int destino = *Tela_de_destino;
  switch (destino)
  {
  case Tela_Menu:
  {
    //  gestorCiclo();
     vMenu(Opcao);
    break;
  }
  case Tela0:
  {
  //  gestorCiclo();
    // xTaskCreate(vModo0, "vModo0", 10000, NULL, 0, &task_modo0);
   vModo0(Tela_de_destino);
    break;
  }
  case Tela1:
  {
    //gestorCiclo();
    //xTaskCreate(vModo1, "vModo1", 10000, NULL, 0, &task_modo1);
    vModo1(Tela_de_destino);
    break;
  }
  case Tela2:
  {
    // gestorCiclo();
    // xTaskCreate(vModo2, "vModo2", 10000, NULL, 0, &task_modo2);
    vModo2(Tela_de_destino);
    break;
  }
  case Tela3:
  {
   //gestorCiclo();
   // xTaskCreate(vModo3, "vModo3", 10000, NULL, 0, &task_modo3);
    vModo3(Tela_de_destino);
    break;
  }
  }
}
void IRAM_ATTR interruptnew()
{

  if ((millis() - timestamp_ultimo_acionamento) >= TEMPO_DEBOUNCE)
  {
    Serial.println("fui interrompido");
    
    gestorCiclo();
    timestamp_ultimo_acionamento = millis();
   *Tela_de_destino = Tela_Menu;
   lcd.clear();
  }
}
