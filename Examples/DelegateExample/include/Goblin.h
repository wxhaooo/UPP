#pragma once

class Goblin
{
public:
	void OnShadowMasterKilled()
	{
		damageValue = std::max(1.f, damageValue - 5.f);
		std::cout << "Goblin's Damage: " << damageValue << "\n\n";
	}
private:
	float damageValue = 10.f;
};