; =====================================================================
; FLAPPY BIRD PARA FANTASYS32
; =====================================================================

.data
    ; Definições de Cores (Formato ARGB: 0xAARRGGBB)
    COR_FUNDO:    .word 0xFF4EC0E4  ; Azul Celeste
    COR_PASSARO:  .word 0xFFFCD814  ; Amarelo
    COR_CANO:     .word 0xFF73C734  ; Verde Canos
    COR_TEXTO:    .word 0xFFFFFFFF  ; Branco

    ; Variáveis do Jogo
    passaro_y:    .word 100         ; Posição Y inicial do pássaro (X é fixo em 50)
    passaro_v:    .word 0           ; Velocidade Y do pássaro
    gravidade:    .word 1           ; Força da gravidade por frame
    forca_pulo:   .word -6          ; Velocidade ao pular (impulso para cima)

    ; Estado dos Canos (X, Largura, Altura do Espaço, Y do Espaço)
    cano_x:       .word 320         ; Começa no canto direito da tela
    cano_larg:    .word 40          ; Largura do cano
    cano_gap:     .word 60          ; Tamanho da abertura para o pássaro passar
    cano_gap_y:   .word 90          ; Posição Y onde começa a abertura (muda aleatoriamente)

    ; Pontuação
    score:        .word 0
    txt_score:    .string "SCORE: "

.text
MAIN:
    ; Inicializa o gerador de números aleatórios com o número de frames
    FRAMENUM R1
    SRAND R1

LOOP_JOGO:
    ; 1. LIMPAR A TELA (Desenha o Céu Azul)
    LW R1, COR_FUNDO(R0)
    CLEAR R1

    ; 2. ATUALIZAR PÁSSARO (Física de Gravidade)
    LW R1, passaro_y(R0)
    LW R2, passaro_v(R0)
    LW R3, gravidade(R0)
    
    ADD R2, R2, R3          ; passaro_v += gravidade
    ADD R1, R1, R2          ; passaro_y += passaro_v
    SW R2, passaro_v(R0)
    SW R1, passaro_y(R0)

    ; 3. VERIFICAR ENTRADA (Teclado - Seta para Cima = ID 2)
    ; De acordo com o mapeamento do seu main.cpp: SDLK_UP = Teclado Virtual ID 2
    MOVL R2, 2
    GKEY R1, R2             ; R1 = 1 se pressionada, 0 se solta
    MOVL R3, 0
    BEQ R1, R3, NAO_PULOU   ; Se for 0, pula a lógica de impulso

    ; Pássaro Impulsionado para cima
    LW R4, forca_pulo(R0)
    SW R4, passaro_v(0)     ; passaro_v = forca_pulo

NAO_PULOU:
    ; 4. MOVIMENTAR OS CANOS
    LW R5, cano_x(R0)
    DEC R5                  ; cano_x-- (Move para a esquerda 1 pixel por frame)
    
    ; Se o cano saiu totalmente da tela esquerda (cano_x < -40)
    MOVL R6, -40
    BGT R5, R6, CANO_EM_TELA

    ; Resetar cano para a direita e gerar nova abertura aleatória
    MOVL R5, 320            ; cano_x = 320
    
    ; Randomizar a posição Y do GAP (Entre 30 e 130)
    MOVL R7, 30
    MOVL R8, 130
    RAND R9, R7, R8         ; R9 = novo cano_gap_y
    SW R9, cano_gap_y(R0)

    ; Incrementar Score
    LW R10, score(R0)
    INC R10
    SW R10, score(R0)

CANO_EM_TELA:
    SW R5, cano_x(R0)

    ; 5. RENDERIZAR CANOS (Superior e Inferior)
    ; Cano Superior: de X=cano_x, Y=0 até Larg=cano_larg, Alt=cano_gap_y
    LW R11, cano_larg(R0)
    LW R12, cano_gap_y(R0)
    LW R13, COR_CANO(R0)
    MOVL R1, 0              ; Y = 0
    RECT R5, R1, R11, R12, R13

    ; Cano Inferior: de X=cano_x, Y=(cano_gap_y + cano_gap) até o chão
    LW R14, cano_gap(R0)
    ADD R1, R12, R14        ; Y_inferior = cano_gap_y + cano_gap
    MOVL R2, 240
    SUB R2, R2, R1          ; Altura restante = 240 - Y_inferior
    RECT R5, R1, R11, R2, R13

    ; 6. RENDERIZAR PÁSSARO (Representado por um retângulo amarelo 12x12)
    MOVL R1, 50             ; X fixo em 50
    LW R2, passaro_y(R0)
    MOVL R3, 12             ; Largura
    MOVL R4, 12             ; Altura
    LW R5, COR_PASSARO(R0)
    RECT R1, R2, R3, R4, R5

    ; 7. EXIBIR PLACAR (Interface)
    MOVL R1, 10             ; Texto X
    MOVL R2, 10             ; Texto Y
    MOVL R3, txt_score      ; Endereço da string
    LW R4, COR_TEXTO(R0)
    PSTR R1, R2, R3, R4     ; Plota "SCORE: "

    MOVL R1, 65             ; Inteiro X (ao lado do texto)
    LW R3, score(R0)
    PINT R1, R2, R3, R4     ; Plota o número do score

    ; 8. DETECÇÃO DE COLISÕES Básica (Chão e Teto)
    LW R2, passaro_y(R0)
    MOVL R6, 0
    BLT R2, R6, GAME_OVER   ; Bateu no teto?
    MOVL R6, 228            ; 240 - tamanho do pássaro
    BGT R2, R6, GAME_OVER   ; Bateu no chão?

    ; Nota: Para colisão avançada com os canos, você compararia se
    ; o X do pássaro (50) está entre cano_x e cano_x + cano_larg,
    ; e se o Y está fora do intervalo (cano_gap_y e cano_gap_y + cano_gap).

    ; Retorna para o próximo ciclo de frames gerenciado pela sua VM
    JMP LOOP_JOGO

GAME_OVER:
    ; Som de derrota (Freq=150Hz, Tempo=500ms, Forma de onda quadrada=1)
    MOVL R1, 150
    MOVL R2, 500
    MOVL R3, 1
    PLAY R1, R2, R3

    ; Trava a execução indicando fim de jogo (ou poderia reiniciar)
    HALT