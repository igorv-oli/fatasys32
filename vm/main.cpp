#include <SDL2/SDL.h>
#include <fstream>
#include <iostream>
#include "memory/memory.h"
#include "cpu/cpu.h"

int main(int argc, char* argv[]) {
    // Inicialização da SDL2
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    SDL_Window* window = SDL_CreateWindow("Fantasys32", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320 * 2, 240 * 2, 0); // Escala x2 de exemplo
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 320, 240);

    Memory memory;
    CPU cpu;

    // Carregar o arquivo binário do jogo (ex: jogo.bin) na memória a partir da posição 0
    std::ifstream file("jogo.bin", std::ios::binary);
    if (file.is_open()) {
        // Código para ler byte a byte e colocar na RAM da VM...
    }

    bool running = true;
    SDL_Event event;
    uint32_t last_time = SDL_GetTicks();

    while (running) {
        // Trata eventos do teclado real e armazena para uso na instrução GKEY
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            // Atualizar o estado das teclas internas da VM aqui...
        }

        // Executa exatamente 10.000 instruções por Frame
        try {
            for (int i = 0; i < 10000; i++) {
                cpu.step(memory);
            }
        } catch (const std::exception& e) {
            std::cerr << "Erro em tempo de execução na VM: " << e.what() << std::endl;
            running = false;
        }

        // Atualiza a tela com o conteúdo do Framebuffer
        // Você precisará converter o formato de cor da especificação para o formato da textura SDL
        uint8_t* fb_ptr = memory.getFramebufferPtr();
        SDL_UpdateTexture(texture, NULL, fb_ptr, 320 * 4); // Ajustar o pitch conforme o formato de cor
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        // Controla o Framerate para cravar em 60 FPS (~16.67 ms por frame)
        uint32_t current_time = SDL_GetTicks();
        uint32_t elapsed = current_time - last_time;
        if (elapsed < 16) {
            SDL_Delay(16 - elapsed);
        }
        last_time = SDL_GetTicks();
    }

    // Limpeza
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}