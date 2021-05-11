#include<iostream>
#include<vector>
#include "Delegate.h"
#include "Hero.h"
#include "ShadowMaster.h"
#include "Goblin.h"

// Shadow Lord is a monster type, when it dies,
// it will restore the hero¡¯s health and reduce the goblin¡¯s attack damage
// Logically, various monsters and heroes are not inclusive.
// So saving the object when the event was triggered in ShadowMaster destroys this relationship.
// So use delegate to complete the unification of code and logic.

int main()
{
	ShadowMaster shadowMaster;
	Hero player0("player0",3.f), player1("player1", 30.f), player2("player2", 5.f),
	player3("player3", 7.f), player4("player4", 100.f);
	Goblin gobin;
	
	shadowMaster.shadowMasterDeadDelegate.AddRaw(&player0, &Hero::OnShadowMasterKilled);
	shadowMaster.shadowMasterDeadDelegate.AddRaw(&player1, &Hero::OnShadowMasterKilled);
	shadowMaster.shadowMasterDeadDelegate.AddRaw(&player2, &Hero::OnShadowMasterKilled);
	shadowMaster.shadowMasterDeadDelegate.AddRaw(&player3, &Hero::OnShadowMasterKilled);
	shadowMaster.shadowMasterDeadDelegate.AddRaw(&player4, &Hero::OnShadowMasterKilled);

	shadowMaster.shadowMasterDeadDelegate.AddRaw(&gobin, &Goblin::OnShadowMasterKilled);

	shadowMaster.Dead();
	return 0;
}