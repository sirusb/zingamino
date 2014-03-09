
#pragma once
#ifndef __Player_h_
#define __Player_h_


#include <string>
#include <list>
#include <vector>

typedef enum Color
{
	Vert=0,
	Jaune=1,
	Violet=2,
	Orange=3
} Color;

typedef struct Piece
{
	Color Couleur;
	int Tete;
	int Queue;
} Piece;

typedef struct Noeud
{   //Indique les posiotion ou on peut pose dans chaque couleur
	std::vector<std::vector<std::pair<Piece,int>>> PosPossible; 
	//Les pieces dans la main du joueurs triées par couleur
	std::vector<std::vector<Piece>> Main;
	//Le numero du joueur
	int Num ;//Insigned char pour occuper moin d'espace
	int Eval; //Evaluation de ce Noeud
	std::pair<Piece,int> Tail;
} Noeud;

typedef struct Sommet
{
	Noeud N;
	std::vector<int> Fils;
} Sommet;

class Player
{

public:
	//Constructeur de la class
	Player(void);
	Player(std::string Nom,int num);	
	virtual ~Player(void);

	//Donne la liste des pieces dans la main du joueur
	virtual std::vector<Piece> GetPiece(void);	

	//Remet la piece pros?dans la procedure du choit dans la pochette
	//Note: Utiliser seulement dans la selection
	virtual void RemetrePiece(void);	

	//Donne le numero du Joueur
	virtual int GetNum();	

	//Ajoute une piece a la main du joueur
	virtual void AddPiece(Piece P);	
	virtual void Remove(Piece P);
	virtual void SetScore(int Valeur);	
	virtual int GetScore(void);		
	virtual std::string GetNom(void);	

	std::string mName; //Nom du joueur
	int Score; //Son score
	int NbPieces; //Le nombre de ces pieces
	int Num;
	std::vector <Piece> mPieces;
	//A declarer dans la procedure aproprier pour gagner de l'espace
	std::vector<int> TableParcoure;//Utiliser comme un index pour le parcour de l'arbre 


	/*Demande au joueur de jouer
	DebutR : indique si c'est le debut de la round 
	PV: la liste des pieces Verte dans la Table
	PJ: la liste des pieces Jaune dans la Table
	PVI:la liste des pieces Violet dans la Table
	PO: la liste des pieces Orange dans la Table
	Sortie : la piece ?poser et la position*/
	virtual std::pair<Piece,int> Joue(bool DebutR,std::vector<std::pair<Piece,bool>> PV,std::vector<std::pair<Piece,bool>> PJ,std::vector<std::pair<Piece,bool>> PVI,std::vector<std::pair<Piece,bool>> PO,std::vector<Piece> Pochette);

	virtual void SuprimerPiece(int Pos);	


	virtual std::pair<Piece,int> Opening(std::vector<std::pair<Piece,bool>> PV,std::vector<std::pair<Piece,bool>> PJ,std::vector<std::pair<Piece,bool>> PVI,std::vector<std::pair<Piece,bool>> PO,std::vector<Piece> Pochette);

	virtual std::pair<Piece,int> Think(std::vector<std::pair<Piece,bool>> PV,std::vector<std::pair<Piece,bool>> PJ,std::vector<std::pair<Piece,bool>> PVI,std::vector<std::pair<Piece,bool>> PO,std::vector<Piece> Pochette);

	//Donne une liste de pieces sans la piece P
	virtual std::vector<std::vector<Piece>> SansCettePiece(Piece P,std::vector<std::vector<Piece>> Main);

	//Donne la liste des position sans la Position données et Mise ?jour de nouvelle position
	virtual std::vector<std::vector<std::pair<Piece,int>>> SansCettePos(Piece P,std::pair<Piece,int> Pos,std::vector<std::vector<std::pair<Piece,int>>> PosPossible);
};
#endif

