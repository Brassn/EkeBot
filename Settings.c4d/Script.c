/*-- Bot Einstellungen --*/

#strict 2

/*
static const EB_Easy = 5;
static const EB_Medium = 3;
static const EB_Hard = 0;
static EB_Difficulty;
*/

local ExtraDataSet;

static EB_Difficulty;
static EB_EnableDebugging;

protected func Initialize()
{
	if (!FindObject2(Find_ID(SF5B), Find_Func("IsComputerControlled")))
		return false;
	ExtraDataSet = true;
	for (var iPlr = 0; iPlr < GetPlayerCount(); iPlr++)
		if (GetPlayerType(iPlr) == C4PT_User)
			SetPlrExtraData(iPlr, "EB_TimesPlayed", GetPlrExtraData(iPlr, "EB_TimesPlayed") + 1);
	if (!EB_Tactic)
		GetTacticTemplate();
	return true;
}

public func BotJoined()
{
	if (!ExtraDataSet)
		Initialize();
	ExtraDataSet = true;
	return false;
}

protected func InitializePlayer(int iPlr)
{
	if (GetPlayerType(iPlr) == C4PT_Script && !ExtraDataSet)
	{
		BotJoined();
		return true;
	}
	if (FindObject2(Find_ID(SF5B), Find_Func("IsComputerControlled")))
	{
		if (GetPlayerType(iPlr) == C4PT_User)
			SetPlrExtraData(iPlr, "EB_TimesPlayed", GetPlrExtraData(iPlr, "EB_TimesPlayed") + 1);
		return true;
	}
}

protected func Activate(int iPlayer)
{
	var pClonk = GetCursor(iPlayer);
	CreateMenu(GetID(), pClonk, this, 0, "$BotSettings$");
	if (iPlayer == GetGameHost())
	{
		AddMenuItem("$Difficulty$", "DifficultyMenu", RSR6, pClonk, , iPlayer, "$Difficulty$");
		AddMenuItem("$TimesPlayed$", "ChoosePlayer", RMMG, pClonk, , , "");
		AddMenuItem("$AddBots$", "AddBotMenu", AR1C, pClonk, , , "");
		//REINE DEBUG GRÜNDE, daher auch nicht ohne deutliche Warnung aktivierbar.
		if (WildcardMatch(GetPlayerName(GetGameHost()), "*Balu"))
			AddMenuItem("ToggleDebug", "ToggleDebugMode", CICA, pClonk, , 1);
	}
	else 
		AddMenuItem("$TimesPlayed$", "TPOutput", RMMG, pClonk, , iPlayer, "");
	return true;
}

protected func AddBotMenu()
{
	var pClonk = GetCursor(GetGameHost());
	CreateMenu(GetID(), pClonk, this, 0, "$AddBots$", , , true);
	AddMenuItem("$AddBot$", "AddBot", AR1C, pClonk, , , "");
	for (var i; i < GetTeamCount(); i++)
		AddMenuItem(Format("$AddBotTeam$", GetTeamName(GetTeamByIndex(i))), "AddBot", AR1C, pClonk, , GetTeamByIndex(i), "");
	return true;
}

protected func AddBot(ID, int Team)
{
	var BotNames = ["Good old Schorsch", "Destructor", "Karl von Hinten", "Snake", "MadBurn", "Steelwolf", "Sam Solidfist", "Rex Hammerschlag", "Joe Backbraker", "Major Bob", "Terminator"];
	var Name = BotNames[Random(GetLength(BotNames))]; //Random it!
	SetMaxPlayer(GetPlayerCount() + 1);
	CreateScriptPlayer(Name, RGB(Random(255), Random(255), Random(255)), Team);
	return true;
}

protected func ChoosePlayer()
{
	var pClonk = GetCursor(GetGameHost());
	CreateMenu(GetID(), pClonk, this, 0, "$ChoosePlayer$");
	for (var iPlr = 0; iPlr < GetPlayerCount(); iPlr++)
		if (GetPlayerType(iPlr) == C4PT_User)
			AddMenuItem(Format("%s", GetTaggedPlayerName(iPlr)), "TPHostView", SF5B, pClonk, , iPlr, "");
	return true;
}

protected func TPHostView(ID, int iPlayer)
{
	var pClonk = GetCursor(GetGameHost());
	var TP = GetPlrExtraData(iPlayer, "EB_TimesPlayed");
	if (ExtraDataSet)
		TP--;
	if (TP < 0)
		TP = 0;
	PlayerMessage(GetGameHost(), "$TPHostView$", pClonk, GetTaggedPlayerName(iPlayer), TP);
	return true;
}

protected func TPOutput(ID, int iPlayer)
{
	var pClonk = GetCursor(iPlayer);
	var TP = GetPlrExtraData(iPlayer, "EB_TimesPlayed");
	if (ExtraDataSet)
		TP--;
	if (TP < 0)
		TP = 0;
	Message("$TPOutput$", pClonk, TP);
	return true;
}

protected func DifficultyMenu(ID, int iPlayer)
{
	if (!ExtraDataSet)
	{
		MessageWindow("$NoBots$", iPlayer, RSR6);
		return false;
	}
	var pClonk = GetCursor(iPlayer);
	CreateMenu(GetID(), pClonk, this, 0, "$BotSettings$");
	AddMenuItem("$Hard$", "SetDifficulty", AR5B, pClonk, , EB_Hard);
	AddMenuItem("$Medium$", "SetDifficulty", UZ5B, pClonk, , EB_Medium);
	AddMenuItem("$Easy$", "SetDifficulty", PT5B, pClonk, , EB_Easy);
	return true;
}

protected func ToggleDebugMode(ID, Trash)
{
	EB_EnableDebugging = !EB_EnableDebugging;
	if (EB_EnableDebugging)
	{
		AddEffect("Debug", this, 1, 5, this);
		Log("Debug enabled!");
	}
	else
		Log("Debug disabled!");
	return true;
}

protected func FxDebugTimer()
{
	//var dir;
	if(!EB_EnableDebugging)
		return -1;
	var sfts = FindObjects(Find_ID(SF5B), Find_OCF(OCF_CrewMember));
	var pObj;
	for (pObj in sfts)
	{
		if(GetPlayerType(GetOwner(pObj)) == C4PT_User)
			if(pObj == GetCursor(GetOwner(pObj)))
				pObj ->~ ByGrenadeFireNew(GetDir(pObj), true);
	}
	return true;
}

protected func SetDifficulty(ID, NewDifficulty)
{
	EB_Difficulty = NewDifficulty;
	LogDifficulty();
	return true;
}

protected func LogDifficulty()
{
	if (EB_Difficulty == EB_Easy)
		Log("$Log$", "$Easy$");
	if (EB_Difficulty == EB_Medium)
		Log("$Log$", "$Medium$");
	if (EB_Difficulty == EB_Hard)
		Log("$Log$", "$Hard$");
	return true;
}

//Weitergabe von Killdaten an Bots:
public func OnClonkDeath(object pClonk, int iKiller)
{
	if (GetPlayerType(iKiller) != C4PT_Script)
		return true;
	//Bot finden
	var x = AbsX(GetX(pClonk));
	var y = AbsY(GetY(pClonk));
	var bot = FindObject2(Find_Distance(500, x, y), Find_ID(SF5B), Find_Owner(iKiller), Find_OCF(OCF_CrewMember), Sort_Distance(x, y));
	if (bot)
		bot->~KilledClonk(pClonk);
	return true;
}

//AI Director:
//Der AI Director ist in Tactictemplates aufgebaut. Diese können per Szenarioscript gesetzt werden,
//aber es wird versucht das Richtige selbst herauszufinden
//Inhalt eines Templates:
//Was ist zu kaufen? Rakten benötigt, steile Abgründe? -> Jetpack, usw.
//Wohin ist zu gehen? Controlpoints einnehmen, Gegner attackieren, usw.
//Erkennung eines Klassenauswahlmenüs: Bei vorgegebener Menü-ID random choice treffen.

//Fortan werden alle Taktikbezogenen Fragen durch den AI Director getroffen.
//Vorzüge: Übersichtlichkeit, leichtere Implementierung zusätzlicher Spielziele.

static EB_Tactic;

//1 = Standardmeele
//2 = ControlPoint

//Wird durch Bot aufgerufen, wenn er nichts zu tun hat. Return: Command-String
global func EB_GetCommand(object Bot, & Command, & pTarget, & iX, & iY, & pTarget2, & Data, & iRetries)
{
	if (!EB_Tactic)
		GetTacticTemplate();
	if (EB_Tactic == 1)
	{
		var Enemy = SearchEnemy(Bot);
		if (Enemy)
		{
			var LoR = GetX(Enemy) - GetX(Bot);
			if (LoR > 0)
				LoR = 1;
			else 
				LoR = -1;
			var Dist = 40 * LoR;
			Command = "MoveTo";
			iX = GetX(Enemy) + Dist;
			iY = GetY(Enemy);
			return true;
		}
	}
	return false;
}

global func SearchEnemy(object Bot)
{
	if (!Bot)
		return false;
	//var X = GetX(Bot);
	//var Y = GetY(Bot);
	//var dist = Find_Distance(1500, X, Y);
	var enemy = Find_Hostile(GetOwner(Bot));
	var ex = Find_Exclude(Bot);
	var ocf = Find_OCF(OCF_Alive);
	var cont = Find_NoContainer();
	var noliq = Find_Not(Find_Action("Swim"));
	var Threats = FindObjects(enemy, ex, ocf, cont, noliq, Sort_Distance());
	for (var pObj in Threats) 
	{
		LocalN("Enemy", Bot) = pObj; //Raketenabschuss muss noch überarbeitet werden
		return pObj;
	}
	return false;
}

global func GetTacticTemplate()
{
	var TI = GameCall("TacticIndicator");
	if (TI)
	{
		EB_Tactic = TI;
		return true;
	}
	EB_Tactic = 1;
	return true;
}
