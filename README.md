# projeto-str-automacao-residencial
Nesse projeto de automação residencial foi implementado o sensoriamento (simulado) de temperatura, umidade, fumaça, presença na casa, e campanhia do portão.

O código é foi construído baseado no Sistema Operacional em Tempo Real FreeRTOS no microcontrolador ESP32.
* Foram implementada a seguinte estrutura:
* * 4 Tasks períodicas para cada sensor mencionado (temperatura, umidade, fumaça, presença).
  * 2 Task Esporádica, sen do 1 para interrupção da campainha e outro refernte a acontecimentos dos níveis de fumaça e temperatura.

 
