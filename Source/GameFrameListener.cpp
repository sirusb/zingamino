#include "GameFrameListener.h"


GameFrameListener::GameFrameListener(void)
{
}

GameFrameListener::GameFrameListener(Ogre::SceneManager *S,Ogre::Camera *C, Ogre::Root *R,Ogre::RenderWindow *Rw,OgreBites::SdkTrayManager* Tm)		
{
	Etat=-2;
	Attante=1;
	mCurrentObject =NULL;
	mRaySQuery =NULL;
	OnBougeDeja=false;
	mTrayMgr = Tm;
	mCamera = C;
	mWindow= Rw;
	mRaySQuery =S->createRayQuery(Ogre::Ray());		
	CanPlay =0;
	NbRepeat=0;
	GestScene=S;
	this->R =R;	
};

bool GameFrameListener::frameStarted(const Ogre::FrameEvent& evt)

{

	OgreBites::TextBox* ZoneTour =  (OgreBites::TextBox*) mTrayMgr->getWidget("Tour");

	//CEGUI::StaticText* ZoneTour =(CEGUI::StaticText*)CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"Tour");
	switch(Etat)
	{
		//Si On est Dans La séléction
	case 0:
		if(Animations.size()>0)
		{
			Ogre::AnimationState *mAnimStat;
			std::vector<Ogre::AnimationState*>::iterator iter;
			iter=Animations.begin();
			mAnimStat =*iter;
			if(mAnimStat->getTimePosition()<mAnimStat->getLength()) mAnimStat->addTime(evt.timeSinceLastFrame);
			else
			{
				tour++;					
				ZoneTour->setText("P"+boost::lexical_cast<std::string>(tour)+" Takes a Piece");
				Animations.erase(iter,iter+1);
			};
		}
		else
			if(equal)
			{
				ZoneTour->setText("Draw");
				Etat=2;
				NbRepeat++;
				//Les Joueurs vont remetre les pieces
				std::vector<Player>::iterator iter;
				//Premierement les joueurs Posent les pieces tirer dans Pour choisir le premeier
				std::vector<Piece> Pochette =Jeux.GetPochete();
				for(iter=Players.begin();iter!=Players.end();iter++)
				{
					Pochette.push_back(iter->GetPiece().front());
					iter->RemetrePiece();
				};
				Jeux.MajPlayer(Players);
				Jeux.SetPochette(Pochette);
				GestScene->destroyAllAnimations();
			}
			else
			{
				ZoneTour->setText(NameFirst+" Will start fisrt");
				if(Attante>0)Attante-=evt.timeSinceLastFrame;
				else
				{
					Ogre::LogManager::getSingletonPtr()->logMessage("GameFrameListener Line 75");
					mTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_CENTER);			
					mTrayMgr->createButton(OgreBites::TL_CENTER,"TirerPieces","Start round",250);										
					Attante=1;
					Etat=1;
				}
			}
			break;
			//Si On est entrains de Jouer
	case 1:
		if(Animations.size()>0)
		{
			Ogre::AnimationState *mAnimStat;
			std::vector<Ogre::AnimationState*>::iterator iter;
			iter=Animations.begin();
			mAnimStat =*iter;
			if(mAnimStat->getTimePosition()<mAnimStat->getLength()) mAnimStat->addTime(evt.timeSinceLastFrame);
			else
			{
				//GestScene->destroyAllAnimations();
				float Alpha =0.389*Place; 
				Ogre::Radian Rot;
				(*iter)->setEnabled(false);
				//On remet tous a l'etat initial puis on applique les rotations
				//Si elle est dans les poisitions Zinga/Double
				if(Place==0 || Place==8) ToMove->roll(Ogre::Degree(180));//PI/2
				else
					if(Place==4 || Place==12) ToMove->roll(Ogre::Degree(90));//PI
					else//Sinon s'il elle est dans une autre place
						if(Place<9)
						{
							Rot=-atan(cos(Alpha)/abs(sin(Alpha)));
							ToMove->roll(Rot);
						}
						else 
						{
							Rot=-atan(-cos(Alpha)/abs(sin(Alpha)));
							ToMove->roll(Rot);
						}
						if(inv){
							ToMove->roll(Ogre::Degree(180));
							ZoneTour->setText("I have inversed");}
						if(turn==0 &&Players.size()==4) turn=3;
						else
							if(turn==0) turn=1;
							else
								if(turn==1)turn=(turn+1)%(int)Players.size();
								else if(turn==2) turn=0;
								else if(turn==3) turn=1;
								if(turn==Premier) NumCoups++;
								Animations.erase(iter,iter+1);
								CanPlay=true;
								Ogre::String T="Tour de ";
								if(turn==0) T+= Players[0].GetNom();
								else T+="P"+boost::lexical_cast<std::string>(turn);
								ZoneTour->setText(T);
			}
		}
		if(turn==0) CanHMove=true;
		else
			if(CanPlay )
			{
				if(CantPlay < (int)Players.size())
				{
					//Mise ?jour de la liste de joeurs
					ZoneTour->setText("I will play");
					Players=Jeux.GetPlayers();
					Player J=Players[turn];
					int nbp=(int)J.GetPiece().size();
					std::pair<Piece,int> PieceAjouer =J.Joue(NumCoups==1,PiecesV,PiecesJ,PiecesVI,PiecesO,Jeux.GetPochete());
					if(PieceAjouer.second!=-3 && PieceAjouer.second<16)
					{   CantPlay =0;
					Place=PieceAjouer.second;
					//Mise ?jours des Pieces dans la Table;
					int Diff=(int)J.GetPiece().size()-nbp;
					CanPlay=false;
					if(Diff>0)
					{//Jeux.Enleve(Diff);
						InfoPlayers I= Infos[turn];
						//On obtient la postion du dérnier noeud dans la Main du joueur
						Ogre::Vector3 Pos=I.NoeudPiece.back()->getPosition();
						Ogre::Vector3 Decale;
						int Degre,rot;
						if(turn<2){
							Decale=Ogre::Vector3(80,0,0);
							Degre=180;
							if(turn<1)rot=-90;
							else rot=90;
						};
						if((turn>1)&&(turn<4)){
							Decale=Ogre::Vector3(0,0,80);
							Degre=90;
							rot=90;
						};
						Pos+=Decale;
						int S=(int)J.GetPiece().size()-1;
						std::vector<Piece> Tails=J.GetPiece();
						std::vector<Piece> U=Jeux.GetPochete();
						while(Diff>0)
						{
							Ogre::String EName;
							if(Tails[S].Tete==-1) EName="Zinga";
							else
								if(Tails[S].Tete<Tails[S].Queue)
									EName=boost::lexical_cast<std::string>(Tails[S].Tete)+boost::lexical_cast<std::string>(Tails[S].Queue);
								else
									EName=boost::lexical_cast<std::string>(Tails[S].Queue)+boost::lexical_cast<std::string>(Tails[S].Tete);
							switch(Tails[S].Couleur)
							{
							case Color::Vert:
								EName+="V";
								break;
							case Color::Jaune:
								EName+="J";
								break;
							case Color::Violet:
								EName+="VI";
								break;
							case Color::Orange:
								EName+="O";
							}
							//On crée un Noued qui va contenir le Domino
							Ogre::SceneNode *N= GestScene->getRootSceneNode()->createChildSceneNode(EName+"S",Pos);
							N->setPosition(Pos); //On le met ?la position désirée
							N->scale(8,8,8);
							Ogre::Entity *E=GestScene->createEntity(EName,EName+".mesh");
							N->attachObject(E);
							N->yaw(Ogre::Degree(Degre));
							N->pitch(Ogre::Degree(rot));
							Pos=Pos+Decale;
							I.NoeudPiece.push_back(N);//On empile le noeud
							Infos[turn]=I;
							S--;
							Diff--;
							U.erase(U.begin());
						}
						Jeux.SetPochette(U);
					}//if Diff
					J.Remove(PieceAjouer.first);
					Players[turn]=J;
					Jeux.MajPlayer(Players);
					MetreAjourTable(PieceAjouer);
					PoserPiece(PieceAjouer.first,PieceAjouer.second,turn);
					if((int)J.GetPiece().size() ==0) //Si le joueur ?joueur tous ces pieces alors Il a gagn?
					{
						OgreBites::Button *Start = (OgreBites::Button*) mTrayMgr->getWidget("TirerPieces");
						//CEGUI::PushButton * Start=(CEGUI::PushButton*)CEGUI::WindowManager::getSingleton().getWindow("TirerPieces");
						Start->show();
						Jeux.EndRound();
						Players =Jeux.GetPlayers();
						AfficherScore();
						Etat =3;
					}
					}
					else//Si le joueur ne peut pas Jouer alors il passe la main ?l'autre joueur
					{ 
						ZoneTour->setText("P"+boost::lexical_cast<std::string>(turn)+" :I pass my turn");
						CantPlay++;
						if(turn==0 &&Players.size()==4) turn=3;
						else
							if(turn==0) turn=1;
							else
								if(turn==1)turn=(turn+1)%(int)Players.size();
								else if(turn==2) turn=0;
								else if(turn==3) turn=1;

					}
				}
				else
				{
					
					mTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_CENTER);						
					mTrayMgr->createButton(OgreBites::TL_CENTER,"TirerPieces","Start round",250);
					//OgreBites::Button *Start = (OgreBites::Button*) mTrayMgr->getWidget("TirerPieces");
					//CEGUI::PushButton * Start=(CEGUI::PushButton*)CEGUI::WindowManager::getSingleton().getWindow("TirerPieces");
					//Start->show();
					Jeux.EndRound();
					Players =Jeux.GetPlayers();
					AfficherScore();
					Etat =3;
				}
			}//IF(CanPlay)
			break;
	case 2:
		Selection();
	case 3:
		break;
	};


	return true;
};



void GameFrameListener::AddAnimStat(Ogre::AnimationState *A)
{
	Animations.push_back(A);
};


bool GameFrameListener::mouseMoved( const OIS::MouseEvent &arg )	
{
	Ogre::LogManager::getSingletonPtr()->logMessage("GameFrameListener::mouseMoved");
	if (mTrayMgr->injectMouseMove(arg)) return true;
	//mCameraMan->injectMouseMove(arg);

	//Si on est entrain de bouger un noeud
	if(OnBougeDeja)
	{

		//CEGUI::Point Pos= CEGUI::MouseCursor::getSingleton().getPosition();
		Ogre::Real x= ((float)arg.state.X.abs) / arg.state.width;
		Ogre::Real y= ((float)arg.state.Y.abs) / arg.state.height;
		Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(x,y);
		mRaySQuery->setRay( mouseRay );
		mRaySQuery->setSortByDistance(true);
		//mRaySQuery->setQueryMask(CANTMOVE);

		Ogre::RaySceneQueryResult &result = mRaySQuery->execute();
		Ogre::RaySceneQueryResult::iterator itr;

		for(itr=result.begin();itr!=result.end();itr++)
		{
			std::pair<bool,Ogre::Real> T= mouseRay.intersects(P);
			if(T.first) //Si il y a intecetion 
				mCurrentObject->setPosition(mouseRay.getPoint(T.second)+Ogre::Vector3(0,20,0));

		}
		mRaySQuery->clearResults();
		//GestScene->destroyQuery(mRaySQuery);
	}
	return true;
};

bool GameFrameListener::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	Ogre::LogManager::getSingletonPtr()->logMessage("GameFrameListener::mousePressed");
	//Si c'est notre tour alors on peut prendre une piéce
	if(CanHMove)
	{
		//Et si on a cliquer avec le bouton Gaunche 
		if (mTrayMgr->injectMouseUp(arg, id)) return true;


		if(id == OIS::MouseButtonID::MB_Left)
		{

			//CEGUI::Point Pos= CEGUI::MouseCursor::getSingleton().getPosition();				
			Ogre::Real x= ((float) arg.state.X.abs) / arg.state.width;
			Ogre::Real y= ((float) arg.state.Y.abs) / arg.state.height ;
			//On Lance une ligne droite apartire de la position de la sourie
			Ogre::Ray CliquePos = mCamera->getCameraToViewportRay(x,y);
			mRaySQuery->setRay(CliquePos);
			//On trie les Objet qui sont dans le chemain de cette ligne par distance (Ordre croissant)
			mRaySQuery->setSortByDistance(true); 
			//On utilise ce masque pour obtnire que les objets qui ont ce maske
			//mRaySQuery->setQueryTypeMask(CANMOVE); 

			//Et on execute la requette
			Ogre::RaySceneQueryResult resultat= mRaySQuery->execute();
			if((int)resultat.capacity()>0)
			{
				Ogre::RaySceneQueryResult::iterator itr =resultat.begin();
				//node= Le SceneNode qui contient le domino selectionn?
				PrePos =itr->movable->getParentNode()->getPosition();
				Ogre::SceneNode *PrevNoeud =itr->movable->getParentSceneNode();
				bool exist=false;
				std::vector<Ogre::SceneNode*>::iterator it;
				PosPiece=0;
				//On cherche la position de cette piece dans la main du Joueur
				InfoPlayers Inf= Infos[0];
				for(it=Inf.NoeudPiece.begin();it!=Inf.NoeudPiece.end();it++)
				{
					if((*it)->getPosition()==PrevNoeud->getPosition())
					{
						exist=true;
						break;
					}
					PosPiece++;
				}
				if(exist)
				{
					mCurrentObject= itr->movable->getParentSceneNode();
					OnBougeDeja=true;
					CanPlay=false;
				}
				else PosPiece=-1;
				mRaySQuery->clearResults();
				//GestScene->destroyQuery(mRaySQuery);
			}
		};
	};

	/*CEGUI::MouseButton B;
	switch(e->getButtonID())
	{
	case MouseEvent::BUTTON0_MASK:
		B= CEGUI::LeftButton;
		break;
	case MouseEvent::BUTTON1_MASK:
		B=CEGUI::RightButton;
	}
	CEGUI::System::getSingleton().injectMouseButtonDown(B);*/
	
	return true;
};

bool GameFrameListener::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	/*CEGUI::MouseButton B;
	switch(e->getButtonID())
	{
	case MouseEvent::BUTTON0_MASK:
		B= CEGUI::LeftButton;
		break;
	case MouseEvent::BUTTON1_MASK:
		B=CEGUI::RightButton;
	}*/
	Ogre::LogManager::getSingletonPtr()->logMessage("GameFrameListener::mouseReleased");
	if (mTrayMgr->injectMouseUp(arg, id)) return true;

	if(OnBougeDeja)
	{  
		//On indique qu'on bouge pas
		OnBougeDeja=false;
		mCurrentObject->showBoundingBox(false);
		Ogre::String N= mCurrentObject->getAttachedObject(0)->getName();
		int col=0,Pos;
		bool retourn=false,Flip=false; 
		Ogre::Vector3 Nearest;
		int T,Q;
		if(N[0]=='Z')
		{
			T=-1;
			Q=-1;
		}
		else
		{
			T= Ogre::StringConverter::parseUnsignedInt(N.substr(0,1));//Tete de la piece
			Q= Ogre::StringConverter::parseUnsignedInt(N.substr(1,1));//Queue de la piece
			if(T==0 || T==3)
			{
				int C=T;
				T=Q;
				Q=C;
			}
		}

		int j=(int)N.length();
		//On Verifie si la piece est dans 
		//Si on est dans le premier coups et la piece posée n'est pas un double alors on la fait 
		//retournée
		if(NumCoups==1 && T!=Q) retourn=true; 
		else
		{
			std::pair<Piece,bool> Place;
			switch(N[j-1])
			{
			case 'V':
				{
					col=0;
					GetLandingPosition(&PiecesV,&Nearest,&Pos,&retourn,&NoeudV, &Place);					
				}
				break;
			case 'J':
				{
					col=1;
					GetLandingPosition(&PiecesJ,&Nearest,&Pos,&retourn,&NoeudJ,&Place);										
				}
				break;
			case 'I':
				{
					col=2;
					GetLandingPosition(&PiecesVI,&Nearest,&Pos,&retourn,&NoeudVI, &Place);					
				}
				break;
			case 'O':
				{
					col=3;
					GetLandingPosition(&PiecesO,&Nearest,&Pos,&retourn,&NoeudO, &Place);					
				}

			}//Switch

			if(!retourn)
			{   //Si c'est une Zinga alors on met sa valeur par convention a la tete
				//Puis on verifie
				if(T==-1)
				{
					if(Place.first.Tete!=9 && Place.first.Tete!=10)
					{
						T=Place.first.Tete;
						Q=T;
					}
					if(Place.first.Queue!=9 && Place.first.Queue!=10)
					{
						T=Place.first.Queue;
						Q=T;
					}
				}
				if(Place.second) retourn=true;
				else //Si les deux voisins sont vide alors on retourne
				{   if(Place.first.Tete==9 && Place.first.Queue==9 && NumCoups!=1) retourn=true;
				else
				{//Si il ya une valuer attendu on verifie si on peut poser
					if(Place.first.Tete!=9 && Place.first.Tete!=10)
						if(Place.first.Tete!=-1)
							if(Place.first.Tete!=T && Place.first.Tete!=Q) retourn=true;
							else //Si elle est bloquée alors on remet la piece
								if(Place.first.Tete==10) retourn=true;

					if(!retourn)
					{
						if(Place.first.Queue!=9 && Place.first.Tete!=10)
							if(Place.first.Queue!=-1)
								if(Place.first.Queue!=T && Place.first.Queue!=Q) retourn=true;
								else if(Place.first.Queue==10) retourn=true;
								if(!retourn)
									if(Place.first.Tete==Q || Place.first.Queue==T) Flip=true;
					}
				}
				}
			}
		}//Else

		//Si la piece n'est pas dans une place convenable on al fait retourner
		if(retourn) mCurrentObject->setPosition(PrePos);
		else //Si tous va bien alors on la place
		{
			CanPlay=true;
			CantPlay=0;
			CanHMove=false;
			float Alpha =0.389*Pos; //Langle de rotation de la piece
			Ogre::Radian Rot;
			//On remet tous ?l'etat initial puis on applique les rotations
			mCurrentObject->resetToInitialState(); 
			if(Flip) mCurrentObject->yaw(Ogre::Degree(180));
			mCurrentObject->pitch(Ogre::Degree(-90));
			mCurrentObject->scale(Ogre::Vector3(8,8,8));
			mCurrentObject->setPosition(Nearest); 
			mCurrentObject->getAttachedObject(0)->setQueryFlags(CANTMOVE);
			//Si elle est dans les poisitions Zinga/Double
			if(Pos==0 || Pos==8) mCurrentObject->roll(Ogre::Degree(180));
			else
				if(Pos==4 || Pos==12) mCurrentObject->roll(Ogre::Degree(90));
				else//Sinon s'il elle est dans une autre place
					if(Pos<9)
					{
						Rot=-atan(cos(Alpha)/abs(sin(Alpha)));
						mCurrentObject->roll(Rot);
					}
					else {	
						Rot=-atan(-cos(Alpha)/abs(sin(Alpha)));
						mCurrentObject->roll(Rot);
					};

					//On suprime la piece de la main du joueur
					std::vector<Piece> Piec =Players.front().GetPiece();
					std::vector<Piece>::iterator Pitr;
					int k=0;
					for(Pitr=Piec.begin();Pitr!=Piec.end();Pitr++)
					{
						if(Pitr->Couleur==col)
						{
							if(Pitr->Tete==T && Pitr->Queue==Q)
							{
								Players.front().SuprimerPiece(k);
								break;
							}
						}
						k++;
					}

					//On ajoute la piece ?la liste correspondante
					std::pair<Piece,bool> Tail;
					Tail.second =true;
					if(Flip)
					{
						Tail.first.Queue = T;
						Tail.first.Tete =Q;
					}
					else
					{
						Tail.first.Queue = Q;
						Tail.first.Tete =T;
					}

					int col;

					switch(N[j-1])
					{
					case 'J':
						col=1;
						Updateboard(&Tail,Pos,Color::Jaune,&PiecesJ,&PiecesV, &PiecesVI);				 												
						break;						
					case 'V':
						col=0;
						Updateboard(&Tail,Pos,Color::Vert,&PiecesV,&PiecesJ, &PiecesO);
						break;
					case 'I':
						col=2;
						Updateboard(&Tail,Pos,Color::Violet,&PiecesVI,&PiecesJ, &PiecesO);
						break;
					case 'O':
						col=3;
						Updateboard(&Tail,Pos,Color::Orange,&PiecesO,&PiecesV, &PiecesVI);						
					}

					//On verifie si on a pas fermer le cercle
					std::vector<std::vector<std::pair<Piece,int>>> PlacesVides;
					PlacesVides.resize(4);
					std::pair<Piece,int> Pos;
					int NbBloced[4];
					for(int j=0;j<4;j++) NbBloced[j]=0;
					for(int n=0;n<16;n++)
					{
						if(!PiecesV[n].second)
						{
							if((PiecesV[n].first.Tete!=9 && PiecesV[n].first.Tete!=10)||(PiecesV[n].first.Queue!=9 && PiecesV[n].first.Queue!=10))
							{
								Pos.first = PiecesV[n].first;
								Pos.first.Couleur = Color::Vert;
								Pos.second = n;
								PlacesVides[0].push_back(Pos);
							}
							else
							{
								if(PiecesV[n].first.Tete==10 && PiecesV[n].first.Queue==10) NbBloced[0]++;
							}
						}
						if(!PiecesJ[n].second)
						{
							if((PiecesJ[n].first.Tete!=9 && PiecesJ[n].first.Tete!=10)||(PiecesJ[n].first.Queue!=9 && PiecesJ[n].first.Queue!=10))
							{
								Pos.first = PiecesJ[n].first;
								Pos.first.Couleur = Color::Jaune;
								Pos.second = n;
								PlacesVides[1].push_back(Pos);
							}
							else
							{
								if(PiecesJ[n].first.Tete==10 && PiecesJ[n].first.Queue==10) NbBloced[1]++;
							}
						}

						if(!PiecesVI[n].second)
						{
							if((PiecesVI[n].first.Tete!=9 && PiecesVI[n].first.Tete!=10)||(PiecesVI[n].first.Queue!=9 && PiecesVI[n].first.Queue!=10))
							{
								Pos.first = PiecesVI[n].first;
								Pos.first.Couleur = Color::Violet;
								Pos.second = n;
								PlacesVides[2].push_back(Pos);
							}
							else
							{
								if(PiecesVI[n].first.Tete==10 && PiecesVI[n].first.Queue==10) NbBloced[2]++;
							}
						}

						if(!PiecesO[n].second)
							if((PiecesO[n].first.Tete!=9 && PiecesO[n].first.Tete!=10)||(PiecesO[n].first.Queue!=9 && PiecesO[n].first.Queue!=10))
							{
								Pos.first = PiecesO[n].first;
								Pos.first.Couleur = Color::Orange;
								Pos.second = n;
								PlacesVides[3].push_back(Pos);
							}
							else
							{
								if(PiecesO[n].first.Tete==10 && PiecesO[n].first.Queue==10) NbBloced[3]++;
							}
					}
					if(((int)PlacesVides[col].size()==0)&&(NbBloced[col]==0))
					{
						Players[0].SetScore(Players[0].GetScore()+16);
						
						OgreBites::TextBox* ZoneScore = (OgreBites::TextBox*) mTrayMgr->getWidget("Inist");
						//CEGUI::StaticText* ZoneTour =(CEGUI::StaticText*)CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"Inist");
						ZoneScore->setText(boost::lexical_cast<std::string>(Players[0].GetScore()));
					}
					//On decale les autre piéces 
					InfoPlayers inf=Infos[0];
					for(int u=PosPiece+1;u<(int)inf.NoeudPiece.size();u++)
					{
						inf.NoeudPiece[u]->setPosition(PrePos);
						PrePos+=Ogre::Vector3(80,0,0);
					}
					//Et on suprime ce noeud des infos du joueur
					std::vector<Ogre::SceneNode*>::iterator itr;
					int i=0;
					for(itr=inf.NoeudPiece.begin();itr!=inf.NoeudPiece.end();itr++)
					{
						if(i==PosPiece)
						{
							inf.NoeudPiece.erase(itr);
							break;
						}
						i++;
					}
					Infos[0]=inf; //On met ?jour les infos ces infos
					//Et on passe la main au joueur suivant
					if((int)Players.size()==4) turn=3;
					else turn=1;
					if(turn==Premier) NumCoups++;
					OgreBites::TextBox* ZoneTour = (OgreBites::TextBox*) mTrayMgr->getWidget("Tour");
					//CEGUI::StaticText* ZoneTour =(CEGUI::StaticText*)CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"Tour");
					ZoneTour->setText("Tour de P"+boost::lexical_cast<std::string>(turn));
		}//FIN SI TOUS va bein
	}
	//CEGUI::System::getSingleton().injectMouseButtonUp(B);
	return true;
};

void GameFrameListener::GetLandingPosition(std::vector<std::pair<Piece,bool>>* dominoSet, Ogre::Vector3* Nearest,int* Pos,bool* retourn, std::vector<Ogre::Vector3>* nodesPos, std::pair<Piece,bool>* Place){
					std::vector<Ogre::Vector3>::iterator itr;
					Ogre::Real MinDist=400,D;
					int Num=0;
					for(itr=nodesPos->begin();itr!=nodesPos->end();itr++)
					{
						Ogre::Vector3 V=*(itr)-mCurrentObject->getPosition();
						D = V.normalise(); //On calcul la distance
						if(D< MinDist)
						{
							MinDist =D;
							(*Nearest)=*(itr);
							(*Pos)=Num;
						}
						Num++;
					}
					//Si On est loin du cercle alors la piece sera remi a ca position
					if(MinDist>100) (*retourn) =true;
					else if(NumCoups==1 && (*Pos)%4!=0) (*retourn)=true;
					else (*Place) =(*dominoSet)[(*Pos)];
};

void GameFrameListener::Updateboard(std::pair<Piece,bool>* Tail,int Pos,Color col,std::vector<std::pair<Piece,bool>>* dominosSet,
				 std::vector<std::pair<Piece,bool>>* neighbor1,std::vector<std::pair<Piece,bool>>* neighbor2)
{
				Tail->first.Couleur =col;
						(*dominosSet)[Pos]=(*Tail); 
						int x;
						(Pos==0)?(x=15):(x=Pos-1);
						if(Pos<=8)
						{
							//Pour la nise-a-jour du voisin en Haut
							if(Pos!=0)
							{
								if(!(*dominosSet)[x].second)
								{
									if((*dominosSet)[x].first.Queue==9)(*dominosSet)[x].first.Queue=Tail->first.Tete;
									else if((*dominosSet)[x].first.Queue!=Tail->first.Tete)
									{(*dominosSet)[x].first.Queue=10;
									(*dominosSet)[x].first.Tete=10;}
								}else
								{
									if((*dominosSet)[x].first.Queue==-1)//Si c'est une Zinga 
									{
										(*dominosSet)[x].first.Queue = Tail->first.Tete;
										(*dominosSet)[x].first.Tete =Tail->first.Tete;
										if(x==0) (*dominosSet)[15].first.Tete = Tail->first.Tete;
										else (*dominosSet)[x-1].first.Queue= Tail->first.Tete;
										if(x%4==0) Pos=x;
										else if((x-1)%4==0) Pos=(x-1)%16;
									}
								}
							}else
								if(!(*dominosSet)[x].second) (*dominosSet)[x].first.Tete=Tail->first.Tete;
								else
								{
									if((*dominosSet)[15].first.Tete==-1)
									{
										(*dominosSet)[15].first.Tete = Tail->first.Tete;
										(*dominosSet)[15].first.Queue =Tail->first.Tete;
										(*dominosSet)[14].first.Tete =Tail->first.Tete;
									}
								}
								//Pour la MAJ du voisin du Bas
								if(Pos!=8)
								{if(!(*dominosSet)[Pos+1].second)
								{
									if((*dominosSet)[Pos+1].first.Tete==9)(*dominosSet)[Pos+1].first.Tete=Tail->first.Queue;
									else if((*dominosSet)[Pos+1].first.Tete!=Tail->first.Queue)
									{(*dominosSet)[Pos+1].first.Tete=10;
									(*dominosSet)[Pos+1].first.Queue=10;}
								}
								else
								{
									if((*dominosSet)[Pos+1].first.Tete==-1)
									{
										(*dominosSet)[Pos+1].first.Tete = Tail->first.Queue;
										(*dominosSet)[Pos+1].first.Queue =Tail->first.Queue;
										if(Pos+1==8) (*dominosSet)[9].first.Queue= Tail->first.Queue;
										else (*dominosSet)[Pos+2].first.Queue = Tail->first.Queue;
										if((Pos+1)%4==0) Pos =Pos+1;
										else if((Pos+2)%4==0) Pos=Pos+2;
									}
								}

								}
								else
									if(!(*dominosSet)[9].second)(*dominosSet)[9].first.Queue=Tail->first.Queue;
									else
									{
										if((*dominosSet)[9].first.Queue==-1)
										{
											(*dominosSet)[9].first.Tete= Tail->first.Queue;
											(*dominosSet)[9].first.Queue =Tail->first.Queue;
											(*dominosSet)[10].first.Queue  =Tail->first.Queue;
										}
									}
						}
						else
						{
							//MAJ voisin du Bas
							if(!(*dominosSet)[x].second)
							{
								if((*dominosSet)[x].first.Tete==9)(*dominosSet)[x].first.Tete=Tail->first.Queue;
								else if((*dominosSet)[x].first.Tete!=Tail->first.Queue)
								{(*dominosSet)[x].first.Tete=10;
								(*dominosSet)[x].first.Queue=10;}
							}
							else
							{
								if((*dominosSet)[x].first.Tete==-1)//Si c'est une Zinga 
								{
									(*dominosSet)[x].first.Queue = Tail->first.Queue;
									(*dominosSet)[x].first.Tete =Tail->first.Queue;
									if(x==8) (*dominosSet)[7].first.Queue = Tail->first.Queue;
									else (*dominosSet)[x-1].first.Queue= Tail->first.Queue;
									if(x%4==0) Pos=x;
									if((x-1)%4==0) Pos=(x-1)%4;
								}
							}
							//MAJ voisin du Haut
							if(!(*dominosSet)[(Pos+1)%16].second)
							{
								if((*dominosSet)[(Pos+1)%16].first.Queue==9)(*dominosSet)[(Pos+1)%16].first.Queue=Tail->first.Tete;
								else if((*dominosSet)[(Pos+1)%16].first.Queue!=Tail->first.Tete)
								{(*dominosSet)[(Pos+1)%16].first.Tete=10;
								(*dominosSet)[(Pos+1)%16].first.Queue=10;}
							}
							else
							{
								if((*dominosSet)[(Pos+1)%16].first.Queue==-1)//Si c'est une Zinga 
								{
									(*dominosSet)[(Pos+1)%16].first.Queue = Tail->first.Tete;
									(*dominosSet)[(Pos+1)%16].first.Tete =Tail->first.Tete;
									if((Pos+1)%16==0)(*dominosSet)[1].first.Tete = Tail->first.Tete;
									else (*dominosSet)[(Pos+2)%16].first.Queue= Tail->first.Tete;
									if((Pos+1)%4==0) Pos =(Pos+1)%16;
									if((Pos+2)%4==0) Pos =(Pos+2)%16;
								}
							}
						}

						//Puis on met ?jour les cercles voisins
						if(Pos==4)
						{
							if(!(*neighbor1)[12].second)
								if((*neighbor1)[12].first.Tete==9){ (*neighbor1)[12].first.Tete=Tail->first.Queue;
							(*neighbor1)[12].first.Queue=Tail->first.Queue;}
								else if((*neighbor1)[12].first.Tete!=Tail->first.Queue)
								{(*neighbor1)[12].first.Tete=10;
							(*neighbor1)[12].first.Queue=10;
							}
						}
						else
							if(Pos==8)
							{
								if(!(*neighbor2)[0].second)
									if((*neighbor2)[0].first.Tete==9){ (*neighbor2)[0].first.Tete=Tail->first.Queue;
								(*neighbor2)[0].first.Queue=Tail->first.Queue;}
									else if((*neighbor2)[0].first.Tete!=Tail->first.Queue)
									{(*neighbor2)[0].first.Tete=10;
								(*neighbor2)[0].first.Queue=10;
								}
							}						
}

void GameFrameListener::PoserPiece(Piece Tail,int Pos,int NumJoueur)
{
	InfoPlayers inf= Infos[NumJoueur];
	std::vector<Ogre::SceneNode*>::iterator itr;
	int PosP=0;//Contient la position de la piece dans la main du joueur
	//On cherche la position de la piece dans la Main du Joeur
	bool Yes=false;
	for(itr=inf.NoeudPiece.begin();itr!=inf.NoeudPiece.end();itr++)
	{
		Ogre::String N=(*itr)->getAttachedObject(0)->getName();
		int T,Q,j;
		j=(int)N.size()-1;
		if(N.substr(0,1)=="Z")
		{
			T=-1;
			Q=-1;
		}
		else
		{
			T= Ogre::StringConverter::parseUnsignedInt(N.substr(0,1));//Tete de la piece
			Q= Ogre::StringConverter::parseUnsignedInt(N.substr(1,1));//Queue de la piece
		}

		if((T==Tail.Tete && Q==Tail.Queue)||(T==Tail.Queue && Q==Tail.Tete)) {
			Ogre::String K;
			switch(Tail.Couleur)
			{
			case Color::Vert:
				K="V";
				break;
			case Color::Jaune:
				K="J";
				break;
			case Color::Violet:
				K="I";
				break;
			case Color::Orange:
				K="O";
				break;
			}
			if(K[0]==N[j])
			{
				ToMove=*itr;
				inf.NoeudPiece.erase(itr);
				Yes=true;
				break;
			}
		}
		PosP++;
	}
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Ogre::Vector3 EndPos;
	switch(Tail.Couleur)
	{
	case Color::Vert:
		EndPos=NoeudV[Pos];
		break;
	case Color::Jaune:
		EndPos=NoeudJ[Pos];
		break;
	case Color::Violet:
		EndPos=NoeudVI[Pos];
		break;
	case Color::Orange:
		EndPos=NoeudO[Pos];
		break;
	}


	//==================== 
	if(Yes)
	{
		Ogre::Vector3 MovePos=ToMove->getPosition();
		Ogre::Vector3 Direction =EndPos - ToMove->getPosition(); //Le vecteur de Direction
		//On crée une animation
		Ogre::Animation* Anim=GestScene->createAnimation(ToMove->getAttachedObject(0)->getName() +"Move",2); 
		Anim->setInterpolationMode(Ogre::Animation::IM_LINEAR);
		//On crée des Track de l'animation
		Ogre::Quaternion Tourne(-sqrt(0.5),sqrt(0.5),0,0);
		Ogre::NodeAnimationTrack *Track = Anim->createNodeTrack(0,ToMove);
		Ogre::TransformKeyFrame  *TFrame =Track->createNodeKeyFrame(0);
		TFrame->setTranslate(MovePos);
		TFrame = Track->createNodeKeyFrame(2);
		TFrame->setRotation(Tourne);
		TFrame->setTranslate(EndPos);
		TFrame->setScale(Ogre::Vector3(8,8,8));
		Ogre::AnimationState *AnimStat=GestScene->createAnimationState(ToMove->getAttachedObject(0)->getName()+"Move");
		AnimStat->setEnabled(true);
		AnimStat->setLoop(false);
		AddAnimStat(AnimStat);

		//On decale les autre pieces
		Ogre::Vector3 Shift;
		if(turn<2) Shift=Ogre::Vector3(80,0,0);
		else
			Shift=Ogre::Vector3(0,0,80);

		for(int u=PosP+1;u<(int)inf.NoeudPiece.size();u++)
		{
			inf.NoeudPiece[u]->setPosition(MovePos);
			MovePos+=Shift;
		}

		Infos[NumJoueur]=inf;
	}
	else
	{
		CanPlay=true;
		if(turn==0 &&Players.size()==4) turn=3;
		else
			if(turn==0) turn=1;
			else
				if(turn==1)turn=(turn+1)%(int)Players.size();
				else if(turn==2) turn=0;
				else if(turn==3) turn=1;
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
};//End PoserPiece


void GameFrameListener::Selection(void)
{
	try{
	//using namespace CEGUI;
	std::vector<Player>::iterator iter;
	//Tanqu'il y a egalite ont refet le tirage
	int nbPlayer = (int)Players.size();
	OgreBites::TextBox* ZoneTour = (OgreBites::TextBox*) mTrayMgr->getWidget("Tour");
	//StaticText* ZoneTour =(StaticText*) WindowManager::getSingleton().getWindow((utf8*)"Tour");
	ZoneTour->show();

	int MaxSum=0; //Represente la somme minimum des pieces

	ZoneTour->setText(Players.front().GetNom()+" Tire une Piece");
	tour=0;
	Ogre::Vector3 MinCorner(-80*nbPlayer,30,60);
	//La liste des positions
	std::list<Ogre::Vector3> Positions;
	Positions.push_back(Ogre::Vector3(-200,30,-1000));
	Positions.push_back(Ogre::Vector3(-200,30,1000));
	Positions.push_back(Ogre::Vector3(-1000,30,-200));
	Positions.push_back(Ogre::Vector3(1000,30,-200));
	Ogre::Vector3 StartPos;

	Jeux.Choisir(); //Chaque joueur choisit une piece
	Players = Jeux.GetPlayers(); //On met ?jours la liste des joueurs

	Ogre::SceneNode *node;
	Ogre::Entity *ent;
	Ogre::String fileName;
	Ogre::String MeshName="Domino0";
	Ogre::String SceneName="Scene0";
	std::list<Ogre::SceneNode*>::iterator Snode;
	Ogre::SceneNode *no;
	for(Snode =Dominos.begin();Snode!=Dominos.end();Snode++)
	{
		no=*Snode;
		GestScene->destroyEntity(no->getAttachedObject(0)->getName());
		GestScene->destroySceneNode(no->getName());
	}
	Dominos.clear();
	equal=false;
	for(iter=Players.begin();iter!=Players.end();iter++)
	{
		StartPos =Positions.front();
		Positions.pop_front();
		std::vector<Piece> P=(*iter).GetPiece();	
		Piece Taill;
		Taill.Couleur =(*P.begin()).Couleur;
		Taill.Queue =(*P.begin()).Queue;
		Taill.Tete =(*P.begin()).Tete;

		if(Taill.Tete <Taill.Queue) fileName=boost::lexical_cast<std::string>(Taill.Tete)+boost::lexical_cast<std::string>(Taill.Queue);
		else
			if(Taill.Tete!=-1)fileName=boost::lexical_cast<std::string>(Taill.Queue)+boost::lexical_cast<std::string>(Taill.Tete);
			else fileName="Zinga";
			switch(Taill.Couleur)
			{
			case Color::Jaune: fileName+="J";
				break;
			case Color::Orange: fileName+="O";
				break;
			case Color::Vert: fileName+="V";
				break;
			case Color::Violet : fileName+="VI";
				break;
			};
			fileName+=".mesh";
			
			
			//ent=GestScene->createEntity(MeshName+boost::lexical_cast<std::string>((unsigned long)(NbRepeat)),fileName);
			MeshName+= boost::lexical_cast<std::string>(NbRepeat);
			ent=GestScene->createEntity(MeshName,fileName);
			SceneName+= boost::lexical_cast<std::string>(NbRepeat);
			node =GestScene->getRootSceneNode()->createChildSceneNode(SceneName,StartPos);
			node->attachObject(ent);
			node->scale(Ogre::Vector3(10,10,10));
			Dominos.push_back(node);
			Animer(node,StartPos,MinCorner);
			MeshName[6]++;
			SceneName[5]++;
			MinCorner=MinCorner+Ogre::Vector3(100,0,0);
	}
	MaxSum=0;
	Premier=0;
	int nbegale=0;//Indique le monbre de joueur qui ont la valeur Min
	for(iter=Players.begin();iter!=Players.end();iter++)
	{
		std::vector<Piece> P =static_cast<Player>(*iter).GetPiece();
		Piece piec;
		piec.Couleur =(*P.begin()).Couleur;
		piec.Queue = (*P.begin()).Queue;
		piec.Tete =(*P.begin()).Tete;

		if(MaxSum<piec.Queue+piec.Tete)
		{
			MaxSum=piec.Queue+piec.Tete;
			Premier = static_cast<Player>(*iter).GetNum();
			nbegale=1;
		}
		else
		{
			if(MaxSum==(piec.Queue+piec.Tete)) nbegale++;
		}

	}
	if(nbegale!=1) equal=true;
	std::string NameH;
	if(Premier>0)
		NameH="P"+boost::lexical_cast<std::string>(Premier);
	else
	{
		Player J=Players.front();
		NameH=J.GetNom();
	};
	NameFirst=NameH;
	Etat=0;
	}
	catch(Ogre::Exception &e){
		Ogre::LogManager::getSingletonPtr()->logMessage("GameFrameListener Error: " + e.getFullDescription());				
	}
};

void GameFrameListener::MetreAjourTable(std::pair<Piece,int> Ajouer)
{
	int Place=Ajouer.second;
	OgreBites::TextBox* ZoneTour = (OgreBites::TextBox*) mTrayMgr->getWidget("Tour");
	//CEGUI::StaticText* ZoneTour =(CEGUI::StaticText*)CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"Tour");
	ZoneTour->setText("Je pose la piece dans la position "+boost::lexical_cast<std::string>(Place));
	inv=false;
	switch(Ajouer.first.Couleur)
	{
	case Color::Vert:
		UpdatePieces(Ajouer,&PiecesV);		
		break;
	case Color::Jaune:
		UpdatePieces(Ajouer,&PiecesJ);
		break;
	case Color::Violet:
		UpdatePieces(Ajouer,&PiecesVI);
		break;

	case Color::Orange:
		UpdatePieces(Ajouer,&PiecesO);
		break;		
	}

	std::vector<std::vector<std::pair<Piece,int>>> PlacesVides;
	PlacesVides.resize(4);
	std::pair<Piece,int> Pos;
	int NbBloced[4];
	for(int j=0;j<4;j++) NbBloced[j]=0;
	for(int n=0;n<16;n++)
	{
		if(!PiecesV[n].second)
		{
			if((PiecesV[n].first.Tete!=9 && PiecesV[n].first.Tete!=10)||(PiecesV[n].first.Queue!=9 && PiecesV[n].first.Queue!=10))
			{
				Pos.first = PiecesV[n].first;
				Pos.first.Couleur = Color::Vert;
				Pos.second = n;
				PlacesVides[0].push_back(Pos);
			}
			else
			{
				if(PiecesV[n].first.Tete==10 && PiecesV[n].first.Queue==10) NbBloced[0]++;
			}
		}
		if(!PiecesJ[n].second)
		{
			if((PiecesJ[n].first.Tete!=9 && PiecesJ[n].first.Tete!=10)||(PiecesJ[n].first.Queue!=9 && PiecesJ[n].first.Queue!=10))
			{
				Pos.first = PiecesJ[n].first;
				Pos.first.Couleur = Color::Jaune;
				Pos.second = n;
				PlacesVides[1].push_back(Pos);
			}
			else
			{
				if(PiecesJ[n].first.Tete==10 && PiecesJ[n].first.Queue==10) NbBloced[1]++;
			}
		}

		if(!PiecesVI[n].second)
		{
			if((PiecesVI[n].first.Tete!=9 && PiecesVI[n].first.Tete!=10)||(PiecesVI[n].first.Queue!=9 && PiecesVI[n].first.Queue!=10))
			{
				Pos.first = PiecesVI[n].first;
				Pos.first.Couleur = Color::Violet;
				Pos.second = n;
				PlacesVides[2].push_back(Pos);
			}
			else
			{
				if(PiecesVI[n].first.Tete==10 && PiecesVI[n].first.Queue==10) NbBloced[2]++;
			}
		}

		if(!PiecesO[n].second)
			if((PiecesO[n].first.Tete!=9 && PiecesO[n].first.Tete!=10)||(PiecesO[n].first.Queue!=9 && PiecesO[n].first.Queue!=10))
			{
				Pos.first = PiecesO[n].first;
				Pos.first.Couleur = Color::Orange;
				Pos.second = n;
				PlacesVides[3].push_back(Pos);
			}
			else
			{
				if(PiecesO[n].first.Tete==10 && PiecesO[n].first.Queue==10) NbBloced[3]++;
			}
	}
	//Si il a fermer un cercle alors on lui ajoute 16 pts
	if(((int)PlacesVides[Ajouer.first.Couleur].size()==0)&&(NbBloced[Ajouer.first.Couleur]==0))
	{
		Players[tour].SetScore(Players[tour].GetScore()+16);
		std::string num="P0";
		num[1]+=tour;
		OgreBites::TextBox* ZoneTour = (OgreBites::TextBox*) mTrayMgr->getWidget(num);
		//CEGUI::StaticText* ZoneTour =(CEGUI::StaticText*)CEGUI::WindowManager::getSingleton().getWindow(num);
		ZoneTour->setText(boost::lexical_cast<std::string>(Players[tour].GetScore()));
	}
};


void GameFrameListener::UpdatePieces(std::pair<Piece,int> Ajouer, std::vector<std::pair<Piece,bool>>* dominos ){

	if(Ajouer.first.Tete==-1)//Si c'est une Zinga
	{				
		if((*dominos)[Place].first.Tete!=9)
		{
			Ajouer.first.Tete=(*dominos)[Place].first.Tete;
			Ajouer.first.Queue=Ajouer.first.Tete;
		}
		else
			if((*dominos)[Place].first.Queue!=9)
			{Ajouer.first.Tete=(*dominos)[Place].first.Queue;
		Ajouer.first.Queue=Ajouer.first.Tete;
		}
	}
	else//Si ce n'est pas une Zinga alors on verifie si on inverse ou non 
	{
		if((*dominos)[Place].first.Tete==Ajouer.first.Queue ||(*dominos)[Place].first.Queue==Ajouer.first.Tete)//On inverse
		{
			int c=Ajouer.first.Queue;
			Ajouer.first.Queue=Ajouer.first.Tete;
			Ajouer.first.Tete=c;
			inv=true;
		}
	}
	if(Place<8)
	{
		if(Place==0)
		{if(!(*dominos)[15].second)(*dominos)[15].first.Tete=Ajouer.first.Tete;
		else
			if((*dominos)[15].first.Tete==-1) //Si la pieces ?cot?est un Zinga 
			{
				(*dominos)[15].first.Tete=Ajouer.first.Tete;
				(*dominos)[15].first.Queue =Ajouer.first.Queue;
				(*dominos)[14].first.Tete =Ajouer.first.Tete;
			}

			if(!(*dominos)[1].second) (*dominos)[1].first.Tete=Ajouer.first.Tete;
			else
				if((*dominos)[1].first.Tete==-1)
				{
					(*dominos)[1].first.Tete =Ajouer.first.Tete;
					(*dominos)[1].first.Queue =Ajouer.first.Tete;
					(*dominos)[2].first.Tete =Ajouer.first.Tete;
				}
		}
		else
		{
			if(!(*dominos)[Place-1].second) (*dominos)[Place-1].first.Queue=Ajouer.first.Tete;
			else
				if((*dominos)[Place-1].first.Queue==-1)
				{
					(*dominos)[Place-1].first.Queue=Ajouer.first.Tete;
					(*dominos)[Place-1].first.Tete =Ajouer.first.Tete;
					if(Place-1==0){
						(*dominos)[15].first.Tete=Ajouer.first.Tete;
					}
					else (*dominos)[Place-2].first.Queue =Ajouer.first.Tete;
					if((Place-1)%4==0) Place=Place-1;
					if((Place-2)%4==0) Place=Place-2;
					Ajouer.first.Queue =Ajouer.first.Tete;
				}
				if(!(*dominos)[(Place+1)%16].second)(*dominos)[(Place+1)%16].first.Tete=Ajouer.first.Queue;
				else
					if((*dominos)[(Place+1)%16].first.Tete==-1)
					{
						(*dominos)[(Place+1)%16].first.Queue=Ajouer.first.Queue;
						(*dominos)[(Place+1)%16].first.Tete =Ajouer.first.Queue;
						if(Place+1==8) (*dominos)[9].first.Queue=Ajouer.first.Queue;
						else (*dominos)[Place+2].first.Tete =Ajouer.first.Queue;
						if((Place+1)%4==0) Place=Place+1;
						if((Place+2)%4==0) Place=Place+2;
						Ajouer.first.Queue =Ajouer.first.Tete;
					}
		}
	}
	else
	{
		if(Place==8)
		{if(!(*dominos)[7].second) (*dominos)[7].first.Queue=Ajouer.first.Tete;
		else
			if((*dominos)[7].first.Queue==-1)
			{
				(*dominos)[7].first.Queue =Ajouer.first.Tete;
				(*dominos)[7].first.Tete =Ajouer.first.Queue;
				(*dominos)[6].first.Queue =Ajouer.first.Tete;
			}
			if(!(*dominos)[9].second) (*dominos)[9].first.Queue=Ajouer.first.Tete;
			else
				if((*dominos)[9].first.Queue==-1)
				{
					(*dominos)[9].first.Queue =Ajouer.first.Tete;
					(*dominos)[9].first.Tete =Ajouer.first.Queue;
					(*dominos)[10].first.Queue =Ajouer.first.Tete;
				}
		}
		else
		{
			if(!(*dominos)[Place-1].second)(*dominos)[Place-1].first.Tete=Ajouer.first.Queue;
			else
				if((*dominos)[Place-1].first.Tete==-1)
				{
					(*dominos)[Place-1].first.Queue=Ajouer.first.Queue;
					(*dominos)[Place-1].first.Tete =Ajouer.first.Queue;
					if(Place-1==8){
						(*dominos)[7].first.Queue=Ajouer.first.Queue;
					}
					else (*dominos)[Place-2].first.Tete =Ajouer.first.Queue;
					if((Place-1)%4==0) Place=Place-1;
					if((Place-2)%4==0) Place=Place-2;
					Ajouer.first.Tete =Ajouer.first.Queue;
				}
				if(!(*dominos)[(Place+1)%16].second)(*dominos)[(Place+1)%16].first.Queue=Ajouer.first.Tete;
				else
					if((*dominos)[(Place+1)%16].first.Queue==-1)
					{
						(*dominos)[(Place+1)%16].first.Queue=Ajouer.first.Tete;
						(*dominos)[(Place+1)%16].first.Tete=Ajouer.first.Tete;
						if(Place+1==16)
						{
							(*dominos)[1].first.Tete =Ajouer.first.Tete;
						}
						else
							(*dominos)[(Place+2)%16].first.Queue= Ajouer.first.Tete;
						if((Place+1)%4==0) Place=(Place+1)%16;
						if((Place+2)%4==0) Place=(Place+2)%16;
						Ajouer.first.Tete =Ajouer.first.Queue;
					}
		}
	}


	//On MET ?jour les cases es autre case
	if(Place==12)
	{
		if(!PiecesJ[4].second)
			if(PiecesJ[4].first.Tete==9)
			{ PiecesJ[4].first.Tete=Ajouer.first.Queue;
		PiecesJ[4].first.Queue=Ajouer.first.Queue;}
			else if(PiecesJ[4].first.Tete!=Ajouer.first.Queue)
			{PiecesJ[4].first.Tete=10;
		PiecesJ[4].first.Queue=10;
		}
	}
	else
		if(Place==8)
		{
			if(!PiecesO[0].second)
				if(PiecesO[0].first.Tete==9)
				{ PiecesO[0].first.Tete=Ajouer.first.Queue;
			PiecesO[0].first.Queue=Ajouer.first.Queue;}
				else if(PiecesO[0].first.Tete!=Ajouer.first.Queue)
				{PiecesO[0].first.Tete=10;
			PiecesO[0].first.Queue=10;
			}
		}
		(*dominos)[Place].first=Ajouer.first;
		(*dominos)[Place].second=true;
}


void GameFrameListener::Animer(Ogre::SceneNode*  Noeud,Ogre::Vector3 StartPos,Ogre::Vector3 EndPos)
{
		Ogre::Vector3 Direction = EndPos - StartPos; //Le vecteur de Direction
		Ogre::Real Dist =Direction.normalise();
		Ogre::Real Deplacement = Dist/2;
		Ogre::Real TrackPos=2;

		//On crée une animation
		Ogre::Animation* Anim=GestScene->createAnimation(Noeud->getName() +"Move",4); 
		Anim->setInterpolationMode(Ogre::Animation::IM_SPLINE);
		//On crée des Track de l'animation
		Ogre::Quaternion Rot(-sqrt(0.5),sqrt(0.5),0,0);
		Ogre::NodeAnimationTrack *Track = Anim->createNodeTrack(0,Noeud);
		Ogre::TransformKeyFrame  *TFrame =Track->createNodeKeyFrame(0);
		TFrame->setTranslate(StartPos);
		while(Dist>0.0f)
		{
			TFrame = Track->createNodeKeyFrame(TrackPos);
			StartPos =StartPos+Direction;
			TFrame->setTranslate(StartPos);
			TFrame->setScale(Ogre::Vector3(10,10,10));
			TrackPos+=TrackPos;
			Dist-=Deplacement;
		};
		TFrame->setRotation(Rot);
		TFrame->setTranslate(EndPos);
		TFrame->setScale(Ogre::Vector3(18,18,18));
		Ogre::AnimationState *AnimStat=GestScene->createAnimationState(Noeud->getName()+"Move");
		AnimStat->setEnabled(true);
		AnimStat->setLoop(false);
		AddAnimStat(AnimStat);
};

void GameFrameListener::AfficherScore(void)
	{
		OgreBites::TextBox* ZoneNom = (OgreBites::TextBox*) mTrayMgr->getWidget("Inist");
		//CEGUI::StaticText* ZoneNom =(CEGUI::StaticText*) CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"Inist");
		Player J= Players[0];

		ZoneNom->setText(J.GetNom() +"   "+ boost::lexical_cast<std::string>(J.GetScore()));
		std::string num="P1";
		for(int i=1;i<(int)Players.size();i++)
		{
			J =Players[i];
			OgreBites::TextBox* ZoneNom = (OgreBites::TextBox*) mTrayMgr->getWidget(num);
			//ZoneNom =(CEGUI::StaticText*) CEGUI::WindowManager::getSingleton().getWindow(num);
			ZoneNom->setText(num+"    "+boost::lexical_cast<std::string>(J.GetScore()));
			ZoneNom->show();
			//ZoneNom->setFrameEnabled(true);
			//ZoneNom->setBackgroundEnabled(true);
			num[1]++;
		}
	}


GameFrameListener::~GameFrameListener(void)
{
}
