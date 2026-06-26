#include <SDL2/SDL.h>
#include <fstream>
#include <iostream>
#include "memory/memory.h"
#include "cpu/cpu.h"

#define JANELA_LARG 320
#define JANELA_ALT 240

#define FPS_ALVO 60

#define ATRASO_FRAME (1000 / FPS_ALVO)

uint32_t frameBuffer[JANELA_LARG * JANELA_ALT];

int main(int argc, char* argv[]) {
    Memory memory;
    CPU cpu;

    // Coloca binário do jogo na memória
    std::ifstream file("jogo.bin", std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo binario!" << std::endl;
    }

    uint32_t text_start_address = 0;
    file.read(reinterpret_cast<char*>(&text_start_address), sizeof(uint32_t));

    std::cout << "O codigo (.text) comeca no endereco: 0x" 
            << std::hex << text_start_address << std::dec << std::endl;

    uint32_t data_size = text_start_address - 4; 
    uint32_t address = 0;
    uint32_t buffer32 = 0;

    for (uint32_t i = 0; i < data_size; i += 4) {
        if (file.read(reinterpret_cast<char*>(&buffer32), sizeof(uint32_t))) {
            memory.write32(address, buffer32);
            address += 4;
        }
    }

    while (file.read(reinterpret_cast<char*>(&buffer32), sizeof(uint32_t))) {
        memory.write32(address, buffer32);
        address += 4;
    }

    cpu.setRegister(15, text_start_address);
    
    // Inicia SDL e faz as configurações de tela, renderização e textura
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* janela = SDL_CreateWindow("Fantasys32", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        JANELA_LARG, 
        JANELA_ALT, 
        SDL_WINDOW_SHOWN);

    SDL_Renderer* renderizador = SDL_CreateRenderer(janela, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture* textura = SDL_CreateTexture(renderizador, 
        SDL_PIXELFORMAT_ARGB8888, 
        SDL_TEXTUREACCESS_STREAMING, 
        JANELA_LARG,
        JANELA_ALT);

    SDL_Event e;
    int quit = 0;

    // Loop principal da VM
    while (!quit) {
        uint32_t inicio_frame = SDL_GetTicks();

        // Trata eventos do teclado real e armazena para uso na instrução GKEY
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = 1;

            // Detecta se uma tecla foi pressionada (true) ou solta (false)
            if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
                bool esta_pressionada = (e.type == SDL_KEYDOWN);

                // Mapa de teclas
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:   cpu.setTecladoVirtual(0, esta_pressionada); break; // Seta para Esquerda = Tecla 0
                    case SDLK_RIGHT:  cpu.setTecladoVirtual(1, esta_pressionada); break; // Seta para Direita = Tecla 1
                    case SDLK_UP:     cpu.setTecladoVirtual(2, esta_pressionada); break; // Seta para Cima = Tecla 2
                    case SDLK_DOWN:   cpu.setTecladoVirtual(3, esta_pressionada); break; // Seta para Baixo = Tecla 3
                    case SDLK_ESCAPE: cpu.setTecladoVirtual(4, esta_pressionada); break; // Esc = Tecla 4
                    case SDLK_RETURN: cpu.setTecladoVirtual(5, esta_pressionada); break; // Enter = Tecla 5
                    case SDLK_n:      cpu.setTecladoVirtual(6, esta_pressionada); break; // Tecla N = Tecla 6
                    case SDLK_m:      cpu.setTecladoVirtual(7, esta_pressionada); break; // Tecla M = Tecla 7
                    case SDLK_a:      cpu.setTecladoVirtual(8, esta_pressionada); break; // Tecla A = Tecla 8
                    case SDLK_s:      cpu.setTecladoVirtual(9, esta_pressionada); break; // Tecla S = Tecla 9
                    case SDLK_d:      cpu.setTecladoVirtual(10, esta_pressionada); break; // Tecla D = Tecla 10
                    case SDLK_w:      cpu.setTecladoVirtual(11, esta_pressionada); break; // Tecla W = Tecla 11
                    case SDLK_q:      cpu.setTecladoVirtual(12, esta_pressionada); break; // Tecla Q = Tecla 12
                    case SDLK_e:      cpu.setTecladoVirtual(13, esta_pressionada); break; // Tecla E = Tecla 13
                    case SDLK_c:      cpu.setTecladoVirtual(14, esta_pressionada); break; // Tecla C = Tecla 14
                    case SDLK_v:      cpu.setTecladoVirtual(15, esta_pressionada); break; // Tecla V = Tecla 15

                    default: break; 
                }
            }
        }

        // Executa exatamente 10.000 instruções por Frame
        try {
            for (int i = 0; i < 10000; i++) {
                cpu.step(memory);
            }
        } catch (const std::exception& e) {
            std::cerr << "Erro em tempo de execução na VM: " << e.what() << std::endl;
            quit = 1;
        }

        // Atualiza a tela com o conteúdo do Framebuffer
        uint8_t* fb_ptr = memory.getFramebufferPtr();
        SDL_UpdateTexture(textura, NULL, fb_ptr, JANELA_LARG * sizeof(uint32_t)); // Ajustar o pitch conforme o formato de cor
        SDL_RenderClear(renderizador);
        SDL_RenderCopy(renderizador, textura, NULL, NULL);
        SDL_RenderPresent(renderizador);

        // Controla o Framerate para cravar em 60 FPS (~16.67 ms por frame)
        uint32_t hora_atual = SDL_GetTicks();
        uint32_t duracao_frame = hora_atual - inicio_frame;
        if (duracao_frame < ATRASO_FRAME) {
            SDL_Delay(ATRASO_FRAME - duracao_frame);
        }
    }

    // Limpeza e destruição da tela
    SDL_DestroyTexture(textura);
    SDL_DestroyRenderer(renderizador);
    SDL_DestroyWindow(janela);
    SDL_Quit();

    return 0;
}