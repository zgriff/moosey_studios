//
//  main.cpp
//  Roshamboogie
//
//  Created by Zach Griffin on 3/6/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "App.h"

using namespace cugl;

/**
 * The main entry point of any CUGL application.
 *
 * This class creates the application and runs it until done.  You may
 * need to tailor this to your application, particularly the application
 * settings.  However, never modify anything below the line marked.
 *
 * @return the exit status of the application
 */
int main(int argc, char * argv[]) {
    // Change this to your application class
    App app;
    
    // Set the properties of your application
    app.setName("Mystic Mayhem");
    app.setOrganization("Moosey Studios");
    app.setHighDPI(true);
    app.setFPS(60.0f);

    // VARY THIS TO TRY OUT YOUR SCENE GRAPH
    app.setSize(1280, 720); // 16x9,  Android phones, PC Gaming

    /// DO NOT MODIFY ANYTHING BELOW THIS LINE
    if (!app.init()) {
        return 1;
    }
    
    app.onStartup();
    while (app.step());
    app.onShutdown();

    exit(0);    // Necessary to quit on mobile devices
    return 0;   // This line is never reached
}
