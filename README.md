# jammer_bluetooth
Bloqueador RF
1. Conexiones del OLED (SSD1306):
VCC del OLED → 5V del Arduino
GND del OLED → GND del Arduino
SDA del OLED → A4 del Arduino (SDA para I2C)
SCL del OLED → A5 del Arduino (SCL para I2C)
2. Conexiones de los módulos nRF24L01:
Los módulos nRF24L01 requieren una fuente de 3.3V y consumen bastante corriente, así que es recomendable usar un capacitor de 10 µF entre VCC y GND para estabilizar la alimentación.

Primer Transmisor (radio1)
CE1 (Pin CE del nRF) → Pin 7 del Arduino
CSN1 (Pin CSN del nRF) → Pin 8 del Arduino
MISO del nRF → Pin 12 del Arduino
MOSI del nRF → Pin 11 del Arduino
SCK del nRF → Pin 13 del Arduino
VCC del nRF → 3.3V del Arduino
GND del nRF → GND del Arduino
Segundo Transmisor (radio2)
CE2 (Pin CE del nRF) → Pin 9 del Arduino
CSN2 (Pin CSN del nRF) → Pin 10 del Arduino
MISO del nRF → Pin 12 del Arduino
MOSI del nRF → Pin 11 del Arduino
SCK del nRF → Pin 13 del Arduino
VCC del nRF → 3.3V del Arduino
GND del nRF → GND del Arduino
Nota: Los pines MISO, MOSI, y SCK son compartidos entre los dos transmisores porque ambos están conectados al mismo bus SPI. Solo los pines CE y CSN deben ser individuales para cada módulo.

3. Conexiones de los botones (BT1 y BT2):
BT1 (Botón 1) → Pin 2 del Arduino (configurado como interruptor en el código)
BT2 (Botón 2) → Pin 3 del Arduino (configurado como interruptor en el código)
Ambos botones deben conectarse a GND cuando son presionados. Para ello, usa la resistencia de pull-up interna configurada en el código (INPUT_PULLUP), que evita la necesidad de resistencias externas.

Esquema de conexión resumido:
Componente	Pin del Arduino	Pin del Componente
OLED SSD1306	5V, GND, A4, A5	VCC, GND, SDA, SCL
NRF24L01 (CE1)	7	CE
NRF24L01 (CSN1)	8	CSN
NRF24L01 (CE2)	9	CE
NRF24L01 (CSN2)	10	CSN
NRF24L01 (MISO)	12	MISO
NRF24L01 (MOSI)	11	MOSI
NRF24L01 (SCK)	13	SCK
Botón 1 (BT1)	2	-
Botón 2 (BT2)	3	-
Este esquema te permitirá tener los dos módulos nRF24L01 y el OLED trabajando en conjunto con el Arduino.

#botones
En este código, los interruptores (botones BT1 y BT2) tienen estas funciones:

Botón 1 (BT1 en Pin 2):

Función: Cambiar el canal de transmisión de los módulos nRF24L01.
Detalles: Cada vez que se presiona, incrementa la variable channels que selecciona el grupo de canales (5 canales consecutivos) que se utilizarán para el "jamming" o para el escaneo. Cuando channels alcanza el valor máximo (13 en este caso), vuelve a 0, reiniciando el ciclo de canales.
Botón 2 (BT2 en Pin 3):

Función: Activar o desactivar el modo de "jamming" (interferencia) de los módulos.
Detalles: Este botón invierte el valor de la variable jamming, lo que alterna entre dos modos:
Modo Jamming (jamming = true): Ambos módulos nRF24L01 transmiten un patrón de interferencia en un grupo de canales especificado.
Modo Escaneo (jamming = false): Ambos módulos cambian a modo de escaneo, midiendo la actividad en los canales seleccionados. Esto permite visualizar el uso del espectro de RF (radiofrecuencia) en esos canales y mostrarlo por la interfaz Serial y la pantalla OLED.
Ambos botones están configurados para detectar cambios usando interrupciones, lo que permite que el Arduino responda inmediatamente cuando se presionan, sin importar qué esté haciendo en ese momento en el bucle principal (loop).
