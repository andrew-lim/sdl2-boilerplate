/*
 SDL2 example that allows user to move an object using arrow keys.
 This is meant to be used as a convenient single-file starting point for
 more complex projects.
 
 Windows / MinGW / Code::Blocks
 Linker settings (in Code::Blocks under Project->Build Options...)
 -lmingw32 -lSDL2main -lSDL2
 
 OSX + XCode
 Download DMG file and copy the SDL2.framework to /Library/Frameworks
 https://www.libsdl.org/release/SDL2-2.0.10.dmg
 
 Author: Andrew Lim Chong Liang
 https://github.com/andrew-lim/sdl2-boilerplate
 */
#include <SDL2/SDL.h>
#include <cstdio>
#include <map>

enum {
    DISPLAY_WIDTH  = 480
    , DISPLAY_HEIGHT = 320
    , UPDATE_INTERVAL = 1000/60
    , HERO_SPEED = 2
};

class Sprite {
public:
    int x, y ;
    Sprite() :x(0), y(0) {}
} ;

class Game {
public:
    Game();
    ~Game();
    void start();
    void stop() ;
    void draw();
    void fillRect(SDL_Rect* rc, int r, int g, int b );
    void fpsChanged( int fps );
    void onQuit();
    void onKeyDown( SDL_Event* event );
    void onKeyUp( SDL_Event* event );
    void run();
    void update();
private:
    std::map<int,int> keys; // No SDLK_LAST. SDL2 migration guide suggests std::map
    int frameSkip ;
    int running ;
    SDL_Window* window;
    SDL_Renderer* renderer;
    Sprite hero ;
};

Game::Game()
:frameSkip(0), running(0), window(NULL), renderer(NULL) {
}

Game::~Game() {
    this->stop();
}

void Game::start() {
    int flags = SDL_WINDOW_SHOWN ;
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        return ;
    }
    if (SDL_CreateWindowAndRenderer(DISPLAY_WIDTH, DISPLAY_HEIGHT, flags, &window, &renderer)) {
        return;
    }
    this->running = 1 ;
    run();
}

void Game::draw() {
    SDL_Rect heroRect ;
    
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    
    // Render hero
    heroRect.x = hero.x ;
    heroRect.y = hero.y ;
    heroRect.w = 20 ;
    heroRect.h = 20 ;
    fillRect(&heroRect, 255, 0, 0 );
    
    SDL_RenderPresent(renderer);
}

void Game::stop() {
    if (NULL != renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
    if (NULL != window) {
        SDL_DestroyWindow(window);
        window = NULL;
    }
    SDL_Quit() ;
}

void Game::fillRect(SDL_Rect* rc, int r, int g, int b ) {
    SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, rc);
}

void Game::fpsChanged( int fps ) {
    char szFps[ 128 ] ;
    sprintf( szFps, "%s: %d FPS", "SDL2 Base C++ - Use Arrow Keys to Move", fps );
    SDL_SetWindowTitle(window, szFps);
}

void Game::onQuit() {
    running = 0 ;
}

void Game::run() {
    int past = SDL_GetTicks();
    int now = past, pastFps = past ;
    int fps = 0, framesSkipped = 0 ;
    SDL_Event event ;
    while ( running ) {
        int timeElapsed = 0 ;
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:    onQuit();            break;
                case SDL_KEYDOWN: onKeyDown( &event ); break ;
                case SDL_KEYUP:   onKeyUp( &event );   break ;
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEMOTION:
                    break ;
            }
        }
        // update/draw
        timeElapsed = (now=SDL_GetTicks()) - past ;
        if ( timeElapsed >= UPDATE_INTERVAL  ) {
            past = now ;
            update();
            if ( framesSkipped++ >= frameSkip ) {
                draw();
                ++fps ;
                framesSkipped = 0 ;
            }
        }
        // fps
        if ( now - pastFps >= 1000 ) {
            pastFps = now ;
            fpsChanged( fps );
            fps = 0 ;
        }
        // sleep?
        SDL_Delay( 1 );
    }
}

void Game::update() {
    if ( keys[SDLK_LEFT] ) {
        hero.x -= HERO_SPEED ;
    } else if ( keys[SDLK_RIGHT] ) {
        hero.x += HERO_SPEED ;
    } else if ( keys[SDLK_UP] ) {
        hero.y -= HERO_SPEED ;
    } else if ( keys[SDLK_DOWN] ) {
        hero.y += HERO_SPEED ;
    }
}

void Game::onKeyDown( SDL_Event* evt ) {
    keys[ evt->key.keysym.sym ] = 1 ;
}

void Game::onKeyUp( SDL_Event* evt ) {
    keys[ evt->key.keysym.sym ] = 0 ;
}

int main(int argc, char** argv){
    Game game;
    game.start();
    return 0;
}
