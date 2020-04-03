/*-- SFT - erneutes Appendto --*/
//NUR für MappersHelp

#strict 2
#appendto SF5B

static MouseHelp;

public func ControlCommand(string strCommand, object pTarget, int iTx, int iTy, object pTarget2, int iData, object pCmdObj) 
{
	if (MouseHelp)
	{
		SetFoW(false, GetOwner(this)); //Keine Beschränkung mehr
		GameCall("AddSpawnPoint", iTx, iTy);
		return true;
	}
	return inherited(...);
}
