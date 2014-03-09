#pragma once

#include "Partie.h"
#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <SdkTrays.h>
#include <SdkCameraMan.h>

#include <vector>
#include <string>
#include <list>
#include <boost\lexical_cast.hpp>
typedef struct InfoP
{	
	std::vector<Ogre::SceneNode*> NoeudPiece; //La liste de pieces
	std::vector<int> NoeudVide;         //La position des pieces vides

} InfoPlayers;



class GameFrameListener : public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::MouseListener, OgreBites::SdkTrayListener
{

public:

	std::vector<Ogre::AnimationState*> Animations;
	//CEGUI::Window *mFen;
	//CEGUI::System *mGUIS;
	Ogre::SceneManager * GestScene;
	//CEGUI::Renderer *R;
	bool inv;	
	OgreBites::SdkTrayManager* mTrayMgr;
	Ogre::Camera *mCamera;
	Ogre::RenderWindow* mWindow;
	
	Ogre::Plane P;
	bool CanHMove;
	bool equal;
	int tour,Etat;
	std::string NameFirst;


	GameFrameListener(void);
	virtual ~GameFrameListener(void);

	GameFrameListener(Ogre::SceneManager *S,Ogre::Camera *C, Ogre::Root *R,Ogre::RenderWindow *Rw,OgreBites::SdkTrayManager* Tm);	


	virtual bool frameStarted(const Ogre::FrameEvent& evt);
	
	virtual void AddAnimStat(Ogre::AnimationState *A);
	
	virtual bool mouseMoved( const OIS::MouseEvent &arg );
		
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

	//Se déclanche quand le bouton de la sourie est rélach?
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	
	virtual void PoserPiece(Piece Tail,int Pos,int NumJoueur);

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	virtual void Selection(void);

	virtual void MetreAjourTable(std::pair<Piece,int> Ajouer);

	virtual void UpdatePieces(std::pair<Piece,int> Ajouer, std::vector<std::pair<Piece,bool>>* dominos );

	virtual void GetLandingPosition(std::vector<std::pair<Piece,bool>>* dominoSet, Ogre::Vector3* Nearest,int* Pos,bool* retourn, std::vector<Ogre::Vector3>* nodesPos, std::pair<Piece,bool>* Place);

	virtual void Updateboard(std::pair<Piece,bool>* Tail,int Pos,Color col,std::vector<std::pair<Piece,bool>>* dominosSet,
				 std::vector<std::pair<Piece,bool>>* neighbor1,std::vector<std::pair<Piece,bool>>* neighbor2);

	virtual void Animer(Ogre::SceneNode*  Noeud,Ogre::Vector3 StartPos,Ogre::Vector3 EndPos);

	virtual void AfficherScore(void);
	




	enum PieceMask
	{
		CANMOVE = 1<<1, //CANMOVE =00000001 Ce mask est utilis?pour indiqu?si une piéce peut être déplacer
		CANTMOVE=1<<2  //CANTMOVE =00000010
	};	


	std::vector<InfoPlayers> Infos;

	std::vector<Ogre::Vector3> NoeudV; //Contient la postions des noeuds Vert
	std::vector<Ogre::Vector3> NoeudJ; //Contient la postions des noeuds Jaune
	std::vector<Ogre::Vector3> NoeudVI;//Contient la postions des noeuds Violet
	std::vector<Ogre::Vector3> NoeudO; //Contient la postions des noeuds Orange

	std::vector<std::pair<Piece,bool>> PiecesV; //Contient les pieces Verte  posées 
	std::vector<std::pair<Piece,bool>> PiecesJ; //Contient les pieces Jaune  posées 
	std::vector<std::pair<Piece,bool>> PiecesVI;//Contient les pieces Violet posées 
	std::vector<std::pair<Piece,bool>> PiecesO; //Contient les pieces Orange posées 

	std::vector<Ogre::Vector3> Centres; //Contient les coordonnées des centres des 4 cercles
	Partie Jeux;
	std::vector<Player> Players; //Contient les infos sur les Joueurs
	int NumCoups;         //Num du coups
	int turn;             //indique le tour du joueur qui va jouer
	int CanPlay;         //Indique si ce joueur peut jouer
	int Premier; //Le numero du joueur qui va commanc?en premier
	int Place;
	int CantPlay; 
	bool StartSelect;

protected:
	
	std::list<Ogre::SceneNode*> Dominos;
	Ogre::Real Attante;	
	Ogre::RaySceneQuery *mRaySQuery; 
	Ogre::SceneNode *mCurrentObject;
	Ogre::SceneNode *ToMove;
	Ogre::Vector3 PrePos;
	Ogre::Root *R;
	bool OnBougeDeja;
	//EventProcessor* mEventPros;
	//InputReader *mIR;
	int PosPiece; //La position de piece tirée dans la main du joueur
	unsigned long NbRepeat;
};

