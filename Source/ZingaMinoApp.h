#pragma once
#include "BaseApplication.h"
#include "GameManager.h"

#ifndef __ZingaMinoApp_h_
#define __ZingaMinoApp_h_

class ZingaMinoApp : public BaseApplication
{

public:
	ZingaMinoApp(void);
	~ZingaMinoApp(void);	


protected:

	bool mShowNewGameMenu;
	bool mShowGameRule;	
	GameManager* mGameMgr;
	std::string rules;

	virtual void createCamera(void){
		mCamera = mSceneMgr->createCamera("PlayerCam");		
		mCamera->setPosition(Ogre::Vector3(0,3000,0));		
		mCamera->setNearClipDistance(200);
		mCamera->setLodBias(1.0f);		
		mCameraMan = new OgreBites::SdkCameraMan(mCamera);
		mCameraMan->setStyle(OgreBites::CameraStyle::CS_MANUAL);
		
	}

	//Methode to create our scene
	// ToDo: Convert it into Ogrebits randering system as our UI is not complex
	virtual void createScene(void)
	{
		try{
			// Set ambient light
			mSceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));
		}
		catch(Ogre::Exception ex){
			Ogre::LogManager::getSingletonPtr()->logMessage(Ogre::LogMessageLevel::LML_CRITICAL, "Error when creating the scene");
			Ogre::LogManager::getSingletonPtr()->logMessage(Ogre::LogMessageLevel::LML_CRITICAL, ex.getFullDescription());
		}
		
	}

	void createViewports(void)
	{
		Ogre::Viewport* vp = mWindow->addViewport(mCamera);
		vp->setBackgroundColour(Ogre::ColourValue(0.7f,0.7f,0.7f));

		// Alter the camera aspect ratio to match the viewport
		mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));	
	}

	void createFrameListener(void){

		try{
			Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
			OIS::ParamList pl;
			size_t windowHnd = 0;
			std::ostringstream windowHndStr;

			mWindow->getCustomAttribute("WINDOW", &windowHnd);
			windowHndStr << windowHnd;
			pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

			mInputManager = OIS::InputManager::createInputSystem( pl );

			mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
			mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

			mMouse->setEventCallback(this);
			mKeyboard->setEventCallback(this);

			//Set initial mouse clipping size
			windowResized(mWindow);

			//Register as a Window listener
			Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

			OgreBites::InputContext inputContext;
			inputContext.mMouse = mMouse; 
			inputContext.mKeyboard = mKeyboard;
			mTrayMgr = new OgreBites::SdkTrayManager("InterfaceName", mWindow, inputContext,(OgreBites::SdkTrayListener*) this);								

			mTrayMgr->createLabel(OgreBites::TL_TOP,"GameCaption","Welcome to ZingaMino",Ogre::Real(mWindow->getViewport(0)->getActualWidth()*0.6));
			mTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);						
			mTrayMgr->showCursor();			
			Ogre::FontManager::getSingleton().getByName("SdkTrays/Caption")->load();			
			
			DisplayMainMenu();
			//mTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);

			mShowGameRule = false;
			mShowNewGameMenu = false;			
			
			mRoot->addFrameListener(this);
			mGameMgr=0;
		}
		catch(Ogre::Exception ex){
			Ogre::LogManager::getSingletonPtr()->logMessage("*** Errror while initializing the frame ***");
			Ogre::LogManager::getSingletonPtr()->logMessage(ex.getFullDescription());
		}
	}


	bool frameRenderingQueued(const Ogre::FrameEvent& evt){

		if(mWindow->isClosed())
			return false;

		if(mShutDown)
			return false;

		//Need to capture/update each device
		mKeyboard->capture();
		mMouse->capture();

		if(mShowGameRule) 
			DisplayGameRules();

		if(mShowNewGameMenu)
			DisplayNewGameMenu();

		mTrayMgr->frameRenderingQueued(evt);

		return true;
	}


	void buttonHit(OgreBites::Button *btn){

		Ogre::LogManager::getSingletonPtr()->logMessage("ZingaMinoApp: We hit " + btn->getName());		
		if(btn->getName()=="newGameBtn"){
			DisplayNewGameMenu();
			
		}
		else{
			if(btn->getName()=="howtoBtn"){
				DisplayGameRules();
			}
			else{
				if(btn->getName()=="QuitBtn"){
					mShutDown = true;
				}
				else
				{
					if(btn->getName() == "faqBtn")
						DisplayMainMenu();
					else{
						if(btn->getName() =="startGamebtn")
							StartNewGame();						
						else{
							  if(mGameMgr)
								  mGameMgr->buttonHit(btn);							  
							}
						}
					}
					
				}
			}
}	

	void DisplayMainMenu(void){

		if(mTrayMgr){			
			mTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_CENTER);			
			mTrayMgr->createLabel(OgreBites::TL_CENTER,"Mainlbl","Main menu",250);
			mTrayMgr->createButton(OgreBites::TL_CENTER, "newGameBtn","New game",250);
			mTrayMgr->createButton(OgreBites::TL_CENTER,"howtoBtn","FAQ",250);
			mTrayMgr->createButton(OgreBites::TL_CENTER,"QuitBtn","Quit",250);			
			
		}
	}


	void DisplayNewGameMenu(void){

		if(mTrayMgr){
			mTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_CENTER);
			OgreBites::Label* lbl = (OgreBites::Label*) mTrayMgr->createLabel(OgreBites::TL_CENTER,"newGamelbl","New game settings",300);			
			mTrayMgr->createLongSlider(OgreBites::TL_CENTER,"nbPlayersSldr","Players:",150,80,2,4,3);
			//mTrayMgr->createTextBox(OgreBites::TL_CENTER,"playerNameTb","PlayerName",300,200);
			mTrayMgr->createButton(OgreBites::TL_CENTER,"startGamebtn","Start",lbl->getOverlayElement()->getWidth());			
			
		}
	}


	void DisplayGameRules(void){
		if(mTrayMgr){
			mTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_CENTER);
			OgreBites::TextBox* rulestb = mTrayMgr->createTextBox(OgreBites::TL_CENTER,"ruletb","The game rules",300,200);			
			rulestb->appendText(rules);
			mTrayMgr->createButton(OgreBites::TL_CENTER,"faqBtn","OK",300);
		}
	}


	void StartNewGame(){
		int nbPlayers = (int)((OgreBites::Slider*) mTrayMgr->getWidget("nbPlayersSldr"))->getValue();

		//mRoot->removeFrameListener(this);
		if(mGameMgr) mGameMgr=0;
		mGameMgr = new GameManager(mSceneMgr,mRoot,mWindow,mTrayMgr);
		mGameMgr->StartGame(nbPlayers,"Me");
	}


	bool mouseMoved( const OIS::MouseEvent &arg )
{
    if (mTrayMgr->injectMouseMove(arg)) return true;
    mCameraMan->injectMouseMove(arg);
	if(mGameMgr)
		mGameMgr->FrmListener->mouseMoved(arg);
    return true;
}

bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    if (mTrayMgr->injectMouseDown(arg, id)) return true;
    mCameraMan->injectMouseDown(arg, id);
	if(mGameMgr)
		mGameMgr->FrmListener->mousePressed(arg,id);

    return true;
}

bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    if (mTrayMgr->injectMouseUp(arg, id)) return true;
    mCameraMan->injectMouseUp(arg, id);
	if(mGameMgr)
		mGameMgr->FrmListener->mouseReleased(arg,id);
    return true;
}
};

#endif 