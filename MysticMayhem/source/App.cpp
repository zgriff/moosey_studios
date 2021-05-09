//
//  App.cpp
//  Roshamboogie
//
//  Created by Zach Griffin on 3/6/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "App.h"
#include "NetworkController.h"
#include "MapConstants.h"
#include "SoundController.h"
#include <cstdlib>

using namespace cugl;


#pragma mark -
#pragma mark Gameplay Control


void App::onStartup() {
    srand((unsigned) time(0));
    _assets = AssetManager::alloc();
    _batch  = SpriteBatch::alloc();
    auto cam = OrthographicCamera::alloc(getDisplaySize());

    // Start-up basic input
#ifdef CU_MOBILE
    Input::activate<Touchscreen>();
    Input::activate<Accelerometer>();
    Input::activate<Keyboard>();
#else
    Input::activate<Mouse>();
    Input::get<Mouse>()->setPointerAwareness(Mouse::PointerAwareness::DRAG);
    Input::activate<Keyboard>();
#endif

    _assets->attach<Font>(FontLoader::alloc()->getHook());
    _assets->attach<Texture>(TextureLoader::alloc()->getHook());
    _assets->attach<Sound>(SoundLoader::alloc()->getHook());
    _assets->attach<scene2::SceneNode>(Scene2Loader::alloc()->getHook());
    _assets->attach<World>(GenericLoader<World>::alloc()->getHook());

    // Create a "loading" screen
    _currentScene = SceneSelect::Loading;
    _loading.init(_assets);

    // Queue up the other assets
    _assets->loadDirectoryAsync("json/assets.json",nullptr);
    _assets->loadAsync<World>(GRASS_MAP_KEY,GRASS_MAP_JSON,nullptr);
    _assets->loadAsync<World>(GRASS_MAP2_KEY, GRASS_MAP2_JSON, nullptr);
    _assets->loadAsync<World>(GRASS_MAP3_KEY, GRASS_MAP3_JSON, nullptr);
    _assets->loadAsync<World>(GRASS_MAP4_KEY, GRASS_MAP4_JSON, nullptr);

    AudioEngine::start();
    SoundController::init(_assets);

    Application::onStartup(); // YOU MUST END with call to parent
}


void App::onShutdown() {
    _loading.dispose();
    _menu.dispose();
    _gameplay.dispose();
    _results.dispose();
    _lobby.dispose();
    _assets = nullptr;
    _batch = nullptr;

    // Shutdown input
#ifdef CU_MOBILE
    Input::deactivate<Touchscreen>();
    Input::deactivate<Accelerometer>();
    Input::deactivate<Keyboard>();
#else
    Input::deactivate<Mouse>();
    Input::deactivate<Keyboard>();
#endif

    AudioEngine::stop();
    Application::onShutdown();
}


/**
 * The method called when the application is suspended and put in the background.
 *
 * When this method is called, you should store any state that you do not
 * want to be lost.  There is no guarantee that an application will return
 * from the background; it may be terminated instead.
 *
 * If you are using audio, it is critical that you pause it on suspension.
 * Otherwise, the audio thread may persist while the application is in
 * the background.
 */
void App::onSuspend() {
    AudioEngine::get()->pause();
}

/**
 * The method called when the application resumes and put in the foreground.
 *
 * If you saved any state before going into the background, now is the time
 * to restore it. This guarantees that the application looks the same as
 * when it was suspended.
 *
 * If you are using audio, you should use this method to resume any audio
 * paused before app suspension.
 */
void App::onResume() {
    AudioEngine::get()->resume();
}

/**
 * The method called to update the application data.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should contain any code that is not an OpenGL call.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
 void App::update(float timestep) {
     switch (_currentScene) {
         case SceneSelect::Loading:{
             if (_loading.isActive()) {
                 _loading.update(0.01f);
             } else {
                 _loading.dispose(); // Disables the input listeners in this mode
                 _menu.init(_assets);
                 _currentScene = SceneSelect::Menu;
                 _menu.setActive(true);
             }
             break;
         }
         case SceneSelect::Menu:{
             _menu.update();
             if (_menu.isActive()) {
 //                _menu.update(0.01f);
                 NetworkController::step();
 //                CULog("menu scene");
                 if((_menu.createPressed() || _menu.joinPressed()) && NetworkController::getStatus() == cugl::CUNetworkConnection::NetStatus::Connected){
                     _menu.setActive(false);
                     _menu.getSettings()->removeAllChildren();
                     _menu.getSettings()->dispose();
                     _menu.removeAllChildren();
                     _menu.dispose();
                     _lobby.init(_assets);
                     _lobby.setActive(true);
                     //NetworkController::setLobbyScene(_lobby);
                     _currentScene = SceneSelect::Lobby;
                 }
             }
             else {
                 _menu.setActive(false);
                 _menu.removeAllChildren();
                 _menu.dispose();
                 _lobby.init(_assets);
                 _lobby.setActive(true);
                 _currentScene = SceneSelect::Lobby;
             }
             break;
         }
         case SceneSelect::Lobby:{
             if (_lobby.isActive()) {
                 _lobby.update(0.01f);
             } else {
                 _lobby.setActive(false);
                 _lobby.getSettings()->removeAllChildren();
                 _lobby.getSettings()->dispose();
                 _lobby.removeAllChildren();
                 _lobby.dispose();
                 _gameplay.init(_assets);
                 _gameplay.setActive(true);
                 _gameplay.setMovementStyle(0);
                 startTimer = time(NULL);
                 _currentScene = SceneSelect::Game;
             }
             break;
         }
         case SceneSelect::Game:{
             _gameplay.update(timestep);
             if (time(NULL) - startTimer >= gameTimer) {
                 _results.init(_assets, _gameplay.getResults(), _gameplay.getWinner());
                 _gameplay.getSettings()->removeAllChildren();
                 _gameplay.getSettings()->dispose();
                 _gameplay.dispose();
 //                _gameplay.reset();
                 _currentScene = SceneSelect::Results;
                 
             }
             else {
                 if (_gameplay.getSettings()->leaveGamePressed()) {
                     CULog("leave game in app");
                     NetworkController::destroyConn();
                     _gameplay.getSettings()->removeAllChildren();
                     _gameplay.getSettings()->dispose();
                     _gameplay.dispose();
                     _menu.init(_assets);
                     _currentScene = SceneSelect::Menu;
                     _menu.setActive(true);
                 }
             }
             break;
         }
         case SceneSelect::Results: {
             if (_results.playAgain()) {
                 _results.dispose();
                 _lobby.init(_assets);
                 _lobby.setActive(true);
                 _lobby.setPlayAgain(true);
                 _currentScene = SceneSelect::Lobby;
             }
             else if (_results.mainMenu()) {
                 NetworkController::destroyConn();
                 _results.dispose();
                 _menu.init(_assets);
                 _currentScene = SceneSelect::Menu;
                 _menu.setActive(true);

             }
             break;
         }
         default:
             break;
     }
 }

/**
 * The method called to draw the application to the screen.
 *
 * This is your core loop and should be replaced with your custom implementation.
 * This method should OpenGL and related drawing calls.
 *
 * When overriding this method, you do not need to call the parent method
 * at all. The default implmentation does nothing.
 */
void App::draw() {
    switch (_currentScene) {
        case SceneSelect::Loading:
            _loading.render(_batch);
            break;
        case SceneSelect::Menu:
            _menu.render(_batch);
            break;
        case SceneSelect::Results:
            _results.render(_batch);
            break;
        case SceneSelect::Lobby:
            _lobby.render(_batch);
            break;
        default:
            _gameplay.render(_batch);
            break;
    }

}
