# projeto-str-automacao-residencial
Nesse projeto de automação residencial foi implementado o sensoriamento (simulado) de temperatura, umidade, fumaça, presença na casa, e campanhia do portão.

O código é foi construído baseado no Sistema Operacional em Tempo Real FreeRTOS no microcontrolador ESP32.
* Foram implementada a seguinte estrutura:
* * 4 Tasks períodicas para cada sensor mencionado (temperatura, umidade, fumaça, presença).
  * 2 Task Esporádica, sendo 1 para interrupção da campainha e outro refernte a acontecimentos dos níveis de fumaça e temperatura.

Três Tasks extras foram inseridas simulando a conexão com Wifi, Servidor MQTT e Envio de Dados.

Para cada Task implementada a mensuração de tempo para execução de cada Task, sendo cada um delas atribuido prioridade igual.

Com relação a aplicação dos conceitos de Sistema em Tempo Real foram utilizados as seguintes funcionalidades fornecidas pelo FreeRTOS:
* Semaphore Binary
* Filas
* EventGroups
* Notifications

