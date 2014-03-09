#include "Partie.h"

Partie::Partie(void)
{
};		

Partie::Partie(int nbJoueurs,std::string NomHumain)
	{
		//On initialise la pochet
		InitPochet();

		//Creation des Joueur;
		Player plyr(NomHumain,0);
		Players.push_back(plyr);
		for(int i=1;i<nbJoueurs;i++)
		{
			Player p("Joueur",i);
			Players.push_back(p);
		}
	};


std::vector<Player> Partie::GetPlayers(void)
{
		return Players;
};


void Partie::InitPochet(void)
	{
		//Initialisation de la Pochette
		int begin=0;
		Piece P;
		Pochette.clear();
		//Ajout des pieces ordinaire 
		//Chaqu'une avec ces 4 couleur
		for(int i=0;i<5;i++)
		{
			for(int j=begin;j<5;j++)
			{
				if(i==0 || i==3)
				{
					P.Tete=j;
					P.Queue=i;
				}else
				{
					P.Tete=i;
					P.Queue=j;
				}
				Color Col;
				Col=Vert;
				for(int k=0;k<4;k++)
				{
					P.Couleur =Col;
					Pochette.push_back(P);
					Col=(Color)((Col+1)%4);
				}
			}
			begin++;
		}
		//Ajout des pieces Zinga
		Color Col;
		Col=Vert;
		P.Queue=-1;
		P.Tete=-1;
		for(int c=0;c<4;c++)
		{
			P.Couleur=Col;
			Pochette.push_back(P);
			Col=(Color)((Col+1)%4);
		}

		//Puis On melange les pieces
		random_shuffle(Pochette.begin(),Pochette.end());
		random_shuffle(Pochette.begin(),Pochette.end());
		random_shuffle(Pochette.begin(),Pochette.end());

	};



void Partie::Choisir(void){

		std::vector<Player>::iterator iter;		
		for(iter=Players.begin();iter!=Players.end();iter++)
		{
			//change the seed
			srand(time(NULL));
			//Le joueur mellange la pochette
			for(int k=0;k<rand()%50;k++) random_shuffle(Pochette.begin(),Pochette.end());
			//Puis prend une piece
			Piece P;
			P.Couleur = Pochette[0].Couleur;
			P.Queue = Pochette[0].Queue;
			P.Tete = Pochette[0].Tete;
			Pochette.erase(Pochette.begin(),Pochette.begin()+1);
			//iter->AddPiece(P);
			(*iter).AddPiece(P);
		}
	};



void Partie::DebuteRound(void)
	{
		std::vector<Player>::iterator iter;
		//Premierement les joueurs Posent les pieces tirer dans Pour choisir le premeier
		for(iter=Players.begin();iter!=Players.end();iter++)
		{
			if((int)iter->GetPiece().size()>0)
			{
				Pochette.push_back(iter->GetPiece().front());
				iter->RemetrePiece();
			}
		};

		for(iter=Players.begin();iter!=Players.end();iter++)
		{
			//Chaque joueur prend 8 pieces
			for(int i=0;i<8;i++)
			{
				srand(time(NULL));
				//Le joueur mellange la pochette
				for(int k=0;k<rand()%50;k++) random_shuffle(Pochette.begin(),Pochette.end());
				//Puis prend une piece
				Piece P;
				P.Couleur = Pochette[0].Couleur;
				P.Queue = Pochette[0].Queue;
				P.Tete = Pochette[0].Tete;
				//On suprime cette piece de la pochette
				Pochette.erase(Pochette.begin(),Pochette.begin()+1);
				//On on l'ajoute au joueur
				(*iter).AddPiece(P);
			};
		};
	};


void Partie::EndRound(void)
{
		//On calcule les Points dans la Main de chaque Joueur
		std::vector<Player>::iterator Jitr;
		std::vector<Piece>::iterator Pitr;
		int NumWinner;
		int Total=0;
		int MinTotal=200;
		int Type=0;
		std::vector<Piece> P;
		for(Jitr=Players.begin();Jitr!=Players.end();Jitr++)
		{ 		      
			if((int)Jitr->GetPiece().size() ==0){
				NumWinner =Jitr->GetNum() ;
				Type=1;}
			else
			{
				int Sum =0;
				P =Jitr->GetPiece();
				for(Pitr=P.begin();Pitr!=P.end();Pitr++)
				{ 	Sum +=(Pitr->Queue +Pitr->Tete);
				Jitr->Remove(*Pitr);
				}
				if(Sum<MinTotal)
				{ MinTotal =Sum;
				NumWinner = Jitr->GetNum();}
				Total += Sum;

			}

		}

		if(Type==1) Players[NumWinner].SetScore(Players[NumWinner].GetScore()+Total);
		else  Players[NumWinner].SetScore(Players[NumWinner].GetScore()+Total-MinTotal);
		
		InitPochet();
	};

Partie::~Partie(void){

}