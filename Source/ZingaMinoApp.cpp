#pragma once
#include "ZingaMinoApp.h"


//-------------------------------------------------------------------------------------
ZingaMinoApp::ZingaMinoApp(void)
{
	mGameMgr = 0;
    rules = "ZinaMino is an extension of the classical domimo game, with 4 diffrent colors  in addition to a joker piece called Zina tile.\n" ;
	rules+= "The game can be played by 2 to 4 players (1 human and the others are machines). \n Click on the 'New Game' button to start a new game and select the number of players\n";
	rules+="1) Click on 'Select a piece' so each player draw one tile to decide the first one to play\n";
	rules+="2) For opening turn, only doubles or a Zinga can be played.\n";
	rules+="3) In the following turns, a piece can be played if it marches the colour and the end of one of the pieces\n";
	rules+="4) if you don't have a suitable piece to play, click on the 'dig' buttong\n";
	rules+="5) The games finishes when one of the players finish his pieces or when the game blocks\n";
}
//-------------------------------------------------------------------------------------
ZingaMinoApp::~ZingaMinoApp(void)
{
}



#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        ZingaMinoApp app;

        try {
            app.go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif