#pragma once

#ifndef __Partie_h_
#define __Partie_h_

#include <string>
#include <algorithm>
#include <functional>
#include "time.h"
#include <vector>
#include "Player.h"

//#using <System.DLL>
//using namespace System;

class Partie
{
protected:
	std::vector<Piece> Pochette; //La Pochette qui contient les Pieces
	std::vector<Player> Players;
public:
	Partie(void);
	Partie(int nbJoueurs,std::string NomHumain);	
	virtual ~Partie(void);

	virtual std::vector<Player> GetPlayers(void);	

	//Initialise la pochette avec les 64 pieces
	virtual void InitPochet(void);

	//Utiliser pour tirer une pieces de la pochette
	virtual void Choisir(void);

	//Procedure Qui permet a chaque joueur de prendre une Piece
	void DebuteRound(void);

	std::vector<Piece> GetPochete(void)
	{
		return Pochette;
	}

	void Enleve(int Nb)
	{
		Pochette.erase(Pochette.begin(),Pochette.begin()+Nb);
	}

	void SetPochette(std::vector<Piece> Sac)
	{
		Pochette.clear();
		Pochette= Sac;
	}

	void MajPlayer(std::vector<Player> J)
	{
		Players.clear();
		Players=J;
	}

	void EndRound(void);	
};

#endif