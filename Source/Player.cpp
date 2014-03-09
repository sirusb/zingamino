#include "Player.h"


Player::Player(void)
{
}

Player::Player(std::string Nom,int num)
{
	mName=Nom;
	Score=0;
	NbPieces=0;
	mPieces.clear();
	Num=num;
}

std::vector<Piece> Player::GetPiece(void){
		return mPieces;
	};



int Player::GetNum(){
		return Num;
};

std::pair<Piece,int> Player::Joue(bool DebutR,std::vector<std::pair<Piece,bool>> PV,std::vector<std::pair<Piece,bool>> PJ,std::vector<std::pair<Piece,bool>> PVI,
	                         std::vector<std::pair<Piece,bool>> PO,std::vector<Piece> Pochette)
{
	//Si c'est le Debut de la round alors on place un double ou un Zinga
	if(DebutR)
		return Opening(PV,PJ,PVI,PO,Pochette);
	else
	{
		std::pair<Piece,int> ret;
		ret.second=-3;
		while(ret.second==-3)
		{
			ret=Think(PV,PJ,PVI,PO,Pochette);
			if(ret.second==-3)
			{
				if(Pochette.size()>0)
				{

					Piece P= Pochette.front(); //On prend une piece de la pochette
					Pochette.erase(Pochette.begin());
					mPieces.push_back(P); //Et on l'ajoute ?la main du joueur					
					NbPieces++;
				}else break;
			}
		}
		return ret;

	}

};



void Player::SetScore(int Valeur)
{
	Score=Valeur;
}

int Player::GetScore(void)
{
	return Score;
}

//Obtenir le nom du joueur
std::string Player::GetNom(void)
{
	return mName;
}



void Player::AddPiece(Piece P)
{
	mPieces.push_back(P);
	NbPieces ++;
};


void Player::Remove(Piece P)
{
	int Del=0;
	std::vector<Piece>::iterator itr;
	for(itr=mPieces.begin();itr!=mPieces.end();itr++)
	{
		if(itr->Couleur==P.Couleur && itr->Tete==P.Tete && itr->Queue==P.Queue) break;
		Del++;
	}
	SuprimerPiece(Del);
}

void Player::SuprimerPiece(int Pos)
{
	std::vector<Piece>::iterator itr=mPieces.begin();
	for(int i=0;i<Pos;i++) itr++;
	mPieces.erase(itr);
};


void Player::RemetrePiece(void)
{
	mPieces.clear();
	NbPieces=0;
};



std::pair<Piece,int> Player::Opening(std::vector<std::pair<Piece,bool>> PV,std::vector<std::pair<Piece,bool>> PJ,std::vector<std::pair<Piece,bool>> PVI,
								std::vector<std::pair<Piece,bool>> PO,std::vector<Piece> Pochette)
{
	//Contient les double qui sont dans la Main du joueur
	std::vector<Piece> LDouble;
	std::vector<Piece>::iterator itr;
	//Premierement on met les double dans la liste LDouble
	for(itr= mPieces.begin();itr!=mPieces.end();itr++)
		if(itr->Tete==itr->Queue) LDouble.push_back(*itr);

	//Si on a pas de double on prend d'autre pieces
	if(LDouble.size()==0)
	{
		bool Double=false;
		//TQ on a pas de double on tire une piece
		while(!Double)
		{
			Piece P= Pochette.front(); //On prend une piece de la pochette
			Pochette.erase(Pochette.begin(),Pochette.begin()+1);
			mPieces.push_back(P); //Et on l'ajoute ?la main du joueur
			NbPieces++;			
			if(P.Tete==P.Queue)//Si c'est un double on l'ajoute ?la liste des double
			{
				LDouble.push_back(P);
				Double=true;
			}
		}//While
	}

	//Ici onverifie si on peut poser la piece
	std::vector<Piece>::iterator Aposer;
	int Position =-1;
	int MaxVal=-1,Pos=-1;	
	for(itr=LDouble.begin();itr!=LDouble.end();itr++)
	{
		switch(itr->Couleur)
		{
		case Color::Vert:
			{
				bool PeutPoser=true;				
				std::pair<Piece,bool> C1=PJ[4];
				std::pair<Piece,bool> C2=PO[0];
				//On verifie si la case dans le cercle adjacent est occupée				
				if(C1.second)
					//Si oui alors , on verfie si on peut poser
						if(C1.first.Queue!=itr->Tete) PeutPoser=false;
				if(itr->Tete==-1) PeutPoser=true;
				if(!PeutPoser)
				{
					if(C2.second)
						if(C2.first.Tete==itr->Tete)
						{							
							std::pair<Piece,bool>Pos=PV[8];
							//Si la case n'est pas occuper alors on pose la piece							
							if(!Pos.second)
							{
								PeutPoser=true;
								Position=8;
								break;
							}
						}
				}
				else
				{
					std::pair<Piece,bool>Pos=PV[12];
					if(!Pos.second)
					{
						Position=12;
						break;
					}
				}
				break;
			}			
		case Color::Jaune:
			{
				bool PeutPoser=true;
				std::pair<Piece,bool> C1=PV[12];
				std::pair<Piece,bool> C2=PVI[0];
				//On verifie si la case dans le cercle adjacent est occupée
				if(C1.second)
					//Si oui alors , on verfie si on peut poser
						if(C1.first.Queue!=itr->Tete) PeutPoser=false;
				if(itr->Tete==-1) PeutPoser=true;
				if(!PeutPoser)
				{
					if(C2.second)
						if(C2.first.Tete==itr->Tete)
						{
							std::pair<Piece,bool>Pos=PJ[8];
							if(!Pos.second)
							{
								Position=8;
								PeutPoser=true;
								break;
							}
						}
				}
				else
				{
					std::pair<Piece,bool>Pos=PJ[4];
					if(!Pos.second)
					{
						Position=4;
						break;
					}
				}
			}
			break;
		case Color::Violet:
			{
				bool PeutPoser=true;
				std::pair<Piece,bool> C1=PJ[8];
				std::pair<Piece,bool> C2=PO[12];
				//On verifie si la case dans le cercle adjacent est occupée
				if(C1.second)
					//Si oui alors , on verfie si on peut poser
						if(C1.first.Queue!=itr->Tete) PeutPoser=false;
				if(itr->Tete==-1) PeutPoser=true;//Si c'est une Zinga alors on peut poser
				if(!PeutPoser)
				{
					if(C2.second)
						if(C2.first.Tete==itr->Tete)
						{
							std::pair<Piece,bool> Pos=PVI[4];
							if(!Pos.second)
							{
								Position=4;
								PeutPoser=true;
								break;
							}
						}
				}
				else
				{
					std::pair<Piece,bool> Pos=PVI[0];
					if(!Pos.second)
					{
						Position=0;
						break;
					}

				}
			}
			break;
		case Color::Orange:
			{
				bool PeutPoser=true;
				std::pair<Piece,bool> C1=PVI[4];
				std::pair<Piece,bool> C2=PV[8];
				//On verifie si la case dans le cercle adjacent est occupée
				if(C1.second)
					//Si oui alors , on verfie si on peut poser
						if(C1.first.Queue!=itr->Tete) PeutPoser=false;
				if(itr->Tete==-1) PeutPoser=true;
				if(!PeutPoser)
				{
					if(C2.second)
						if(C2.first.Tete==itr->Tete)
						{
							std::pair<Piece,bool> Pos=PO[0];
							if(!Pos.second)
							{
								Position=0;
								PeutPoser=true;
								break;
							}
						}
				}
				else
				{
					std::pair<Piece,bool> Pos=PO[12];
					if(!Pos.second)
					{
						Position=12;
						break;
					}
				}
			}
			break;
		}//Switch

		std::vector<Piece>::iterator cpt;
		int Nb=0;
		for(cpt=mPieces.begin();cpt!=mPieces.end();cpt++)
		{
			//Si c'est une piece autre que celle-ci
			//On compte le nombre de pieces qui ont un numero que cette piece
			//Pour assuer au moins qu'on peut poser dans le tour suivant
			if((itr->Couleur==cpt->Couleur)&&(itr->Tete!=cpt->Tete)&&(itr->Queue!=cpt->Queue))
			{
				if((itr->Tete==cpt->Tete)||(itr->Tete==cpt->Queue)) Nb++;
			}
			else
			{
				if((itr->Couleur!=cpt->Couleur)&&(cpt->Queue==cpt->Tete)&&(cpt->Tete==itr->Tete))
				{
					switch(itr->Couleur)
					{
					case Vert:
						if(cpt->Couleur==Orange && Position== 8) Nb++;
						else
							if(cpt->Couleur==Jaune && Position==12) Nb++;
						break;
					case Jaune:
						if(cpt->Couleur==Vert && Position== 4) Nb++;
						else
							if(cpt->Couleur==Violet && Position==8) Nb++;
						break;
					case Violet:
						if(cpt->Couleur==Orange && Position== 4) Nb++;
						else
							if(cpt->Couleur==Jaune && Position==0) Nb++;
						break;
					case Orange:
						if(cpt->Couleur==Vert && Position== 0) Nb++;
						else
							if(cpt->Couleur==Violet && Position==12) Nb++;
						break;
					}
				}
			}
		}
		if(Nb>MaxVal)
		{
			MaxVal=Nb;
			Pos=Position;
			Aposer=itr;
		}
	}//For

	//Si on peut pas occuper les bridge au debut alors on essaye de d'occuper le cercle avec 
	//le moin de peices
	if(Pos==-1) 
	{
		std::vector<std::pair<int,Color>> Nb;
		Nb.resize(4);
		Nb[0].second=Vert;
		Nb[1].second=Jaune;
		Nb[2].second=Violet;
		Nb[3].second=Orange;

		//On compte le nombre de case Double/Zinga occupées dans chaque couleur
		for(int i=0;i<16;i+=4)
		{
			std::pair<Piece,bool> CV=PV[i];
			std::pair<Piece,bool> CJ=PJ[i];
			std::pair<Piece,bool> CVI=PVI[i];
			std::pair<Piece,bool> CO=PO[i];

			if(CV.second)Nb[0].first++;
			else
				if(CJ.second) Nb[1].first++;
				else
					if(CVI.second) Nb[2].first++;
					else
						if(CO.second) Nb[3].first++;
			//i+=3;
		}

		//On place la piece dans le cercle qui contient le min de pieces
		//Premierement on trie le tableau
		bool Stop=false;
		while(!Stop)
		{
			Stop=true;
			for(int j=0;j<3;j++)
				if(Nb[j].first>Nb[j+1].first)
				{
					std::pair<int,Color> c= Nb[j+1];
					Nb[j+1]=Nb[j];
					Nb[j]=c;
					Stop=false;
				}
		}//While

		//On essaye de trouver la piece qui peut se poser
		std::vector<std::pair<int,Color>>::iterator iter;
		for(iter=Nb.begin();iter!=Nb.end();iter++)
		{
			std::vector<Piece>::iterator it;
			for(it=LDouble.begin();it!=LDouble.end();it++)
			{
				/*Si il existe une piece de la même couleur que le cercle voulu alors 
				on la pose*/
				if(it->Couleur==iter->second)
				{
					switch(it->Couleur)
					{
					case Vert:
						if(!PV[0].second)//Si la case n'est pas occuper alors on la prend
						{
							Aposer=it;
							Pos=0;								
						}
						else
							if(!PV[4].second)
							{
								Aposer=it;
								Pos=4;									
							}
							break;
					case Jaune:
						if(!PJ[0].second)//Si la case n'est pas occuper alors on la prend
						{
							Aposer=it;
							Pos=0;
							break;
						}
						else
							if(!PJ[12].second)
							{
								Aposer=it;
								Pos=12;
								break;
							}
							break;
					case Violet:
						if(!PVI[8].second)//Si la case n'est pas occuper alors on la prend
						{
							Aposer=it;
							Pos=8;
							break;
						}
						else
							if(!PVI[12].second)
							{
								Aposer=it;
								Pos=12;
								break;
							}
							break;
					case Orange:
						if(!PO[4].second)//Si la case n'est pas occuper alors on la prend
						{
							Aposer=it;
							Pos=4;
							break;
						}
						else
							if(!PO[8].second)
							{
								Aposer=it;
								Pos=8;
								break;
							}
					}//Switch
				}//IF it
			}//For it
		}//for iter
	}//If position==-1
	NbPieces--;
	std::pair<Piece,int> ret(*Aposer,Pos);
	return ret;
}


std::pair<Piece,int> Player::Think(std::vector<std::pair<Piece,bool>> PV,std::vector<std::pair<Piece,bool>> PJ,std::vector<std::pair<Piece,bool>> PVI,std::vector<std::pair<Piece,bool>> PO,std::vector<Piece> Pochette)
{
	//On cree la liste des places dans les quelle on peut jouer
	std::vector<std::vector<std::pair<Piece,int>>> PlacesVides;
	PlacesVides.resize(4);
	std::pair<Piece,int> Pos;
	for(int n=0;n<16;n++)
	{
		if(!PV[n].second)
		{
			if((PV[n].first.Tete!=9 && PV[n].first.Tete!=10)||(PV[n].first.Queue!=9 && PV[n].first.Queue!=10))
			{
				Pos.first = PV[n].first;
				Pos.first.Couleur = Vert;
				Pos.second = n;
				PlacesVides[0].push_back(Pos);
			}
		}
		if(!PJ[n].second)
		{
			if((PJ[n].first.Tete!=9 && PJ[n].first.Tete!=10)||(PJ[n].first.Queue!=9 && PJ[n].first.Queue!=10))
			{
				Pos.first = PJ[n].first;
				Pos.first.Couleur = Jaune;
				Pos.second = n;
				PlacesVides[1].push_back(Pos);
			}
		}

		if(!PVI[n].second)
		{
			if((PVI[n].first.Tete!=9 && PVI[n].first.Tete!=10)||(PVI[n].first.Queue!=9 && PVI[n].first.Queue!=10))
			{
				Pos.first = PVI[n].first;
				Pos.first.Couleur = Violet;
				Pos.second = n;
				PlacesVides[2].push_back(Pos);
			}
		}

		if(!PO[n].second)
			if((PO[n].first.Tete!=9 && PO[n].first.Tete!=10)||(PO[n].first.Queue!=9 && PO[n].first.Queue!=10))
			{
				Pos.first = PO[n].first;
				Pos.first.Couleur = Orange;
				Pos.second = n;
				PlacesVides[3].push_back(Pos);
			}
	}

	//On ogranise les pieces qui sont dans la main du joueur par couleur
	std::vector<std::vector<Piece>> InMyHand;
	InMyHand.resize(4);
	std::vector<Piece>::iterator itr;
	for(itr=mPieces.begin();itr!=mPieces.end();itr++) InMyHand[itr->Couleur].push_back(*itr);
	//***********************************************************************************************
	unsigned char NumP =0;
	std::vector<Sommet> Racine;//la racine de l'arbre
	std::vector<std::vector<Sommet>> MinMax;
	MinMax.resize(4); //4 niveax de l'arbre seulement

	//ON initialise la racine de l'arbre
	std::vector<Piece>::iterator it;
	for(it=mPieces.begin();it!=mPieces.end();it++)
	{
		std::vector<std::pair<Piece,int>>::iterator cpt;
		for(cpt=PlacesVides[it->Couleur].begin();cpt!=PlacesVides[it->Couleur].end();cpt++)
		{
			if(cpt->second%4==0)
				if(it->Queue!=it->Tete) continue;
			//Si on attand une piece qui a le même numero que la tete 
			if(cpt->first.Tete !=9 && cpt->first.Tete!=10)
			{   //Si il y a une pieces qui peut être poser alors on l'ajoute au possobilités 
				if(cpt->first.Tete== it->Tete || cpt->first.Tete ==it->Queue || it->Tete ==-1)
				{
					Sommet S;
					S.N.Eval =-5; //Indique non encore evalu?
					S.N.Main =SansCettePiece(*it,InMyHand); //Les pieces qui reste
					S.N.PosPossible = SansCettePos(*it,*cpt,PlacesVides); //les place vides qui reste
					S.N.Num =0;
					S.N.Tail.first =*it;
					S.N.Tail.second =cpt->second;
					Racine.push_back(S);
					continue;
				}
			}
			else
				if(cpt->first.Queue !=9 && cpt->first.Queue!=10)
					if(cpt->first.Queue== it->Tete || cpt->first.Queue ==it->Queue)
					{
						Sommet S;
						S.N.Eval =-5;
						S.N.Main =SansCettePiece(*it,InMyHand);
						S.N.PosPossible = SansCettePos(*it,*cpt,PlacesVides);
						S.N.Num =0;
						S.N.Tail.first =*it;
						S.N.Tail.second =cpt->second;
						Racine.push_back(S);
						continue;
					}

		}
	}
	//Si on a le choix de poser plus qu'une piece alors on crée l'arbre
	std::pair<Piece,int> Ajouer;
	if(Racine.size()>1)
	{
		MinMax[0] = Racine;

		//********************On cree la liste des pieces qui reste**************************************
		std::vector<std::vector<Piece>> InOtherHand;
		InOtherHand.resize(4);
		std::vector<Piece>::iterator Pitr;
		Piece P;                                    
		int begin=0;
		//On percoure toutes les couleurs
		for(int i=0;i<5;i++)
		{
			for(int j=begin;j<5;j++)
			{
				P.Tete=i;
				P.Queue=j;
				Color Col;
				Col=Color::Vert;
				for(int k=0;k<4;k++)
				{
					P.Couleur =Col;
					//Si la pieces n'est pas dans notre main alors l'ajoute
					bool exist =false;
					for(Pitr=InMyHand[Col].begin();Pitr!=InMyHand[Col].end();Pitr++)
					{
						if(Pitr->Couleur== P.Couleur && Pitr->Tete==P.Tete && Pitr->Queue == P.Queue)
						{ exist =true;
						break;
						}
					}
					//Si elle n'est pas dans ma main alors on verifie si elle n'est pas sur la table du jeux
					if(!exist)
					{
						std::vector<std::pair<Piece,bool>> PsurTable;
						switch(P.Couleur)
						{
						case Vert : PsurTable = PV;
							break;
						case Jaune: PsurTable =PJ;
							break;
						case Violet : PsurTable =PVI;
							break;
						case Orange : PsurTable = PO;
						}

						std::vector<std::pair<Piece,bool>>::iterator ptr;
						for(ptr=PsurTable.begin();ptr!=PsurTable.end();ptr++)
						{
							if((ptr->first.Tete ==P.Tete && ptr->first.Queue==P.Queue) ||(ptr->first.Tete ==P.Queue && ptr->first.Queue==P.Tete))
							{exist = true;
							break;
							}
						}//Si la piece n'est ni dans ma main ni dans la table alors on la met.
						if(!exist) InOtherHand[P.Couleur].push_back(P);		

					}
					Col=(Color)((Col+1)%4);
				}
			}
			begin++;
		}
		//Ajout des pieces Zinga
		Color Col;
		Col=Color::Vert;
		P.Queue=-1;
		P.Tete=-1;
		for(int c=0;c<4;c++)
		{
			P.Couleur=Col;
			bool exist =false;
			for(Pitr=InMyHand[Col].begin();Pitr!=InMyHand[Col].end();Pitr++)
			{
				if(Pitr->Couleur== P.Couleur && Pitr->Tete==P.Tete && Pitr->Queue == P.Queue)
				{ exist =true;
				break;
				}
			}
			if(!exist) 
			{
				std::vector<std::pair<Piece,bool>> PsurTable;
				switch(P.Couleur)
				{
				case Vert : PsurTable = PV;
					break;
				case Jaune: PsurTable =PJ;
					break;
				case Violet : PsurTable =PVI;
					break;
				case Orange : PsurTable = PO;
				}
				std::vector<std::pair<Piece,bool>>::iterator ptr;
				for(ptr=PsurTable.begin();ptr!=PsurTable.end();ptr++)
				{
					if((ptr->first.Tete ==P.Tete && ptr->first.Queue==P.Queue) ||(ptr->first.Tete ==P.Queue && ptr->first.Queue==P.Tete))
					{exist = true;
					break;
					}
				}
				if(!exist) InOtherHand[P.Couleur].push_back(P);		
			}
			Col=(Color)((Col+1)%4);
		}
		//*******************************************************************************************************


		std::vector<int> Index; //Utiliser comme index pour le parcour de l'arbre
		Index.resize(4);
		for(int i=0;i<4;i++) Index[i]=0;
		int Niveau=0; //Indique le niveau actuelle du noeud ?traiter
		bool Fin=false;
		Sommet S;
		S.N.Main =InOtherHand;
		std::vector<Sommet> CurLevel;
		std::vector<Sommet> NextLevel;
		std::vector<Sommet> PrecLevel;
		while(!Fin) //Tanque on a pas Generer tous les possibilit?on s'arrete pas
		{
			//On charge le niveau actuelle
			CurLevel = MinMax[Niveau];
			if(Niveau<3) NextLevel=MinMax[Niveau+1];
			if(Niveau>0) PrecLevel =MinMax[Niveau-1];
			int Cur =0;
			//Puis on parcourt tous les noueds de ce niveau
			for(int Choix=0;Choix<(int)CurLevel.size();Choix++)
			{

				//Si ce n'est pas une feuille alors on genere les possibilités
				if(CurLevel[Choix].N.Eval ==-5)
				{
					if(Niveau>0)
					{
						if((int)PrecLevel[Cur].Fils.size()>0)
						{
							if(Choix>PrecLevel[Cur].Fils[PrecLevel[Cur].Fils.size()-1])
							{Cur++;
							S.N.Main=PrecLevel[Cur].N.Main;
							}
						}
						else{Cur++; 
						continue;}
					}
					std::vector<int> Fils;
					//On cherche les qui peuvent se posées
					Sommet V =CurLevel[Choix];
					std::vector<Piece>::iterator pos;//Piece a pos?
					for(int col=0;col<4;col++)
						for(pos=S.N.Main[col].begin();pos!=S.N.Main[col].end();pos++)
						{
							std::vector<std::pair<Piece,int>>::iterator O;//C'est une position vide
							for(O=V.N.PosPossible[col].begin();O!=V.N.PosPossible[col].end();O++)
							{
								if(O->second%4==0)
									if(pos->Queue!=pos->Tete) continue;
								if(O->first.Tete !=9 && O->first.Tete !=10)
								{
									if(O->first.Tete ==pos->Tete || O->first.Tete ==pos->Queue)
									{
										Sommet W;
										W.N.Main = SansCettePiece(*pos,S.N.Main);
										W.N.PosPossible =SansCettePos(*pos,*O,V.N.PosPossible);
										W.N.Tail.first= *pos;
										W.N.Tail.second = O->second;

										if(Niveau%2==0)
										{W.N.Num= 1;
										bool Win = true;
										//On verifie si on terminer toutes les pieces
										for(int B=0;B<4;B++) if(W.N.Main[B].size()!=0) Win = false;
										if(Win) W.N.Eval =7;
										else
											if((int)V.N.PosPossible[pos->Couleur].size()==0) W.N.Eval=2;
											else W.N.Eval=-5;
										}
										else{
											W.N.Num=0;
											bool Win = true;
											//On verifie si on terminer toutes les pieces
											for(int B=0;B<4;B++) if(W.N.Main[B].size()!=0) Win = false;
											if(Win) W.N.Eval =-7;
											else
												if((int)V.N.PosPossible[pos->Couleur].size()==0) W.N.Eval=-6;
												else W.N.Eval=-5;
										}
										Fils.push_back((int)NextLevel.size());										
										//On limite le nombre de possibilit?a considér?
										if((int)NextLevel.size()<200) { NextLevel.push_back(W); }
									}
								}

								if(O->first.Queue !=9 && O->first.Queue !=10)
								{
									if(O->first.Queue ==pos->Tete || O->first.Queue ==pos->Queue)
									{
										Sommet W;
										W.N.Main = SansCettePiece(*pos,S.N.Main);
										W.N.PosPossible =SansCettePos(*pos,*O,V.N.PosPossible);
										W.N.Tail.first= *pos;
										W.N.Tail.second = O->second;
										//On verifie si on a fermer un cercle ou non
										if(Niveau%2==0)
										{W.N.Num= 1;
										bool Win = true;
										//On verifie si on terminer toutes les pieces
										for(int B=0;B<4;B++) if(W.N.Main[B].size()!=0) Win = false;
										if(Win) W.N.Eval =7;
										else
											if((int)V.N.PosPossible[pos->Couleur].size()==0) W.N.Eval=2;
											else W.N.Eval=-5;
										}
										else{
											W.N.Num=0;
											bool Win = true;
											//On verifie si on terminer toutes les pieces
											for(int B=0;B<4;B++) if(W.N.Main[B].size()!=0) Win = false;
											if(Win) W.N.Eval =-7;
											else
												if((int)V.N.PosPossible[pos->Couleur].size()==0) W.N.Eval=-6;
												else W.N.Eval=-5;
										}
										Fils.push_back((int)NextLevel.size());
										if((int)NextLevel.size()<200)NextLevel.push_back(W); 
									}
								}

							}//?for(O)
						}//for(pos)
						//Si on peut pas poser alors on indique blocage
						if((int)Fils.size() ==0)
							if(Niveau%2==0) CurLevel[Choix].N.Eval=-1;
							else CurLevel[Choix].N.Eval=1;
							V.Fils =Fils;
							CurLevel[Choix]=V;
				}

			}//§for(Choix=0)
			MinMax[Niveau]=CurLevel;
			if(Niveau<3) MinMax[Niveau+1]=NextLevel;
			Niveau++;
			if(Niveau==3)Fin=true;
		}

		//Puis on cherche la meilleur solution dans l'arbre
		int BestSol =-5;
		Niveau=2;
		int Pos;
		while(Niveau>=0)
		{
			CurLevel = MinMax[Niveau];
			NextLevel = MinMax[Niveau+1];
			int Best;
			Pos=0;
			for(int i=0;i<(int)CurLevel.size();i++)
			{
				Best =CurLevel[i].N.Eval;
				if((int)CurLevel[i].Fils.size()>0)
				{
					int Begin = CurLevel[i].Fils[0];
					for(int j=Begin;j<(int)CurLevel[i].Fils.size();j++)
					{
						//Si c'est notre tour on choisit le MAX
						if(Niveau%2==0)
						{
							if(NextLevel[j].N.Eval>Best)
							{Best = NextLevel[j].N.Eval;
							Pos=j;
							}
						}else //Sinon on minimise
							if(NextLevel[j].N.Eval<Best){
								Best = NextLevel[j].N.Eval;
								Pos=j;
							}
					}
					CurLevel[i].Fils.clear();
				}
				if(Best==-5) Best =0;
				CurLevel[i].N.Eval=Best;
			}
			if(Niveau==0) BestSol = Best;
			Niveau--;
		}
		CurLevel =MinMax[0];
		Ajouer = CurLevel[Pos].N.Tail;
		MinMax.clear();
	}//If (racine.size()>1)
	else 
		if((int)Racine.size()==1)Ajouer =Racine[0].N.Tail;
		else Ajouer.second =-3;

		return Ajouer;
};

std::vector<std::vector<Piece>> Player::SansCettePiece(Piece P,std::vector<std::vector<Piece>> Main)
{
		std::vector<Piece>::iterator itr;
		for(itr=Main[P.Couleur].begin();itr!=Main[P.Couleur].end();itr++)
		{
			if((itr->Tete == P.Tete && itr->Queue ==P.Queue)||(itr->Tete==P.Queue && itr->Queue ==P.Tete))
			{
				Main[P.Couleur].erase(itr);
				break;
			}
		}

		return Main;
}



std::vector<std::vector<std::pair<Piece,int>>> Player::SansCettePos(Piece P,std::pair<Piece,int> Pos,std::vector<std::vector<std::pair<Piece,int>>> PosPossible)
	{
		std::vector<std::pair<Piece,int>>::iterator it;

		for(it=PosPossible[Pos.first.Couleur].begin();it!=PosPossible[Pos.first.Couleur].end();it++)
			//Si on a trouver cette piece alors on MAJ les 
			if(it->second == Pos.second)
			{ 
				PosPossible[it->first.Couleur].erase(it);
				if(Pos.second <8){
					if(Pos.second ==0) 
					{
						//On met a jours les cases voisine
						//1:On verifie s'il sont deja dans PosPieces
						std::vector<std::pair<Piece,int>>::iterator I;
						bool Exist1 = false,Exist15=false;
						for(I=PosPossible[Pos.first.Couleur].begin();I!=PosPossible[Pos.first.Couleur].end();I++)
						{

							if(I->second==1 || I->second==15)
							{
								(I->second==1)?(Exist1=true):(Exist15=true);
								I->first.Tete = P.Tete;
								if(Exist1 && Exist15) break;
							}
						}
						if(!Exist1 || !Exist15 )
						{
							std::pair<Piece,int> V;
							V.first.Tete = P.Tete;
							V.first.Queue= 9;
							V.first.Couleur = Pos.first.Couleur;
							(!Exist1)?(V.second=1):V.second=15;
							PosPossible[Pos.first.Couleur].push_back(V);
						}
						//On verifie si on peut MAJ les cercles voisins
						switch(Pos.first.Couleur)
						{
						case Violet:
							{
								bool exist =false;
								for(I =PosPossible[1].begin();I!=PosPossible[1].end();I++)
									if(I->second ==8)
									{
										exist =true;
										if(I->first.Tete!=Pos.first.Tete)
										{
											I->first.Tete =10;
											I->first.Queue=10;
										}
										break;
									}
									if(!exist)
									{
										std::pair<Piece,int> M;
										M.first.Couleur = Jaune;
										M.first.Tete =Pos.first.Tete;
										M.first.Queue =Pos.first.Tete;
										M.second =8;
										PosPossible[1].push_back(M);
									}
							}
							break;
						case Orange:
							{
								bool exist =false;
								for(I =PosPossible[0].begin();I!=PosPossible[0].end();I++)
									if(I->second ==8)
									{
										exist =true;
										if(I->first.Tete!=Pos.first.Tete)
										{
											I->first.Tete =10;
											I->first.Queue=10;
										}
										break;
									}
									if(!exist)
									{
										std::pair<Piece,int> M;
										M.first.Couleur = Vert;
										M.first.Tete =Pos.first.Tete;
										M.first.Queue =Pos.first.Tete;
										M.second =8;
										PosPossible[0].push_back(M);
									}
							}
						}
					}
					else
					{//Pos!=0
						//Si la Voisin1 n'est pas vide
						if(Pos.first.Tete !=9 && Pos.first.Tete !=10)
						{    //Et le Voisin2 n'est pas vide
							if(Pos.first.Queue!=9 && Pos.first.Queue!=10)
							{   //On suprime directement la position vide 
								//PosPossible[Pos.first.Couleur].erase(it);
							}
							else
							{
								if(P.Tete==-1)
								{
									P.Tete=Pos.first.Tete;
									P.Queue=Pos.first.Tete;
								}
								//On verifie si le voisin bas est deja dans PosPossible
								std::vector<std::pair<Piece,int>>::iterator j;
								bool Yes=false;
								for(j=PosPossible[Pos.first.Couleur].begin();j!=PosPossible[Pos.first.Couleur].end();j++)
								{
									if(j->second == Pos.second+1)
									{
										Yes = true;
										j->first.Tete = P.Queue;
										break;
									}
								}//§For j
								//Si il n'existe pas alors on l'ajoute
								if(!Yes)
								{
									std::pair<Piece,int> np;
									np.first.Couleur =P.Couleur;
									if(P.Tete==P.Queue)np.first.Tete =P.Tete;
									else
										if(P.Tete ==Pos.first.Tete) np.first.Tete = P.Queue;
										else np.first.Tete =P.Tete;
										np.first.Queue =9;
										np.second= Pos.second+1;
										PosPossible[P.Couleur].push_back(np);
								}
							}	 
						}
						else
						{
							if(Pos.first.Queue !=9 && Pos.first.Queue !=10)
							{
								if(P.Tete==-1)
								{
									P.Tete=Pos.first.Queue;
									P.Queue=Pos.first.Queue;
								}
								//On verifie si le voisin Haut est deja dans PosPossible
								std::vector<std::pair<Piece,int>>::iterator j;
								bool Yes=false;
								for(j=PosPossible[Pos.first.Couleur].begin();j!=PosPossible[Pos.first.Couleur].end();j++)
								{
									if(j->second == Pos.second-1)
									{
										Yes = true;
										j->first.Tete = P.Queue;
										break;
									}
								}//§For j
								//Si il n'existe pas alors on l'ajoute
								if(!Yes)
								{
									std::pair<Piece,int> np;
									np.first.Couleur =P.Couleur;
									np.first.Tete =9;
									if(P.Tete==P.Queue)np.first.Queue =P.Tete;
									else
										if(P.Tete ==Pos.first.Queue) np.first.Queue = P.Queue;
										else np.first.Queue =P.Tete;
										np.second= Pos.second-1;
										PosPossible[P.Couleur].push_back(np);
								}
							}
						}
						if(Pos.second ==4){
							//On verifie si on peut MAJ les cercles voisins
							std::vector<std::pair<Piece,int>>::iterator I;
							switch(Pos.first.Couleur)
							{
							case Jaune:
								{
									bool exist =false;
									for(I =PosPossible[0].begin();I!=PosPossible[0].end();I++)
										if(I->second ==12)
										{
											exist =true;
											if(I->first.Tete!=Pos.first.Tete)
											{
												I->first.Tete =10;
												I->first.Queue=10;
											}
											break;
										}
										if(!exist)
										{
											std::pair<Piece,int> M;
											M.first.Couleur = Vert;
											M.first.Tete =P.Tete;
											M.first.Queue =P.Tete;
											M.second =12;
											PosPossible[0].push_back(M);
										}
								}
								break;
							case Violet:
								{
									bool exist =false;
									for(I =PosPossible[3].begin();I!=PosPossible[3].end();I++)
										if(I->second ==12)
										{
											exist =true;
											if(I->first.Tete!=Pos.first.Tete)
											{
												I->first.Tete =10;
												I->first.Queue=10;
											}
											break;
										}
										if(!exist)
										{
											std::pair<Piece,int> M;
											M.first.Couleur = Orange;
											M.first.Tete =P.Tete;
											M.first.Queue =P.Tete;
											M.second =12;
											PosPossible[3].push_back(M);
										}
								}
							}
						}//pos==4
					}
				}//IF pos.second <8
				else
				{
					if(Pos.second==8)
					{
						//On met a jours les cases voisine
						//1:On verifie s'il sont deja dans PosPieces
						std::vector<std::pair<Piece,int>>::iterator I;
						bool Exist7 = false,Exist9=false;
						for(I=PosPossible[Pos.first.Couleur].begin();I!=PosPossible[Pos.first.Couleur].end();I++)
						{

							if(I->second==7 || I->second==9)
							{
								(I->second==7)?(Exist7=true):(Exist9=true);
								I->first.Tete = P.Tete;
								if(Exist7 && Exist9) break;
							}
						}
						if(!Exist7 || !Exist9 )
						{
							std::pair<Piece,int> V;
							V.first.Queue = P.Tete;
							V.first.Tete= 9;
							V.first.Couleur = Pos.first.Couleur;
							if(!Exist7){V.second=7;PosPossible[Pos.first.Couleur].push_back(V);}
							if(!Exist9){V.second=9;PosPossible[Pos.first.Couleur].push_back(V);}
						}
						//On verifie si on peut MAJ les cercles voisins
						switch(Pos.first.Couleur)
						{
						case Jaune:
							{
								bool exist =false;
								for(I =PosPossible[2].begin();I!=PosPossible[2].end();I++)
									if(I->second ==0)
									{
										exist =true;
										if(I->first.Tete!=Pos.first.Tete)
										{
											I->first.Tete =10;
											I->first.Queue=10;
										}
										break;
									}
									if(!exist)
									{
										std::pair<Piece,int> M;
										M.first.Couleur = Violet;
										M.first.Tete =P.Tete;
										M.first.Queue =P.Tete;
										M.second =0;
										PosPossible[2].push_back(M);
									}
							}
							break;
						case Vert:
							{
								bool exist =false;
								for(I =PosPossible[3].begin();I!=PosPossible[3].end();I++)
									if(I->second ==0)
									{
										exist =true;
										if(I->first.Tete!=Pos.first.Tete)
										{
											I->first.Tete =10;
											I->first.Queue=10;
										}
										break;
									}
									if(!exist)
									{
										std::pair<Piece,int> M;
										M.first.Couleur = Orange;
										M.first.Tete =P.Tete;
										M.first.Queue =P.Tete;
										M.second =0;
										PosPossible[3].push_back(M);
									}
							}
						}
					}
					else
					{
						//Si la Voisin1 n'est pas vide
						if(Pos.first.Tete !=9 && Pos.first.Tete !=10)
						{    //Et le Voisin2 n'est pas vide
							if(Pos.first.Queue!=9 && Pos.first.Queue!=10)
							{   //On suprime directement la position vide 
								//PosPossible[Pos.first.Couleur].erase(it);
							}
							else
							{
								if(P.Tete==-1)
								{
									P.Tete=Pos.first.Tete;
									P.Queue=Pos.first.Tete;
								}
								//On verifie si le voisin bas est deja dans PosPossible
								std::vector<std::pair<Piece,int>>::iterator j;
								bool Yes=false;
								for(j=PosPossible[Pos.first.Couleur].begin();j!=PosPossible[Pos.first.Couleur].end();j++)
								{
									if(j->second == Pos.second-1)
									{
										Yes = true;
										j->first.Tete = P.Queue;
										break;
									}
								}//§For j
								//Si il n'existe pas alors on l'ajoute
								if(!Yes)
								{
									std::pair<Piece,int> np;
									np.first.Couleur =P.Couleur;
									if(P.Tete==P.Queue)np.first.Tete =P.Tete;
									else
										if(P.Tete ==Pos.first.Tete) np.first.Tete = P.Queue;
										else np.first.Tete =P.Tete;
										np.first.Queue =9;
										np.second= Pos.second-1;
										PosPossible[P.Couleur].push_back(np);
								}
							}	 
						}
						else
						{
							if(Pos.first.Queue !=9 && Pos.first.Queue !=10)
							{
								if(P.Tete==-1)
								{
									P.Tete=Pos.first.Queue;
									P.Queue=Pos.first.Queue;
								}
								//On verifie si le voisin Haut est deja dans PosPossible
								std::vector<std::pair<Piece,int>>::iterator j;
								bool Yes=false;
								for(j=PosPossible[Pos.first.Couleur].begin();j!=PosPossible[Pos.first.Couleur].end();j++)
								{
									if(j->second == (Pos.second+1)%16)
									{
										Yes = true;
										j->first.Tete = P.Queue;
										break;
									}
								}//§For j
								//Si il n'existe pas alors on l'ajoute
								if(!Yes)
								{
									std::pair<Piece,int> np;
									np.first.Couleur =P.Couleur;
									np.first.Tete =9;
									if(P.Tete==P.Queue)np.first.Queue =P.Tete;
									else
										if(P.Tete ==Pos.first.Queue) np.first.Queue = P.Queue;
										else np.first.Queue =P.Tete;
										np.second= (Pos.second+1)%16;
										PosPossible[P.Couleur].push_back(np);
								}
							}
						}
						if(Pos.second ==12){
							//On verifie si on peut MAJ les cercles voisins
							std::vector<std::pair<Piece,int>>::iterator I;
							switch(Pos.first.Couleur)
							{
							case Orange:
								{
									bool exist =false;
									for(I =PosPossible[2].begin();I!=PosPossible[2].end();I++)
										if(I->second ==4)
										{
											exist =true;
											if(I->first.Tete!=Pos.first.Tete)
											{
												I->first.Tete =10;
												I->first.Queue=10;
											}
											break;
										}
										if(!exist)
										{
											std::pair<Piece,int> M;
											M.first.Couleur = Violet;
											M.first.Tete =P.Tete;
											M.first.Queue =P.Tete;
											M.second =4;
											PosPossible[2].push_back(M);
										}
								}
								break;
							case Vert:
								{
									bool exist =false;
									for(I =PosPossible[1].begin();I!=PosPossible[1].end();I++)
										if(I->second ==4)
										{
											exist =true;
											if(I->first.Tete!=Pos.first.Tete)
											{
												I->first.Tete =10;
												I->first.Queue=10;
											}
											break;
										}
										if(!exist)
										{
											std::pair<Piece,int> M;
											M.first.Couleur = Orange;
											M.first.Tete =P.Tete;
											M.first.Queue =P.Tete;
											M.second =4;
											PosPossible[1].push_back(M);
										}
								}
							}
						}//pos==12
					}
				}

				break;
			}
			return PosPossible;
	};

Player::~Player(void)
{
}
