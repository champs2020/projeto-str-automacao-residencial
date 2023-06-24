#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/event_groups.h>
#include <esp_log.h>
#include "esp_timer.h"
#include "driver/gpio.h"
 
 // Definindo Pino que servirá para interrupção
 #define BTN_PIN0 0

// Declarando fila para interrupção
xQueueHandle fila_ittr_campainha;

// Declarando Semaphore
xSemaphoreHandle ligarMQTT;
EventGroupHandle_t eventGroupConectividade;
const int conexaoWifi = BIT0;
const int conexaoMQTT = BIT1;

// Declarando Handle de notificação
static TaskHandle_t handler_notifica_alerta = NULL;
 
void conectaWifi (void * params) 
{
    uint64_t start;
    uint64_t end;
    int flag_conexao;

    printf("WIFI: Conectando WiFi...");

    while (true)
    {
        start = esp_timer_get_time(); // init time
        printf("\n\n*******************************    TASK CONECTA WIFI    ************************************\n");
        // Gerando flag aleatória de 1 a 3
        flag_conexao =  ((int) ((float) rand() / (float) (RAND_MAX/4)) );

        if (flag_conexao != 3)
        {
            printf("WIFI Conexão estável com WiFi! Flag:%d\n", flag_conexao);
            xEventGroupSetBits(eventGroupConectividade, conexaoWifi);
            xSemaphoreGive(ligarMQTT);
        } else
        {
            printf("WIFI Falha na conexão com WiFi! Nova tentativa será efetuada! Flag: %d\n", flag_conexao);
        }
        
        end = esp_timer_get_time(); // end time
        ESP_LOGW("MQTT", "Execution Time: [%llu] microseconds\n", (end-start));
        
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    
}

void conectaMQTT (void * params)
{
    uint64_t start;
    uint64_t end;
    int flag_conexao;

    printf("MQTT: Conectando com Servidor MQTT!\n");

    while (true)
    {
        xSemaphoreTake(ligarMQTT, portMAX_DELAY);
        start = esp_timer_get_time(); // init time
        printf("\n*******************************    TASK CONECTA MQTT    ************************************\n");

        // Gerando flag aleatória de 1 e 3
        flag_conexao =  ((int) ((float) rand() / (float) (RAND_MAX/4)) );

        if (flag_conexao != 3)
        {
            printf("MQTT: Conexão com Servidor MQTT Estável! Flag:%d\n", flag_conexao);
            xEventGroupSetBits(eventGroupConectividade, conexaoMQTT);

        } else
        {
            printf("MQTT: Falha na conexão com Servidor MQTT! Nova tentativa será efetuada! Flag: %d\n", flag_conexao);
        }

        end = esp_timer_get_time(); // end time
        ESP_LOGW("MQTT", "Execution Time: [%llu] microseconds\n", (end-start));

    }
    
}

void envio_dados (void * params) {
    uint64_t start;
    uint64_t end;
    while (true)
    { 
        // Aguardar Wifi e MQTT
        xEventGroupWaitBits(eventGroupConectividade, conexaoWifi | conexaoMQTT, true, true, portMAX_DELAY);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
        start = esp_timer_get_time(); // init time

        printf("\n*******************************    TASK ENVIO DADOS    ************************************\n");
        printf("ENVIO: Enviando Dados para o Servidor\n");

        end = esp_timer_get_time(); // end time
        ESP_LOGW("ENVIO", "Execution Time: [%llu] microseconds\n", (end-start));
    }
    
}

static void IRAM_ATTR gpio_isr_handler(void *args)
{
    int sts_campainha = (int) args;
    xQueueSendFromISR(fila_ittr_campainha, &sts_campainha, NULL);
    // Adicionar prioridade
}


// Task para tratar a interrupção do sinal da campainha
void ittr_botao_campainha(void *params)
{
    int sts_campainha = 0;
    int contador = 0;

    while (1)
    {
        uint64_t start;
        uint64_t end;
        // Será uma chamada sob espera por tempo indeterminado
        if (xQueueReceive(fila_ittr_campainha, &sts_campainha, portMAX_DELAY))
        {
            start = esp_timer_get_time(); // init time
            contador++;
            printf("\n\n******************************* TASK CAMPAINHA ************************************\n");
            printf("\nA CAMPAINHA foi acionada! Apertada pela %d° vez\n", contador);

            end = esp_timer_get_time(); // end time
            ESP_LOGW("Campainha", "Execution Time: [%llu] microseconds\n", (end-start));
        }
        
    }
    
}

 void leitura_sensor_umidade (void * params)
{
    float umidade;
    uint64_t start;
    uint64_t end;
    while (true)
    {
        start = esp_timer_get_time(); // init time

        umidade =  30.0 + ((float) rand() / (float) (RAND_MAX/70));
        printf("\n\n*******************************      TASK UMIDADE       ************************************\n");

        if(umidade < 60)
        {
            printf("UMIDADE: %.2f%% | Umidade preocupante! Utilize umidificadores e tome bastante água.\n", umidade);
        }else if(umidade < 80)
        {
            printf("UMIDADE: %.2f%% | Umidade agradável! \n", umidade);
        } else
        {
            printf("UMIDADE: %.2f%% | Ambiente úmido! Aumente a ventilação na residêncial e tome cuidado com fungos e problemas respiratórios\n", umidade);
        }



        end = esp_timer_get_time(); // end time
        ESP_LOGW("Umidade", "Execution Time: [%llu] microseconds\n", (end-start));
        vTaskDelay(8000 / portTICK_PERIOD_MS);
    }
    
}

void leitura_sensor_temperatura (void * params)
{
    float temperatura;
    uint64_t start;
    uint64_t end;
    while (true)
    {
        start = esp_timer_get_time(); // init time


        temperatura =  20.0 + ((float) rand() / (float) (RAND_MAX/20));
        printf("\n\n*******************************    TASK TEMPERATURA     ************************************\n");
        
        if(temperatura < 27)
        {
            printf("TEMPERATURA: %.2f°C | Ambiente agradável!\n", temperatura);
        }else if(temperatura < 37)
        {
            printf("TEMPERATURA: %.2f°C | Cuidado! Tome bastante água e se possível acione ventiladores e ar-condicionados\n", temperatura);
        } else
        {
            printf("TEMPERATURA: %.2f°C | Risco de Vida! Se atente a incêndios e se desloque para  uma localidade segura\n", temperatura);
            xTaskNotifyGive(handler_notifica_alerta);
        }


        end = esp_timer_get_time(); // end time
        ESP_LOGW("TEMPERATURA", "Execution Time: [%llu] microseconds\n", (end-start));
        vTaskDelay(5000 / portTICK_PERIOD_MS);

    }
}

void leitura_sensor_presenca (void * params)
{
    int presenca;
    uint64_t start;
    uint64_t end;
    while (true)
    {
        printf("\n\n*******************************      TASK PRESENÇA       ************************************\n");
        start = esp_timer_get_time(); // init time

        presenca = (int) (rand() % 2);
        ESP_LOGI("Presença", "%d", presenca);

        // O acionamento das lampadas será baseado na presença de pessoas
        if (presenca)
        {   
            printf("PRESENÇA: Não há pessoas no ambiente! | Iluminação Desigada! | Flag: %d\n", presenca);

        } else
        {
            printf("PRESENÇA: Há pessoas no ambiente! | Iluminação Ligada! | %d\n", presenca);
        }
        
        


        end = esp_timer_get_time(); // end time
        ESP_LOGW("Presença", "Execution Time: [%llu] microseconds\n", (end-start));
        vTaskDelay(6000 / portTICK_PERIOD_MS);
    } 

}

void leitura_sensor_fumaca (void * params)
{
    uint64_t start;
    uint64_t end;
    int fumaca;
    while (true)
    {
        printf("\n\n**********************************     TASK FUMAÇA     ***************************************\n");
        start = esp_timer_get_time(); // init time

        fumaca = (int) (rand() % 3);

        // O acionamento das lampadas será baseado na presença de pessoas
        if (fumaca)
        {   
            printf("FUMAÇA: Não Há Fumaça no Ambiente | Flag: %d\n", fumaca);
        } else
        {
            printf("FUMAÇA: Há Fumaça no Ambiente! | Alerta! | %d\n", fumaca);
            xTaskNotifyGive(handler_notifica_alerta);
        }


        end = esp_timer_get_time(); // end time
        ESP_LOGW("Fumaça", "Execution Time: [%llu] microseconds\n", (end-start));
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    } 

}
void alerta_notificacao_receptor()
{
    int quantidade;
    while (1)
    {
        quantidade = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        printf("\n\n**********************************     TASK NOTIFICACÃO     ***************************************\n");
        printf("\n NOTIFICAÇÃO: RISCO DE INCÊNCDIO em Sua Residencia Notificação de alerta MÁXIMO recebido! .\n%d notificações recebidas\n", quantidade);
    }
    
}

void app_main(void)
{
    // Configuração do pino da Campainha
    gpio_pad_select_gpio(BTN_PIN0);
    gpio_set_direction(BTN_PIN0, GPIO_MODE_INPUT);
    gpio_pullup_dis(BTN_PIN0);

    // Configurando botão para interrupção
    gpio_set_intr_type(BTN_PIN0, GPIO_INTR_POSEDGE);

    // Inicializando fila para interrupção da campainha
    fila_ittr_campainha = xQueueCreate(5, sizeof(float));

    // Inicializar EventGroups
    eventGroupConectividade = xEventGroupCreate();
    // Inicializar Semaphore Binario
    ligarMQTT = xSemaphoreCreateBinary();

    // Criando as Tasks de conectividade, dos sensores e tratador de interrupção
    xTaskCreate(&conectaWifi, "Conecta ao wifi", 2048, NULL, 1, NULL);
    xTaskCreate(&conectaMQTT, "Conecta ao servidor MQTT", 2048, NULL, 1, NULL);
    xTaskCreate(&envio_dados, "Processa dados", 2048, NULL, 2, NULL);
    
    xTaskCreate(&leitura_sensor_temperatura, "Ler Sensor Temperatura", 2048, NULL, 1, NULL);
    xTaskCreate(&leitura_sensor_umidade, "Ler Sensor Umidade", 2048, NULL, 1, NULL);
    xTaskCreate(&leitura_sensor_presenca, "Ler Sensor Presença", 2048, NULL, 1, NULL);
    xTaskCreate(&leitura_sensor_fumaca, "Ler Sensor Fumaça", 2048, NULL, 1, NULL);
    xTaskCreate(&alerta_notificacao_receptor, "Notificação", 2048, NULL, 1, &handler_notifica_alerta);

    xTaskCreate(&ittr_botao_campainha, "Alerta de Visita", 2048, NULL, 1, NULL);

    // Iniciando o serviço de interrupção com flag 0
    gpio_install_isr_service(0);
    // Atribuindo handler ao botão
    gpio_isr_handler_add(BTN_PIN0, gpio_isr_handler, (void *) BTN_PIN0);
    
}
