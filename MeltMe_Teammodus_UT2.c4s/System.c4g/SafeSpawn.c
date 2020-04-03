/*-- Ballon beseitigt Schnee --*/

#strict 2
#appendto BALN

protected func Initialize()
{
	AddEffect("Unstuck", this, 1, 1, this);
	return inherited();
}

protected func FxUnstuckTimer()
{
	if (FindObject2(Find_OCF(OCF_CrewMember), Find_InRect(-13, -20, 24, 53)))
		DigFreeRect(-13 + GetX(), GetY() + 19, 24, 3);
	return true;
}
