# RF Spectrum Scanner & Jammer

Este proyecto utiliza Arduino y módulos **nRF24L01** para escanear el espectro de RF de 2.4 GHz o generar interferencias ("jamming") en los canales seleccionados. También incluye una pantalla OLED para mostrar el nivel de actividad en cada canal escaneado.

## Características

- Escaneo del espectro de RF en la banda de 2.4 GHz.
- Generación de interferencia en canales específicos.
- Control a través de botones para cambiar de canal y alternar entre los modos de escaneo y jamming.
- Visualización en pantalla OLED de la actividad de cada canal.

## Componentes Necesarios

- **Arduino** (Uno, Mega, o compatible).
- **Módulos nRF24L01** (2 unidades).
- **Pantalla OLED** (128x32, protocolo I2C).
- **Botones** (2 unidades).
- **Conexión Serial** (opcional, para monitorizar actividad en la computadora).

## Conexiones del Hardware

| Componente        | Pin de Arduino | Pin del Componente |
|-------------------|----------------|---------------------|
| **nRF24L01 (1)**  | CE (Pin 7)     | CE (Pin nRF24L01)  |
|                   | CSN (Pin 8)    | CSN (Pin nRF24L01) |
| **nRF24L01 (2)**  | CE (Pin 9)     | CE (Pin nRF24L01)  |
|                   | CSN (Pin 10)   | CSN (Pin nRF24L01) |
| **Pantalla OLED** | SDA (A4)       | SDA                |
|                   | SCL (A5)       | SCL                |
| **Botón 1 (BT1)** | Pin 2          | A Tierra (GND) al pulsar |
| **Botón 2 (BT2)** | Pin 3          | A Tierra (GND) al pulsar |

> **Nota:** Ambos módulos nRF24L01 comparten las conexiones de alimentación y datos SPI (SCK, MOSI, MISO).

## Instalación de Librerías

Las siguientes librerías deben estar instaladas en el entorno de Arduino:

1. **SPI**
2. **Wire**
3. **Adafruit_GFX**
4. **Adafruit_SSD1306**
5. **RF24**
6. **nRF24L01**

Instálalas desde el Administrador de Librerías de Arduino.

## Código

El código se encuentra en el archivo `jammer.ino`. Carga este código en el Arduino después de hacer todas las conexiones.

## Uso de los Botones

- **Botón 1 (BT1)**: Cambia el grupo de canales activos.
  - Cada vez que se presiona, selecciona un grupo de 5 canales consecutivos tanto para el modo de jamming como para el de escaneo.
  - El índice del grupo de canales (variable `channels` en el código, que cicla de 0 a 13) se reinicia al alcanzar el valor máximo.

- **Botón 2 (BT2)**: Alterna entre los modos de "jamming" y "escaneo".
  - **Modo Jamming** (`jamming = true`): Transmite un patrón de interferencia en el grupo de 5 canales seleccionado por BT1.
  - **Modo Escaneo** (`jamming = false`): Realiza un escaneo del espectro en el grupo de 5 canales seleccionado por BT1. Los niveles de actividad de estos 5 canales se muestran en la pantalla OLED.

## Visualización de Resultados

1. **Monitor Serial**: Actualmente no se utiliza para mostrar los niveles de actividad de cada canal escaneado; esta información se visualiza en la pantalla OLED. El puerto serial está inicializado y puede usarse para otros fines de depuración si se modifica el código.
2. **Pantalla OLED**: Visualiza la intensidad de señal para cada uno de los 5 canales dentro del grupo actualmente seleccionado por BT1. Muestra el índice del grupo, el canal con la señal más fuerte dentro de ese grupo y su nivel de actividad.

## Propósito Educativo y Advertencia

Este proyecto está diseñado **exclusivamente con fines educativos y experimentales**. La creación de interferencias en el espectro de RF (jamming) puede afectar dispositivos de comunicación inalámbrica y está regulada en muchas jurisdicciones. 

### Advertencia
**No uses este proyecto en entornos donde pueda interferir con otros dispositivos críticos o de comunicaciones.** Asegúrate de cumplir con las normativas locales y obtener los permisos necesarios antes de realizar cualquier tipo de transmisión de interferencias. 

El autor y los colaboradores de este proyecto **no se hacen responsables** de cualquier mal uso de este software o hardware. 

> **Recordatorio:** Este proyecto está destinado a fines educativos para entender el espectro RF y su funcionamiento. Usa este conocimiento de manera responsable.


## Notas Adicionales

- Este proyecto es para uso experimental y educativo.
- Asegúrate de cumplir con las normativas locales sobre el uso de jamming de RF, ya que puede interferir en otros dispositivos de comunicación inalámbrica.
