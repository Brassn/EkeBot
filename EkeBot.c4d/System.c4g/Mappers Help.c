/*-- SFT - erneutes Appendto --*/
//NUR f�r MappersHelp

#strict 2
#appendto SF5B

static MouseHelp;

public func ControlCommand(string strCommand, object pTarget, int iTx, int iTy, object pTarget2, int iData, object pCmdObj) 
{
	if (MouseHelp)
	{
		SetFoW(false, GetOwner(this)); //Keine Beschr�nkung mehr
		GameCall("AddSpawnPoint", iTx, iTy);
		return true;
	}
	return inherited(...);
}
