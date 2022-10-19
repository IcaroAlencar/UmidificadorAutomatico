# Umidificador Automático
Sistema que controla o acionamento do umidificador com base na umidade.

Projeto desenvolvido na disciplina de microcontroladores, com o objetivo de controlar o acionamento de umidificadores,
foram utilizados para esse projeto:
1 PIC16F877A,
1 LCD 1602 para mostrar a temperatura e umidade,
1 Sensor de temperatura e umidade (DHT11),
1 modulo relé 5v,
1 modulo de fonte 5v para alimentar o sistema,
1 Crystal 20Mhz,
leds para mostrar o funcionamento do sistema e leitura da temperatura,
capacitores e resistores.

O sistema faz a leitura da temperatura e umidade, mostra as informações no LCD, no caso em que a umidade esteja abaixo de 70%, o sistema 
aciona o relé ligando o umidificador, no caso da umidade estar acima de 80%, o sitema desarma o relé desligando o umidificador.
