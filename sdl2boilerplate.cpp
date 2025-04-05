/*
 SDL2 example that allows user to move an object using arrow keys.
 This is meant to be used as a convenient single-file starting point for
 more complex projects.

 Latest known stable release:
 https://github.com/libsdl-org/SDL/releases/tag/release-2.32.2

 Windows / MinGW / Code::Blocks
 Linker settings (in Code::Blocks under Project->Build Options...)
 -lmingw32 -lSDL2main -lSDL2

 Use the files inside i686-w64-mingw32 to compile for 32-bit.
 Use the files inside x86_64-w64-mingw32 to compile for 64-bit.
 
 OSX + XCode
 Download DMG file and copy the SDL2.framework to /Library/Frameworks
 https://www.libsdl.org/release/SDL2-2.0.10.dmg
 
 Author: Andrew Lim Chong Liang
 https://github.com/andrew-lim/sdl2-boilerplate
*/

// Assumes your compiler include path is set to search inside one of SDL2's
// include folders. E.g. ..\SDL2-2.32.2\i686-w64-mingw32\include
#include <SDL2/SDL.h>
#include <cstdio>
#include <map>

enum {
    DISPLAY_WIDTH  = 480,
    DISPLAY_HEIGHT = 320,
    UPDATE_INTERVAL = 1000/60,
    HERO_SPEED = 2
};

typedef struct {
    int x = 0;
    int y = 0;
} Sprite;

class Game {
public:
    Game();
    ~Game();
    void start();
    void draw();
    void fillRect(SDL_Rect* rc, int r, int g, int b);
    void fpsChanged(int fps);
    void onQuit();
    void onKeyDown(const SDL_Event& event);
    void onKeyUp(const SDL_Event& event);
    void run();
    void update();
private:
    std::map<int,int> m_keys; // No SDLK_LAST. SDL2 migration guide suggests std::map
    int m_frameSkip;
    bool m_running;
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    Sprite m_hero;
};

Game::Game() {
    m_frameSkip = 0;
    m_running = false;
}

Game::~Game() {
    if (nullptr != m_renderer) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }
    if (nullptr != m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
    SDL_Quit();
}

void Game::start() {
    int flags = SDL_WINDOW_SHOWN;
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        return;
    }
    if (SDL_CreateWindowAndRenderer(DISPLAY_WIDTH, DISPLAY_HEIGHT, flags, &m_window, &m_renderer)) {
        return;
    }
    m_running = true;
    run();
}

void Game::draw() {
    SDL_Rect heroRect;
    
    // Clear screen
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(m_renderer);
    
    // Render hero
    heroRect.x = m_hero.x;
    heroRect.y = m_hero.y;
    heroRect.w = 20;
    heroRect.h = 20;
    fillRect(&heroRect, 255, 0, 0);
    
    SDL_RenderPresent(m_renderer);
}



void Game::fillRect(SDL_Rect* rc, int r, int g, int b) {
    SDL_SetRenderDrawColor(m_renderer, r, g, b, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(m_renderer, rc);
}

void Game::fpsChanged(int fps) {
    char szFps[64];
    sprintf(szFps, "SDL2 Base C++ - Use Arrow Keys to Move: %d FPS", fps);
    SDL_SetWindowTitle(m_window, szFps);
}

void Game::run() {
    int past = SDL_GetTicks();
    int now = past, pastFps = past;
    int fps = 0, framesSkipped = 0;
    SDL_Event event;
    while (m_running) {
        int timeElapsed = 0;
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    m_running = false;
                    break;
                case SDL_KEYDOWN:
                    onKeyDown(event);
                    break;
                case SDL_KEYUP:
                    onKeyUp(event);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEMOTION:
                    break;
            }
        }
        // update/draw
        timeElapsed = (now=SDL_GetTicks()) - past;
        if (timeElapsed >= UPDATE_INTERVAL ) {
            past = now;
            update();
            if (framesSkipped++ >= m_frameSkip) {
                draw();
                ++fps;
                framesSkipped = 0;
            }
        }
        // fps
        if (now - pastFps >= 1000) {
            pastFps = now;
            fpsChanged(fps);
            fps = 0;
        }
        // sleep?
        SDL_Delay(1);
    }
}

void Game::update() {
    if (m_keys[SDLK_LEFT]) {
        m_hero.x -= HERO_SPEED;
    } else if (m_keys[SDLK_RIGHT]) {
        m_hero.x += HERO_SPEED;
    } else if (m_keys[SDLK_UP]) {
        m_hero.y -= HERO_SPEED;
    } else if (m_keys[SDLK_DOWN]) {
        m_hero.y += HERO_SPEED;
    }
}

void Game::onKeyDown(const SDL_Event& event) {
    m_keys[event.key.keysym.sym] = 1;
}

void Game::onKeyUp(const SDL_Event& event) {
    m_keys[event.key.keysym.sym] = 0;
}

int main(){
    Game game;
    game.start();
    return 0;
}
