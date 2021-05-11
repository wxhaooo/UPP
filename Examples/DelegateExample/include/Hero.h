#pragma once
#include<string>

class Hero
{
public:
	Hero() = delete;
	Hero(const std::string& heroNameIn,float helathIn) :heroName(heroNameIn),health(helathIn){}
	
public:
	void OnShadowMasterKilled()
	{
		health = std::min(100.f, health + 20.f);
		std::cout << "Hero: " << heroName << " Restore health!" << "\n";
		std::cout << "Hero: " << heroName << " Health: " << health << "\n\n";
	}

	float health;
	std::string heroName;
};