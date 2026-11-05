# Sistema de Controle de Reservatórios – Fase 3 (Arduino UNO)

Porta o sistema das Fases 1 e 2 (C++/Qt) para o Arduino UNO. **As máquinas de
estados não foram alteradas**: só foi trocada a Camada de Abstração de Hardware
(HAL) e o ponto de entrada do programa.

```
TDE_03/
├── ap/
│   ├── common/            # Lógica de controle (igual à Fase 2, NÃO alterado)
│   │   ├── sm1..sm4.cpp   #   Máquinas de estados (v1, b1, v2, r1)
│   │   ├── behavior.cpp   #   Inicializa e executa as 4 máquinas
│   │   ├── chronometer.cpp#   Temporização não-bloqueante (usa now())
│   │   └── hal.h          #   Interface da HAL (o "contrato")
│   ├── arduino/
│   │   └── hal.cpp        # >>> NOVO: HAL para o Arduino UNO <<<
│   └── win_x86_qt_gui/    # Simulador Qt da Fase 2 (mantido como referência)
├── lib/arduino_core/      # Core do Arduino (copiado pelo template)
└── template_arduino_se/   # Projeto do Microchip Studio
    ├── main.cpp           # >>> ALTERADO: setup()/loop() chamando o behavior <<<
    ├── template_arduino_se.cppproj  # >>> ALTERADO: inclui ap/common e ap/arduino <<<
    └── flash.bat          # Grava o .hex na placa
```

## 1. Mapeamento de pinos

| Componente | Função                  | Pino Arduino | Mesa digital |
|------------|-------------------------|:------------:|--------------|
| `s11`      | nível baixo tanque 1    | 2            | SW0          |
| `s12`      | nível alto tanque 1     | 3            | SW1          |
| `s21`      | nível baixo tanque 2    | 4            | SW2          |
| `s22`      | nível alto tanque 2     | 5            | SW3          |
| `s31`      | nível baixo tanque 3    | 6            | SW4          |
| `s32`      | nível alto tanque 3     | 7            | SW5          |
| `v1`       | válvula (enche t1)      | 8            | LED3         |
| `v2`       | válvula (t2 → t3)       | 9            | LED2         |
| `b1`       | bomba (t1 → t2)         | 10           | LED1         |
| `r1`       | resistência (aquece t3) | 11           | LED0         |
| `st1`      | temperatura tanque 3    | A0           | Potenciômetro (cursor) |
| erro `sm1` | falha sensores t1       | 12           | LED4         |
| erro `sm2` | falha sensores t2       | 13           | LED5         |
| erro `sm3` | falha sensores t3       | A1           | LED6         |
| erro `sm4` | temperatura inválida    | A2           | LED7         |

Os pinos de 2 a 11 e A0 são os exigidos pela especificação. Os LEDs de erro
(12, 13, A1, A2) foram escolhidos por nós, usando os LEDs livres da mesa.

## 2. Montagem

Faça a fiação com a **mesa desligada** e o **USB do Arduino desconectado**.

1. **GND comum**: ligue um pino `GND` do Arduino ao `GND` da mesa. Sem isso
   nada funciona de forma confiável.
2. **Chaves**: SW0..SW5 (bloco "8 Bits Data Switches") → pinos 2..7.
   Chave na posição `1` = 5 V = sensor ativo.
3. **LEDs** (bloco "Bits LED Displays"): pinos 8, 9, 10, 11 → LED3, LED2,
   LED1, LED0; pinos 12, 13, A1, A2 → LED4, LED5, LED6, LED7. Os LEDs da mesa
   acendem com nível alto.
4. **Potenciômetro** (bloco "Potentiometer"): um extremo no `5V` do Arduino, o
   outro no `GND`, e o cursor (terminal do meio) no `A0`. Assim `A0` lê de 0 V
   a 5 V.
5. Coloque **todas as chaves SW0..SW5 em 0**.
6. Ligue a mesa e conecte o **USB** do Arduino ao computador (alimenta a placa
   e serve para gravar e para o Serial Monitor).

## 3. Compilar e gravar

1. Abra `template_arduino_se.atsln` (na raiz do repositório) no **Microchip
   Studio**.
2. Na barra de ferramentas, confira que a configuração ativa é **Debug** (a
   única com os includes e defines do Arduino configurados; a Release não
   compila).
3. `Build → Build Solution` (**F7**). A janela *Output* deve terminar com
   `Build succeeded`, e é gerado o arquivo
   `template_arduino_se\Debug\template_arduino_se.hex`.
4. Descubra a porta COM do Arduino: *Gerenciador de Dispositivos → Portas
   (COM e LPT)* → "Arduino Uno (COMx)" ou "USB-SERIAL CH340 (COMx)".
5. Feche qualquer Serial Monitor aberto (a porta não pode estar em uso).
6. Abra a pasta `template_arduino_se` no Explorer e dê **dois cliques** em
   `flash.bat` (ele procura o `.hex` na pasta `Debug` ao lado dele). Digite o
   número da porta COM do Arduino e aguarde. Ao final deve aparecer
   `avrdude: ... bytes of flash verified` e `avrdude done. Thank you.`
7. Abra um Serial Monitor (Arduino IDE, PuTTY, Data Visualizer…) na porta do
   Arduino a **9600 bps**. A cada 0,5 s aparece uma linha como:

   ```
   st1=49.3C | s11=1 s12=0 s21=0 s22=0 s31=0 s32=0 | sm1_v1_aberta sm2_b1_ligada sm3_v2_fechada sm4_r1_desligada
   ```

   Abrir o Serial Monitor reinicia a placa. Isso é normal e não atrapalha.

> Para gravar de novo depois de alterar o código: F7 no Microchip Studio,
> fechar o Serial Monitor e rodar o `flash.bat` outra vez.

## 4. Roteiro de teste funcional

Comece com **todas as chaves em 0** e a placa recém-ligada (ou após RESET).

| Passo | Ação                                   | Resultado esperado                                  |
|------:|----------------------------------------|-----------------------------------------------------|
| 1     | Ligar a placa                          | Só LED3 (`v1`) aceso: t1 vazio → válvula enchendo   |
| 2     | SW0 (`s11`) → 1                        | Após 2 s, LED1 (`b1`) acende (há água para bombear) |
| 3     | SW1 (`s12`) → 1                        | LED3 (`v1`) apaga (t1 cheio)                        |
| 4     | SW2 (`s21`) → 1                        | LED2 (`v2`) acende (t2 tem água, t3 não está cheio) |
| 5     | SW3 (`s22`) → 1                        | LED1 (`b1`) apaga (t2 cheio)                        |
| 6     | SW4 (`s31`) → 1 e pot. girado p/ baixo | LED0 (`r1`) acende quando `st1 ≤ 49 °C`             |
| 7     | Girar o potenciômetro para cima        | LED0 (`r1`) apaga quando `st1 ≥ 51 °C`              |
| 8     | SW5 (`s32`) → 1                        | LED2 (`v2`) apaga (t3 cheio)                        |
| 9     | SW1 → 0 (t1 baixou)                    | LED3 (`v1`) volta a acender (ver nota dos tempos)   |
| 10    | SW3 → 0 (t2 baixou)                    | LED1 (`b1`) volta a acender (ver nota dos tempos)   |

**Nota dos tempos mínimos:** `v1` fica fechada **no mínimo 3 s** depois de
fechar e `b1` fica desligada **no mínimo 5 s** depois de desligar. O cronômetro
começa a contar quando a válvula fecha / bomba desliga (entrada no estado), não
quando a chave volta a 0. Para ver o tempo agindo, ligue e desligue SW1 (ou
SW3) rapidamente: o LED só reacende quando os 3 s (ou 5 s) se completarem. Se
você esperar mais que isso antes de voltar a chave, o LED reacende na hora.

**Proteções:**

- Com `b1` ligada e SW1 em 0, coloque SW0 → 0: LED1 apaga **imediatamente**
  (bomba não pode trabalhar sem água).
- Com `r1` ligada e **SW5 em 0**, coloque SW4 → 0: LED0 apaga **imediatamente**.
  (Se SW5 estiver em 1, desligar SW4 deixa o tanque 3 incoerente e a `sm3`
  entra em erro – LED6.)

**Estado de erro (falha segura):** com SW0 em 0, coloque SW1 em 1 (sensor de
nível alto ativo sem o de nível baixo – fisicamente impossível). LED4 acende,
`v1` desliga e o Serial mostra `sm1_err`. O mesmo vale para SW3 sem SW2 (LED5)
e SW5 sem SW4 (LED6). Para sair do erro, **pressione RESET na placa**.

> Dica: para não entrar em erro sem querer, sempre ligue a chave de nível
> baixo **antes** da de nível alto, e desligue a de nível alto **antes** da de
> nível baixo.

> `sm4_err` (LED7) só ocorre com temperatura fora de 0..100 °C. Como o
> potenciômetro gera 40..60 °C, esse erro nunca acontece no hardware — mas a
> lógica foi mantida porque as máquinas de estados não podem ser alteradas.

## 5. Problemas comuns

| Sintoma | Causa provável |
|---|---|
| `flash.bat` diz "Nenhum arquivo .hex encontrado" | Não compilou em **Debug**, ou rodou o `.bat` fora da pasta `template_arduino_se` |
| `avrdude: stk500_recv(): programmer is not responding` | Porta COM errada ou Serial Monitor aberto |
| LEDs não acendem / chaves não fazem efeito | Falta o **GND comum** entre Arduino e mesa, ou mesa desligada |
| Tudo apagado e um LED de erro (LED4–LED6) aceso | Chave de nível alto ligada sem a de nível baixo → corrigir as chaves e apertar **RESET** |
| `st1` fica fixo em 40.0 ou 60.0 | Cursor do potenciômetro não está no `A0`, ou faltou ligar 5V/GND nos extremos |
| Serial Monitor mostra caracteres estranhos | Velocidade diferente de **9600** |

## 6. Como o código funciona (para a defesa)

### Arquitetura em camadas

```
 main.cpp (Arduino)  →  behavior.cpp  →  sm1..sm4.cpp  →  hal.h  →  hal.cpp (Arduino)
   setup()/loop()        init/step        lógica           contrato    pinMode/digitalRead/...
```

As máquinas de estados só conhecem o `hal.h`. Na Fase 2 quem implementava o
`hal.h` era `win_x86_qt_gui/hal.cpp` (variáveis em memória e relógio do PC);
agora é `arduino/hal.cpp` (pinos físicos e `millis()`). Trocar um arquivo
trocou o hardware inteiro sem tocar na lógica de controle — esse é o ponto
central da atividade.

### `ap/arduino/hal.cpp`

- **`#define PIN_...`** no topo: tabela de pinos em um só lugar. Para mudar a
  fiação basta mudar um número.
- **`init_hal()`**: chama `pinMode()` para cada pino (entradas = chaves, saídas
  = LEDs) e desliga todas as saídas. É chamada por `behavior_init()`, que por
  sua vez é chamada no `setup()`.
- **`now()`**: devolve `millis()`. O `Chronometer` das máquinas calcula
  `now() + delta` e compara depois, sem travar o programa (sem `delay()`).
- **`set_v1()` etc.**: `digitalWrite(pino, state)`. O `true` vale 1 (`HIGH`,
  LED aceso) e o `false` vale 0 (`LOW`, LED apagado).
- **`get_s11()` etc.**: `return digitalRead(pino);`. Chave em `1` → 5 V →
  `HIGH` (1) → `true`.
- **`get_st1()`**: `analogRead(A0)` dá 0..1023 (0 V..5 V). A conversão é uma
  regra de três: `40 + (valor / 1023) * (60 - 40)` → 40,0..60,0 °C.
- **`get_btn_reset()`**: retorna sempre `false`. A especificação diz que a
  saída do estado de erro é reiniciar a placa; a função existe porque as
  máquinas de estados a chamam e não podem ser alteradas.

### `template_arduino_se/main.cpp`

- O template do professor não usa o `main()` do core Arduino, então criamos o
  nosso: `init()` (timers do Arduino, obrigatório para `millis()`), depois
  `setup()` uma vez e `loop()` para sempre.
- **`setup()`**: `Serial.begin(9600)` e `behavior_init()` (que chama
  `init_hal()` e inicializa as 4 máquinas).
- **`loop()`**: `behavior_step()` executa um passo de cada máquina. Em seguida
  um `Chronometer` decide se já passaram 500 ms para imprimir o status no
  Serial — de novo sem `delay()`, para que as máquinas rodem milhares de vezes
  por segundo e reajam imediatamente às chaves.

### Por que não usar `delay()`?

`delay()` para o processador. Se `sm1` precisasse esperar 3 s com `delay()`,
`sm2`, `sm3` e `sm4` ficariam congeladas e a proteção da bomba não funcionaria.
Com `millis()` cada máquina só "anota" quando deve agir e segue em frente.

## 7. Simulador Qt e testes da Fase 2

Continuam funcionando normalmente: o projeto Qt em `ap/win_x86_qt_gui` e os
testes em `ap/tests` usam a HAL simulada e não dependem do Arduino.
