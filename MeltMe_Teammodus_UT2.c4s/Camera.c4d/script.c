/*-- Kamera --*/

#strict 2

static Voting; //0 = Voting done, 1 = Yes-Votes, 2 = No-Votes, 3 = Verbleibende Zeit, 4 = Aktiviert
static PlayerRelaunches;
local VotingDone;

public func Initialize()
{
	if(Voting[0])
	{
		PlayerRelaunches[GetPlayerID(GetOwner(this))]++;
		GameCall("RelaunchPlayer", GetOwner(this));
		RemoveObject(this);
		return true;
	}
	AddEffect("Invulnerable", this, 1, , this);
	SetAction("Float");
	SetVisibility(VIS_None);
	SetPosition(LandscapeWidth() / 2, LandscapeHeight() / 2);
	CreateMenu(RMMG, this, this, 0, "Relaunches erspielen können?", 0, C4MN_Style_Context, false, VOTE);
	AddMenuItem("Für NEIN stimmen", "Vote", TD1Z, this, 0, false, "Keine Relaunches erspielen können");
	AddMenuItem("Für JA stimmen", "Vote", TU1Z, this, 0, true, "Relaunches können erspielt werden");
	AddEffect("DoNotCloseMenu", this, 1, 5, this);
	return true;
}

protected func FxDoNotCloseMenuTimer()
{
	if (VotingDone)
		return -1;
	if (GetMenu(this) != VOTE)
	{
		CloseMenu();
		CreateMenu(RMMG, this, this, 0, "Relaunches erspielen können?", 0, C4MN_Style_Context, false, VOTE);
		AddMenuItem("Für NEIN stimmen", "Vote", TD1Z, this, 0, false, "Keine Relaunches erspielen können");
		AddMenuItem("Für JA stimmen", "Vote", TU1Z, this, 0, true, "Relaunches können erspielt werden");
		AddEffect("DoNotCloseMenu", this, 1, 1, this);
	}
	return true;
}

public func Vote(id crap, int iVote)
{
	if(iVote)
		Voting[1]++;
	else
		Voting[2]++;
	VotingDone = true;
	CreateMenu(RMMG, this, this, 0, "Verbleibende Voter", 0, C4MN_Style_Context, true, MONI);
	UpdateAllMenus();
	return true;
}

global func UpdateAllMenus()
{
	var sNames = [];
	for (var i; i < GetPlayerCount(C4PT_User); i++)
	{
		var oCamera = GetCursor(GetPlayerByIndex(i, C4PT_User));
		var bDone = LocalN("VotingDone", oCamera);
		if(bDone != true)
		{
			var sPlayerName = GetTaggedPlayerName(GetPlayerByIndex(i, C4PT_User));
			sNames[GetLength(sNames)] = sPlayerName;
		}
	}
	for (i = 0; i < GetPlayerCount(C4PT_User); i++)
	{
		var oCamera = GetCursor(GetPlayerByIndex(i, C4PT_User));
		if(GetMenu(oCamera) == MONI)
			oCamera ->~ ShowRemainingVoters(sNames);
	}
}


public func ShowRemainingVoters(sNames)
{
	CloseMenu(this);
	CreateMenu(RMMG, this, this, 0, "Verbleibende Voter", 0, C4MN_Style_Context, true, MONI);
	for (var i in sNames)
	{
		AddMenuItem(i, "", SF5B, this, 0, true, "Muss noch voten");
	}
	return true;
}

protected func FxInvulnerableDamage()
{
	return 0;
}

protected func Incineration(player)
{
	Extinguish();
}

protected func ControlCommand(string strCommand, object pTarget, int iTx, int iTy, object pTarget2, int iData, object pCmdObj)
{
	return true;
}
