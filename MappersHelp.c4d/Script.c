/*-- Mappers Help--*/
//Per Include ins Szenarien Script setzen

#strict 2

static iDeaths;
static iKills;
static iSuicides;
static iPlayerCount;
static szPlayerNames;
static RespawnX, RespawnY;
static TeamKilled;

//Funktionen zum überladen
protected func SetRespawnPoints()
{
	RespawnX = [];
	RespawnY = [];
}
protected func Relaunches() 
{
	return 10;
}
protected func NoMessages() 
{
	return false;
}
protected func NoSlaying() 
{
	return false;
}
protected func StartMaterial(Clonk)
{
	CreateContents(AR5B, Clonk);
	CreateContents(CA5B, Clonk);
	CreateContents(GB5B, Clonk);
}
protected func SpawnProtection() 
{
	return false;
}


//Ab hier alles normal
protected func RespawnPoints(& XOut, & YOut)
{
	if (GetType(RespawnX) != C4V_Array || GetType(RespawnY) != C4V_Array)
	{
		Log("$Set$", RGB(255, 50, 50));
		return false;
	}
	var X = RespawnX; //X Positionen
	var Y = RespawnY; //Y Positionen
	var R = Random(GetLength(X));
	XOut = X[R];
	YOut = Y[R];
}

protected func Initialize() 
{
	if (this)
	{
		RemoveObject();
		return false;
	}
	if (!FindObject(_MSG, , , , , , "MainSaver"))
		CreateObject(_MSG, 0, 0, -1)->SetAction("MainSaver");
	TeamKilled = [];
	iDeaths = [];
	iKills = [];
	iSuicides = [];
	szPlayerNames = [];
	Schedule("SetMaxPlayer()", 100);
	SetRespawnPoints();
	return true;
}

public func OnClonkDeath(object pClonk, int iKiller)
{
	var iOwner = GetOwner(pClonk);
	if (!GetPlayerName(iOwner) || !pClonk)
		return 0;
	var sTypeName = GetName(0, GetID(pClonk));
	if (!sTypeName)
		return 0;
	if (iKiller == iOwner)
		ShowMessage(Format("$Suicide$", GetTaggedPlayerName(iOwner)));
	else 
	{
		if (GetPlayerName(iKiller))
		{
			if (!Hostile(iOwner, iKiller))
				ShowMessage(Format("$TeamKill$", GetTaggedPlayerName(iKiller), GetTaggedPlayerName(iOwner)));
			else 
				ShowMessage(Format("$Killed$", GetTaggedPlayerName(iOwner), GetTaggedPlayerName(iKiller)));
		}
	}
	if (iKiller == NO_OWNER)
		return false;
	var playerID = GetPlayerID(iKiller);
	if (iKiller == GetOwner(pClonk))
	{
		iSuicides[iKiller]++;
		SetScoreboardData(playerID, 3, Format("%d", iSuicides[iKiller]));
		return 1;
	}
	if (!NoSlaying() && !Hostile(GetOwner(pClonk), iKiller) && !GetCrewCount(GetOwner(pClonk)))
	{
		TeamKilled[GetOwner(pClonk)] = iKiller + 1; //Der Teamkiller + 1
		return true;
	}
	iKills[iKiller]++;
	SetScoreboardData(playerID, 2, Format("%d", iKills[iKiller]));
}

protected func InitializePlayer(int player)
{
	var playerID = GetPlayerID(player);
	SetScoreboardData(SBRD_Caption, 1, "{{SF5B}}", 1);
	SetScoreboardData(SBRD_Caption, 2, "{{FT5B}}", 1);
	SetScoreboardData(SBRD_Caption, 3, "{{HG5B}}", 1);
	SetScoreboardData(playerID, 1, Format("%d", Relaunches()), Relaunches());
	SetScoreboardData(playerID, 2, "0");
	SetScoreboardData(playerID, 3, "0");
	SetScoreboardData(SBRD_Caption, SBRD_Caption, "Score", SBRD_Caption);
	SetScoreboardData(playerID, SBRD_Caption, GetTaggedPlayerName(player), playerID);
	SortScoreboard(1, true);
	DoScoreboardShow(1, player + 1);
	szPlayerNames[player] = GetPlayerName(player);
	var clonk = GetCrew(player);
	PositionClonk(clonk);
	StartMaterial(clonk);
	return 1;
}

protected func PositionClonk(pClonk)
{
	var X, Y;
	RespawnPoints(X, Y);
	SetPosition(X, Y, pClonk);
	if (SpawnProtection())
		AddEffect("SpawnProtect", pClonk, 20, 1);
	return true;
}

protected func RelaunchPlayer(int player)
{
	iDeaths[player]++;
	var relaunch = Relaunches() - iDeaths[player];
	var playerID = GetPlayerID(player);
	if (relaunch < 0)
		return;
	SetScoreboardData(playerID, 1, Format("%d", relaunch), relaunch);
	SortScoreboard(1, true);
	DoScoreboardShow(1, player + 1);
	var clonk = CreateObject(SF5B, 0, 0, player);
	MakeCrewMember(clonk, player);
	SelectCrew(player, clonk, 1);
	PositionClonk(clonk);
	StartMaterial(clonk);
	//var Killer = TeamKilled[player];
	/*
  if(Killer > 0)
   SlayMenu(clonk);
  */
	return 1;
}

protected func SlayMenu(pClonk)
{
	var pPlayer = GetOwner(pClonk);
	var iKiller = TeamKilled[pPlayer] - 1;
	if (iKiller < 0)
		return false;
	CreateMenu(SKUL, pClonk, pClonk, 0, Format("$WhatToDo$", GetTaggedPlayerName(iKiller)), , , , SM1C);
	AddMenuItem("$Forgive$", "Forgive", BIRD, pClonk, , pPlayer, "$Forgive$");
	AddMenuItem("$Kill$", "Slay", SKUL, pClonk, , pPlayer, "$Kill$");
	SelectMenuItem(0, pClonk);
	Schedule(Format("CloseSlayMenu(%d)", pPlayer), 500);
}

global func CloseSlayMenu(pPlayer)
{
	if (GetCursor(pPlayer) && GetMenu(GetCursor(pPlayer)) == SM1C)
		CloseMenu(GetCursor(pPlayer));
	TeamKilled[pPlayer] = 0;
	return true;
}

global func Forgive(ID, pPlayer)
{
	TeamKilled[pPlayer] = 0;
	var PlayerName = GetTaggedPlayerName(pPlayer);
	Log("$Forgiven$", PlayerName);
	return true;
}

global func Slay(ID, pPlayer)
{
	var TeamKiller = TeamKilled[pPlayer] - 1;
	TeamKilled[pPlayer] = 0;
	if (TeamKiller < 0)
		return false;
	var SlayHim = GetCursor(TeamKiller);
	DoEnergy(-100, SlayHim, false, FX_Call_EngScript, 0);
	var PlayerName = GetTaggedPlayerName(pPlayer);
	Log("$Slayed$", PlayerName);
	return true;
}

protected func OnGameOver()
{
	ScriptGo(0);
	var b = iKills[0]; //b erstellen
	var iPlrNum = 0; //Spielernummer für GetTaggedPlayerName erstellen
	for (var a = 1; a < GetLength(iKills); a++)
	{
		if (b < iKills[a])
		{
			b = iKills[a];
			iPlrNum = a;
		}
	}
	var SuicideKing, Counter;
	for (var i = 0; i < GetLength(iSuicides); i++)
	{
		if (iSuicides[i] > Counter)
		{
			SuicideKing = i;
			Counter = iSuicides[i];
		}
	}
	if (NoMessages())
		return true;
	Log("$DamageDealer$", szPlayerNames[iPlrNum], b);
	if (Counter > 0)
		Log("$SuicideKing$", szPlayerNames[SuicideKing], RGB(0, 255, 0), Counter);
	AddEvaluationData(Format("{{FT5B}} <c e30000> %v Kills, Damage Dealer! <c ffffff>{{FT5B}}", b), iPlrNum + 1);
	//Suizide sind nicht unbedingt so toll, dass sie da auch noch zur Schau gestellt werden müssen :/
	return 1;
}

protected func RemovePlayer(player)
{
	var playerID = GetPlayerID(player);
	SetScoreboardData(playerID, 1, "{{SKUL}}", 0);
	return 1;
}

// Spawnschutz
// Danke an Saturas, dass ich mir das kopieren durfte. Sonst hätte ich es selbst schreiben müssen :/
//--------------------------------------

global func FxSpawnProtectTimer(object pTarget, int iEffectNumber, int iEffectTime) 
{
	EffectVar(3, pTarget, iEffectNumber) += 5;
	if (EffectVar(0, pTarget, iEffectNumber) <= 0)
		EffectVar(1, pTarget, iEffectNumber) = 3;
	if (EffectVar(0, pTarget, iEffectNumber) >= 40)
		EffectVar(1, pTarget, iEffectNumber) = -3;
	if (Contained(pTarget))
		return 0;
	if (iEffectTime > 150)
	{
		SetClrModulation(RGB(255, 255, 255), pTarget);
		return -1;
	}
	CreateParticle("PSpark", GetX(pTarget) + Cos(EffectVar(3, pTarget, iEffectNumber), 10), GetY(pTarget) + Sin(EffectVar(3, pTarget, iEffectNumber), 10), 0, 0, 30, HSL(EffectVar(0, pTarget, iEffectNumber), 255, 128));
	CreateParticle("PSpark", GetX(pTarget) + Cos(EffectVar(3, pTarget, iEffectNumber) - 180, 10), GetY(pTarget) + Sin(EffectVar(3, pTarget, iEffectNumber) - 180, 10), 0, 0, 30, HSL(EffectVar(0, pTarget, iEffectNumber), 255, 128));
	SetClrModulation(HSL(EffectVar(0, pTarget, iEffectNumber), 255, 128), pTarget);
	EffectVar(0, pTarget, iEffectNumber) += EffectVar(1, pTarget, iEffectNumber);
	Extinguish(pTarget);
}

global func FxSpawnProtectDamage(object pTarget, int iEffectNumber, int iDmgEngy, int iCause) 
{
	return 0;
}
//--------------------------------------

//Spawnpunkt setzen per Maus
public func AddSpawnPoint(X, Y)
{
	// Neue Positionen hinzufügen
	RespawnX[GetLength(RespawnX)] = X;
	RespawnY[GetLength(RespawnY)] = Y;
	// Den X und Y array ausgeben
	Log("--------------------");
	Log("RespawnX = %v;", RespawnX);
	Log("RespawnY = %v;", RespawnY);
	// Damit man sich besser merken kann, wo man schon war
	for (var i = 0; i < GetLength(RespawnX); i++)
	{
		X = RespawnX[i];
		Y = RespawnY[i];
		CreateParticle("NoGravSpark", X, Y, 0, 0, 40, RGB(0, 255, 0));
	}
	return true;
}
