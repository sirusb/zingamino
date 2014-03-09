#pragma once

#ifndef __GameManager_h_
#define __GameManager_h_

#include "Partie.h"
#include <Ogre.h>
#include <OgreSceneNode.h>
#include "GameFrameListener.h"

using namespace Ogre;
using namespace std;




class GameManager : public  OgreBites::SdkTrayListener
{
private:	
	Ogre::SceneManager * mScene;
	Ogre::Camera *mCamera;
	OgreBites::SdkTrayManager* mTrayMgr;
	//CEGUI::Window * mFenetre;
	//CEGUI::System * mGUISystem;
	std::vector<Ogre::SceneNode*> Dominos;
	Root *mRacine;

	void InitScene(void)
	{
		//Create the game board
		Entity *ent=mScene->createEntity("Table","Cube.mesh");
		SceneNode *Node = mScene->getRootSceneNode()->createChildSceneNode("TableNoeud");
		ent->setCastShadows(true);
		Node->attachObject(ent);
		Node->scale(Vector3(800,1,800));
		mCamera->setLodBias(1);
		mCamera->pitch(Ogre::Degree(-90));
		mCamera->yaw(Ogre::Degree(180));
		//Creation de la table
		Plane plane(Vector3::UNIT_Y,0);
		MeshManager::getSingleton().createPlane("Terrain",
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
			4500,4500,20,20,true,1,5,5,Vector3::UNIT_Z);

		FrmListener->P = plane;
		ent= mScene->createEntity("ground","Terrain");
		SceneNode *nod=mScene->getRootSceneNode()->createChildSceneNode("Table");
		nod->attachObject(ent);
		ent->setMaterialName("N012"); //Texture en bois
		ent->setCastShadows(false);
		ent->setQueryFlags(GameFrameListener::CANTMOVE);
		
		//Creation de la source de la lumiere
		Light* light=mScene->createLight("Lumiere");
		light->setType(Light::LT_POINT);
		light->setPosition(Ogre::Vector3(0,1000,0));
		light->setDiffuseColour(ColourValue::White);
		light->setSpecularColour(ColourValue::White);

		//Creation de la lumiere
		Light* l = mScene->createLight("MainLight");
		l->setPosition(20,80,50);

		mScene->setAmbientLight(ColourValue(1,1,1));


		//Creation des emplacement des Domino dans la Table	
		FrmListener->Centres.push_back(Ogre::Vector3(360,10,360)); //Le cercle Vert
		FrmListener->Centres.push_back(Ogre::Vector3(-360,10,360)); //Le cercle Jaune
		FrmListener->Centres.push_back(Ogre::Vector3(-360,10,-360)); //Le cercle Violet
		FrmListener->Centres.push_back(Ogre::Vector3(360,10,-360)); //Le cercle Orange

		float Alpha; //L'angle de deviation de la case sur le centre
		Vector3 Pos(0,60,0); //Contient la Position du SceneNode a place
		int j=0;
		std::vector<Ogre::Vector3>::iterator iter;
		for(iter=FrmListener->Centres.begin();iter!=FrmListener->Centres.end();iter++)
		{
			Alpha=0;
			for(int i=0;i<16;i++)
			{
				if(i%4==0) Pos= *(iter)+ Ogre::Vector3(310*sin(Alpha),0,310*cos(Alpha));
				else
					Pos= *(iter)+ Ogre::Vector3(320*sin(Alpha),0,320*cos(Alpha));
				switch(j)
				{
				case 0:
					FrmListener->NoeudV.push_back(Pos);
					break;
				case 1:
					FrmListener->NoeudJ.push_back(Pos);
					break;
				case 2:
					FrmListener->NoeudVI.push_back(Pos);
					break;
				case 3:
					FrmListener->NoeudO.push_back(Pos);
					break;
				}
				Alpha+=0.391f;
			}
			j++;
		};

		FrmListener->PiecesJ.resize(16); 
		FrmListener->PiecesO.resize(16);
		FrmListener->PiecesV.resize(16);
		FrmListener->PiecesVI.resize(16);
		pair<Piece,bool> T;
		T.second=false;
		T.first.Tete=9;
		T.first.Queue=9;
		for(int i=0;i<16;i++)
		{
			FrmListener->PiecesJ[i]=T;
			FrmListener->PiecesO[i]=T;
			FrmListener->PiecesV[i]=T;
			FrmListener->PiecesVI[i]=T;
		};

		//Creation des emplacement des Dominos dans les mains des Joeurs
		std::vector<Ogre::Vector3> Positions;
		Positions.push_back(Ogre::Vector3(-600,30,-900));
		Positions.push_back(Ogre::Vector3(-600,30,900));
		Positions.push_back(Ogre::Vector3(-900,30,-600));
		Positions.push_back(Ogre::Vector3(900,30,-600));


		Ogre::Vector3 Decale;
		int Degre,rot;
		string EName;
		//Le Player 0 = Humain , 1:P1 ... 4:P4
		FrmListener->Infos.clear();
		for(int j=0;j<4;j++)
		{
			if(j<2){
				Decale=Ogre::Vector3(80,0,0);
				Degre=180;
				if(j<1)rot=-90;
				else rot=90;
			};
			if((j>1)&&(j<4)){
				Decale=Ogre::Vector3(0,0,80);
				Degre=90;
				rot=90;
			};
			Pos =Positions.front();
			InfoPlayers Inf;
			for(int i=0;i<8;i++)
			{
				//Le nom d'un noeud est "+ n°Joueur + n°Piece
				EName="P"+StringConverter::toString(j)+ StringConverter::toString(i);
				Node = mScene->getRootSceneNode()->createChildSceneNode(EName,Pos);
				Node->yaw(Ogre::Degree(Degre));
				Node->pitch(Ogre::Degree(rot));
				Node->scale(Ogre::Vector3(8,8,8));
				Pos=Pos+Decale;
				Inf.NoeudPiece.push_back(Node);//On empile le noeud
				Inf.NoeudVide.push_back(i);//On empile le Numeroe du noeud vide
			};
			FrmListener->Infos.push_back(Inf); //On ajoute les infos du FrmListener->Players J
			Positions.erase(Positions.begin(),Positions.begin()+1);
		}

		Node=mScene->createSceneNode("Tansporteur");

	};
public:
	GameFrameListener *FrmListener;
    GameManager(void);
	virtual ~GameManager(void);


	GameManager(Ogre::SceneManager *mSceneManager,Ogre::Root *mR,Ogre::RenderWindow *RW, OgreBites::SdkTrayManager* stm)
	{
		mScene =mSceneManager;		
		mCamera =mScene->getCamera("PlayerCam");
		//mScene->setSkyDome(false, "Examples/TextureEffect3", 5, 8);
		mTrayMgr = stm;
		try{			
			FrmListener= new GameFrameListener(mScene,mCamera,mR,RW,stm);
			mR->addFrameListener(FrmListener);
		}
		catch(Ogre::Exception& e)
		{
			Ogre::LogManager::getSingletonPtr()->logMessage(Ogre::StringConverter::toString("Error GameManager constructor :")+ e.getFullDescription());
		}		
		
	};

	void StartGame(int NbPlayers,string NomHumain)
	{
		//using namespace CEGUI;
		//1:Affichage de la Bare D'outils
		//if(mFenetre) mFenetre->destroy();		
		mTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_CENTER);	
		mTrayMgr->createButton(OgreBites::TL_CENTER,"Start","Select a piece",250);
		//mTrayMgr->createButton(OgreBites::TL_NONE,"TirerPieces","Start round",250);
		/*mTrayMgr->createButton(OgreBites::TL_RIGHT,"Creuser","Dig",250);*/
		
		mTrayMgr->createLabel(OgreBites::TL_TOPLEFT,"infoPanel","Scores",250);
		OgreBites::TextBox* humanScore = mTrayMgr->createTextBox(OgreBites::TL_TOPLEFT,"Inist","Your Score",250,90);
		humanScore->setText("00");
		//StaticText* ZoneNom =(StaticText*) WindowManager::getSingleton().getWindow((utf8*)"Inist");
		
		/*CEGUI::PushButton * Pick=(CEGUI::PushButton*)CEGUI::WindowManager::getSingleton().getWindow("TirerPieces");
		Pick->hide();*/
		//ZoneNom->setText(NomHumain+"     00");
		string num="P1";
		for(int i=1;i<NbPlayers;i++)
		{
			OgreBites::TextBox* ZoneNom = mTrayMgr->createTextBox(OgreBites::TL_TOPLEFT,num,num+" Score",250,90);
			//ZoneNom =(StaticText*) WindowManager::getSingleton().getWindow(num);
			ZoneNom->setText("00");
			/*ZoneNom->setFrameEnabled(true);
			ZoneNom->setBackgroundEnabled(true);*/
			num[1]++;
		}

		mTrayMgr->createTextBox(OgreBites::TL_BOTTOM,"Tour","Info Pabel",400,80);
		//2:Initialisation de la Scene
		InitScene();
		//3:Initialisation des FrmListener->Players
		FrmListener->Jeux=Partie(NbPlayers,NomHumain);//On crée une partie avec NbPlayers
		FrmListener->Players = FrmListener->Jeux.GetPlayers(); //MAJ de la liste des FrmListener->Players
	};

	
	void buttonHit(OgreBites::Button *btn){
		Ogre::LogManager::getSingletonPtr()->logMessage("GameManaget: We hit " + btn->getName());
		if(btn->getName()=="Start"){
			
			LanceSeletion();
		}
		else{
			if(btn->getName()=="TirerPieces"){
				StartRound();
			}
			else{
				if(btn->getName()=="Creuser"){
					Creuser();
				}
			}
		}
	}

	bool LanceSeletion(void)
	{
		//using namespace CEGUI;
		FrmListener->Etat=2;
		//On Cache le bouton Start
		mTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_CENTER);
		//OgreBites::Button* Start = (OgreBites::Button*) mTrayMgr->getWidget("Start");
		//CEGUI::PushButton * Start=(CEGUI::PushButton*)CEGUI::WindowManager::getSingleton().getWindow("Start");
		//Start->hide();
		FrmListener->StartSelect=true;
		return true;
	};


	
	bool StartRound(void)
	{
		mTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_CENTER);
		//OgreBites::Button* Tirer = (OgreBites::Button*) mTrayMgr->getWidget("TirerPieces");
		//CEGUI::StaticText* Tirer =(CEGUI::StaticText*) CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"TirerPieces");
		
		mTrayMgr->createButton(OgreBites::TL_RIGHT,"Creuser","Dig",250);
		int nbPlayers =(int)FrmListener->Players.size();
		string EntityName="Domino0";
		string SceneName="Scene0";
		OgreBites::TextBox* ZoneTour= (OgreBites::TextBox*) mTrayMgr->getWidget("Tour");
		//CEGUI::StaticText* ZoneTour =(CEGUI::StaticText*) CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"Tour");

		ZoneTour->setText("Debut De la Partie");
		mScene->clearScene();
		mCamera->pitch(Ogre::Degree(90));
		mCamera->yaw(Ogre::Degree(-180));
		InitScene();
		FrmListener->Jeux.DebuteRound(); //Chaque Player Tire 8 Pieces
		FrmListener->Players =FrmListener->Jeux.GetPlayers(); //mise ?jour dans liste des informations des FrmListener->Players

		//On Affiche les Dominos Prit par chaque Player
		int NumP=0; //Indique le numero du FrmListener->Players
		std::vector<Player>::iterator itr;
		GameFrameListener::PieceMask Pmask=GameFrameListener::CANMOVE;
		std::vector<Piece> P;
		std::vector<Piece>::iterator Pitr;
		Entity *Ent;
		SceneNode *nod;
		//Ici chaque on affiche les pieces tirer par chaque Player
		for(itr=FrmListener->Players.begin();itr!=FrmListener->Players.end();itr++)
		{
			//On peut faire Bouger que les pieces du Player Humain
			if(NumP>0 && Pmask==GameFrameListener::CANMOVE) Pmask=GameFrameListener::CANTMOVE;  
			P= itr->GetPiece();
			int NumPiece=0;
			for(Pitr=P.begin();Pitr!=P.end();Pitr++)
			{
				//On Obtient la Scene avec le nom "P+numoueur+num_piece"
				nod=mScene->getSceneNode("P"+StringConverter::toString(NumP)+ StringConverter::toString(NumPiece) );
				if(Pitr->Tete<Pitr->Queue)
					EntityName=StringConverter::toString(Pitr->Tete)+StringConverter::toString(Pitr->Queue);
				else
					if(Pitr->Queue!=-1)EntityName=StringConverter::toString(Pitr->Queue)+StringConverter::toString(Pitr->Tete);
					else EntityName="Zinga";

					switch(Pitr->Couleur)
					{
					case Color::Jaune: EntityName+="J";
						break;
					case Color::Orange: EntityName+="O";
						break;
					case Color::Vert: EntityName+="V";
						break;
					case Color::Violet : EntityName+="VI";
						break;
					};

					Ent =mScene->createEntity(EntityName,EntityName+".mesh"); //On charge le fichier du domino
					Ent->setQueryFlags((NumP==0)? GameFrameListener::CANMOVE : GameFrameListener::CANTMOVE); //On definie sont Drapeau 
					nod->attachObject(Ent); //Et on l'attach a la scene distination
					if(EntityName[0]=='Z') nod->scale(Ogre::Vector3(4,4,4));
					NumPiece++; //On passe ?la piece suivante
			};
			NumP++; //On passe au Player suivant
		};
		FrmListener->NumCoups=1;
		FrmListener->turn=FrmListener->Premier;
		FrmListener->CanPlay=true;
		return false;
	};

	bool Creuser(void)
	{
		if(FrmListener->turn==0)
		{
			FrmListener->Players= FrmListener->Jeux.GetPlayers();
			std::vector<Piece> Pochette=FrmListener->Jeux.GetPochete();
			if(Pochette.size()>0)
			{
				Piece P;
				P.Couleur=Pochette[0].Couleur;
				P.Queue=Pochette[0].Queue;
				P.Tete=Pochette[0].Tete;
				FrmListener->Players[0].AddPiece(P);
				Pochette.erase(Pochette.begin());
				FrmListener->Jeux.MajPlayer(FrmListener->Players);
				FrmListener->Jeux.SetPochette(Pochette);

				Ogre::String N;
				if(P.Tete==-1) N="Zinga";
				else if(P.Tete<P.Queue) N=StringConverter::toString(P.Tete)+StringConverter::toString(P.Queue);
				else
					N=StringConverter::toString(P.Queue)+StringConverter::toString(P.Tete);

				switch(P.Couleur)
				{
				case Color::Vert:
					N+="V";
					break;
				case Color::Jaune:
					N+="J";
					break;
				case Color::Violet:
					N+="VI";
					break;
				case Color::Orange:
					N+="O";
					break;
				}
				InfoPlayers I;
				I=FrmListener->Infos[0];
				Ogre::Vector3 V=I.NoeudPiece.back()->getPosition();
				V+=Ogre::Vector3(80,0,0);
				SceneNode *node=mScene->getRootSceneNode()->createChildSceneNode(N+"S",V);
				Entity *E=mScene->createEntity(N,N+".mesh");
				node->attachObject(E);
				if(P.Tete!=-1 && P.Tete!=0) node->scale(8,8,8);
				else node->scale(9,9,9);
				node->yaw(Ogre::Degree(180));
				node->pitch(Ogre::Degree(-90));
				I.NoeudPiece.push_back(node);
				FrmListener->Infos[0]=I;
			}
			else //On passe notre tour
			{
				FrmListener->CantPlay++;
				OgreBites::Button* Piocher =  (OgreBites::Button*) mTrayMgr->getWidget("Creuser");
				//CEGUI::PushButton* Piocher =(CEGUI::PushButton*)CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"Creuser");
				Piocher->setCaption("Pass");				
				FrmListener->CanPlay=true;
				if(FrmListener->turn==0 &&FrmListener->Players.size()==4) FrmListener->turn=3;
				else
					if(FrmListener->turn==0) FrmListener->turn=1;
					else
						if(FrmListener->turn==1)FrmListener->turn=(FrmListener->turn+1)%(int)FrmListener->Players.size();
						else if(FrmListener->turn==2) FrmListener->turn=0;
						else if(FrmListener->turn==3) FrmListener->turn=1;
			}
		}
		return true;
 }
};

#endif
