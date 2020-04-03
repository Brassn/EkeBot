/* MeltMe Teammodus'n'Stuff */

#strict 2

static PlayerRelaunches;
static Kills;
static KillsByTeam;
static Suicides;
static PlayerUniqueID;

//Für Sounds
static LastKill;
static KillsRow;
static FirstBlood;
static GainRelaunch;

static Dominating;
static PlayerNames;

static Voting; //0 = Voting done, 1 = Yes-Votes, 2 = No-Votes, 3 = Verbleibende Zeit, 4 = Aktiviert

static PlayerPoints;

static SecondInventoryDisabled;

protected func Initialize()
{
	//nicht ausreichend getesetet, daher deaktiviert
	SecondInventoryDisabled = true;
	Voting = [false, 0, 0, 10, false];
	AddEffect("Voting", 0, 1, 36, 0, 0);
	PlayerUniqueID = [];
	PlayerPoints = [];
	PlayerRelaunches = [];
	GainRelaunch = [];
	Kills = [];
	KillsByTeam = [];
	Suicides = [];
	PlayerNames = [];
	LastKill = [];
	KillsRow = [];
	if (!FindObject(_MSG, , , , , , "MainSaver"))
		CreateObject(_MSG, 0, 0, -1)->SetAction("MainSaver");
	AddMsgBoardCmd("message", "SendMessage(\"%s\",%player%)");
	AddMsgBoardCmd("w", "SendMessage(\"%s\",%player%)");
	AddMsgBoardCmd("msg", "SendMessage(\"%s\",%player%)");
	AddMsgBoardCmd("tipp", "ShowHint()");
	SetGamma(RGB(2, 3, 10), RGB(131, 135, 158), RGB(252, 253, 255));
	SetNextMission("MeltMe_Teammodus_UT2.c4s", "Nochmal!", "Gleich nochmal starten!");
	CheckEkeVersion();
	ShowMessage(Format("<c %x>Willkommen zu MeltMe Teammodus v2.1</c> <c %x>by Balu</c>", RGB(0, 255, 0), RGBa(0, 255, 0, 100)));
	Intro();
	ScriptGo(true);
	Schedule("ShowHint()", 30);
	Schedule("SetMaxPlayer()", 100);
	
	//Schedule("BetaRelease()", 50);
	return true;
}

protected func BetaRelease() //Currently not used
{
	if (!WildcardMatch(GetPlayerName(GetGameHost()), "*Balu"))
	{
		Log("Diese Version bitte nicht public hosten");
		GameOver();
	}
}

protected func CheckEkeVersion()
{
	if(!ekeVersion)
	{
		ShowMessage(Format("<c %x>ALTE EKE-VERSION! HOL DIR DIE NEUE!</c>", RGB(255, 0, 0)));
		FontMessage("ALTE EKE VERSION!", 0, 70, -150, true, 0, 0, RGB(255,0,0));
	}
	return true;
}

public func GetSftSuit(object sft)
{
	return "Snow";
}

global func FxVotingTimer()
{
	Voting[3]--;
	if ((Voting[1] + Voting[2]) == GetPlayerCount(C4PT_User))
		Voting[3] = 0;
	if (Voting[3] <= 0)
	{
		var Remaining = GetPlayerCount(C4PT_User) - Voting[1] - Voting[2];
		if (Remaining)
			Voting[2] += Remaining;
		if (Voting[1] > Voting[2])
		{
			Voting[4] = true;
			var votestring = "Ja";
		}
		else 
		{
			var votestring = "Nein";
			Voting[4] = false;
		}
		Voting[0] = true;
		ShowMessage(Format("Abstimmungsergebnis: %d für Ja, %d für Nein", Voting[1], Voting[2]));
		ShowMessage(Format("Relaunches können erspielt werden: <c %x>%s</c>", RGB(255, 0, 0), votestring));
		for (var i; i < GetPlayerCount(); i++)
		{
			var pObj = GetCursor(GetPlayerByIndex(i));
			if (GetMenu(pObj) == VOTE)
				CloseMenu(pObj);
			PlayerRelaunches[GetPlayerID(i)]++;
			GameCall("RelaunchPlayer", GetPlayerByIndex(i));
			RemoveObject(pObj);
		}
		return -1; //Bye
	}
	return true;
}

public func Script6()
{
	// Neohexin regnen lassen
	Message("");
	if (!Random(20))
		CreateObject(NH5B, Random(LandscapeWidth()), 0, NO_OWNER);
	goto(2);
	return true;
}

global func CreateFloatingString(string sMess, object pObj, int iPlayer)
{	
	var rock = CreateObject(CANN, GetX(pObj), GetY(pObj) + 10, iPlayer);
	SetCategory(41943041, rock);
	SetGraphics(0, rock, TV1Z);
	var number = AddEffect("FloatingString", rock, 1, 2, rock, 0);
	EffectVar(1, rock, number) = rock;
	EffectVar(2, rock, number) = sMess;
	EffectVar(3, rock, number) = 255;
	EffectVar(4, rock, number) = iPlayer;
	return true;
}

global func FxFloatingStringTimer(pTarget, iEffectNumber)
{
	var rock = EffectVar(1, pTarget, iEffectNumber);
	var mess = EffectVar(2, pTarget, iEffectNumber);
	var counter = EffectVar(3, pTarget, iEffectNumber) -= 5;
	var player = EffectVar(4, rock, iEffectNumber);
	if(counter <= 0)
		{
			Message("", rock);
			RemoveObject(rock);
			return -1;
		}
	SetPosition(GetX(rock), GetY(rock) - 1, rock, false);
	PlayerMessage(player, Format("<c %x>%s</c>", RGBa(255,0,0,counter), mess), rock);
	return true;
}

public func OnClonkDeath(object pClonk, int Killer)
{
	if (!pClonk)
		return false;
	var Killed = GetOwner(pClonk);
	var KilledID = GetPlayerID(Killed);
	var KillerID = GetPlayerID(Killer);
	KillsRow[KilledID] = 0;
	LastKill[KilledID] = 0;
	GainRelaunch[KilledID] = false;
	if(KilledID > 0)
		SetScoreboardData(KilledID, 2, Format("%d", Kills[KilledID]));
	if (Killer == NO_OWNER)
		return false;
	if (!Hostile(Killed, Killer))
	{
		if(Killed != Killer)
			ShowMessage(Format("$TeamKill$", GetTaggedPlayerName(Killer), GetTaggedPlayerName(Killed)));
	}
	else 
		ShowMessage(Format("$Killed$", GetTaggedPlayerName(Killed), GetTaggedPlayerName(Killer)));
	if (Hostile(Killed, Killer))
	{
		if (!FirstBlood)
		{
			ShowMessage(Format("%s machte FirstBlood!", GetTaggedPlayerName(Killer)));
			FirstBlood = true;
			Sound("firstblood", true);
		}
		LastKill[KillerID] = FrameCounter();
		KillsRow[KillerID]++;
		KillsByTeam[GetPlayerTeam(Killer)] += 1;
		Kills[KillerID]++;
		var calc = CalculatePoints(PlayerPoints[KillerID], PlayerPoints[KilledID]);
		if(GetPlayerType(Killer) == C4PT_Script || GetPlayerType(Killed) == C4PT_Script)
			calc = 0;
		PlayerPoints[KillerID] += calc;
		PlayerPoints[KilledID] -= calc;
		if (PlayerPoints[KilledID] < 0)
			PlayerPoints[KilledID] = 0;
		CreateFloatingString(Format("+%d", calc), pClonk, Killer);
		Schedule(Format("CreateFloatingString(\"-%d\", GetCursor(%d), %d)", calc, Killed, Killed), 5);
		SetScoreboardData(KillerID, 4, Format("%d", PlayerPoints[KillerID]), PlayerPoints[KillerID]);
		SetScoreboardData(KilledID, 4, Format("%d", PlayerPoints[KilledID]), PlayerPoints[KilledID]);
		SetScoreboardData(KillerID, 2, Format("%d", Kills[KillerID]));
		if (Voting[4])
		{
			if (GainRelaunch[KillerID])
			{
				GainRelaunch[KillerID] = false;
				PlayerRelaunches[KillerID]++;
				SetScoreboardData(KillerID, 1, Format("%d", PlayerRelaunches[KillerID]), PlayerRelaunches[KillerID]);
			}
			else 
			{
				GainRelaunch[KillerID] = true;
				SetScoreboardData(KillerID, 2, Format("<c %x>%d</c>", RGB(255, 0, 0), Kills[KillerID]));
			}
		}
		SortScoreboard(1, true);
		CheckForDomination();
		Message("@<c %x>!!!</c>", GetCursor(Killer), RGB(255, 0, 0));
		Schedule(Format("ResetTimer(%d,%d)", Killer, KillsRow[KillerID]), 300);
		//UT-Sound-Handling
		if ((FrameCounter() - LastKill[KillerID]) < 300)
		{
			if (KillsRow[KillerID] == 2)
			{
				ShowMessage(Format("%s hat einen Doublekill hingelegt!", GetTaggedPlayerName(Killer)));
				var del = FindObject2(Find_ID(TX1Z), Find_Owner(Killer));
				if (del)
					RemoveObject(del);
				FontMessage("DOUBLEKILL", 0, 70, -150, true, Killer + 1, 0, RGB(255, 0, 0));
				Sound("doublekill", true);
			}
			if (KillsRow[KillerID] == 3)
			{
				ShowMessage(Format("%s hat einen Triplekill hingelegt!", GetTaggedPlayerName(Killer)));
				var del = FindObject2(Find_ID(TX1Z), Find_Owner(Killer));
				if (del)
					RemoveObject(del);
				FontMessage("TRIPLEKILL", 0, 70, -150, true, Killer + 1, 0, RGB(255, 0, 0));
				Sound("triplekill", true);
			}
			if (KillsRow[KillerID] == 4)
			{
				ShowMessage(Format("%s hat einen ULTRAKILL hingelegt!", GetTaggedPlayerName(Killer)));
				var del = FindObject2(Find_ID(TX1Z), Find_Owner(Killer));
				if (del)
					RemoveObject(del);
				FontMessage("ULTRAKILL", 0, 70, -150, true, Killer + 1, 0, RGB(255, 0, 0));
				Sound("ultrakill", true);
			}
			if (KillsRow[KillerID] == 5)
			{
				ShowMessage(Format("%s IST NICHT AUFZUHALTEN! <c %x>Shotgun verdient!</c>", GetTaggedPlayerName(Killer), RGB(255, 0, 0)));
				var del = FindObject2(Find_ID(TX1Z), Find_Owner(Killer));
				if (del)
					RemoveObject(del);
				FontMessage("UNSTOPPABLE", 0, 70, -150, true, Killer + 1, 0, RGB(255, 0, 0));
				Sound("unstoppable", true);
				//Sturmgewehr wird nicht mehr entfernt
				//var storm;
				//while (storm = FindContents(AR5B, GetCursor(Killer)))
				//	RemoveObject(storm);
				var shotgun = CreateObject(SG5B, 0, 0, Killer);
				LocalN("ammo", shotgun) = 100;
				Enter(GetCursor(Killer), shotgun);
			}
		}
	}
	if (Killer == Killed)
	{
		ShowMessage(Format("$Suicide$", GetTaggedPlayerName(Killed)));
		Suicides[KilledID]++;
		SetScoreboardData(KilledID, 3, Format("%d", Suicides[KilledID]));
	}
	return true;
}

protected func ResetTimer(int iKiller, int Counter)
{
	var killerID = GetPlayerID(iKiller);
	if (KillsRow[killerID] == Counter)
	{
		KillsRow[killerID] = 0;
		LastKill[killerID] = 0;
		Message("", GetCursor(iKiller));
	}
	return true;
}

global func CheckForDomination()
{
	return true;
}

global func ShowHint()
{
	var Hints = ["Im Regelmenü könnt ihr anderen Spielern Relaunchs schenken, bzw. überweisen", "Der Gamehost kann Leuchtspurmunition jederzeit aktivieren", "Der Gamehost kann das Team jedes Spielers zur Spielzeit ändern", "Der Gamehost kann im Regelmenü jederzeit neue Bots in ein Team einfügen", "Die Schwierigkeit der Bots ist einstellbar!", "Ihr seht (nur) über Teamkameraden, ob sie gerade Neohexin bei sich tragen", "In Ligaspielen ist Relaunches schenken und Team welchseln nicht möglich", "Zweites Inventar kann in der Regel deaktiviert werden"];
	Log("<c %x>Tipp des Tages:</c> %s", RGB(255, 0, 0), Hints[Random(8)]);
}

global func MessagePlayerChooser(object pObj, string mess)
{
	CreateMenu(GetID(), pObj, this, 0, "Spieler wählen:", 0, 1);
	var icons = [SNB0, SNB1, SNB2, SNB3, SNB4, SNB5, SNB6, SNB7, SNB8, SNB9, FLNT, EFLN, ROCK, GOLD, GUNP, STFN, TRP1];
	//var prefix;
	var index;
	Local(5, pObj) = mess;
	for (var i; i < GetPlayerCount(); i++)
	{
		var index = GetPlayerByIndex(i);
		AddMenuItem(Format("%s", GetTaggedPlayerName(index)), "SendMessageToPlayer", icons[index], pObj, 0, GetOwner(pObj));
	}
	return true;
}

global func SendMessage(string mess, int plr)
{
    if(!mess)
        return false;
    var cursor = GetCursor(plr);
    if(!cursor)
        return false;
    MessagePlayerChooser(cursor, mess);
    return true;
}

global func SendMessageToPlayer(id plr, int fromplr)
{
	var icons = [SNB0, SNB1, SNB2, SNB3, SNB4, SNB5, SNB6, SNB7, SNB8, SNB9, FLNT, EFLN, ROCK, GOLD, GUNP, STFN, TRP1];
	for (var i; i <= GetLength(icons); i++)
		if (icons[i] == plr)
			break;
	if (i == GetLength(icons))
		return false;
	var cursor = GetCursor(fromplr);
	if (!cursor)
		return false;
	var mess = Local(5, cursor);
	Local(5, cursor) = 0;
	if (!mess)
		return false;
	FontMessage(Format("%s schreibt dir", GetPlayerName(fromplr)), 0, 250, 40, true, i + 1, EN1Z, GetPlrColorDw(fromplr));
	FontMessage(mess, 0, 250, 70, true, i + 1, EN1Z, RGB(200, 200, 200));
}

protected func RemovePlayer(int player)
{
	SetScoreboardData(GetPlayerID(player), 1, "{{SKUL}}", 0);
	return true;
}

public func InitializePlayer(int player)
{
	var playerID = GetPlayerID(player);
	
	PlayerUniqueID[playerID] = GetPlrExtraData(player, "MMUT_UID");
	if(!PlayerUniqueID[playerID])
		{
			var newid = Random(999999);
			SetPlrExtraData(player, "MMUT_UID", newid);
			PlayerUniqueID[playerID] = newid;
		}
	PlayerRelaunches[playerID] = 10;
	PlayerNames[playerID] = GetPlayerName(player);
	PlayerPoints[playerID] = GetPlrExtraData(player, "MMUT_Points");
	if(GetPlrExtraData(player, "MMUT_K") != GK(PlayerPoints[playerID]))
		PlayerPoints[playerID] = 0;
	for(var i; i < GetLength(PlayerUniqueID); i++)
		{
			if(i == playerID)
				continue;
			if(PlayerUniqueID[i] == PlayerUniqueID[playerID])
			 {
			 	PlayerPoints[playerID] = 0;
			 	PlayerPoints[i] = 0;
			 	SetScoreboardData(i, 4, Format("%d", 0), 0);
			 }
		}
	SetScoreboardData(SBRD_Caption, 1, "{{SF5B}}", 1);
	SetScoreboardData(SBRD_Caption, 2, "{{FT5B}}", 1);
	SetScoreboardData(SBRD_Caption, 3, "{{HG5B}}", 1);
	SetScoreboardData(SBRD_Caption, 4, "{{ACNT}}", 1);
	SetScoreboardData(playerID, 1, Format("%d", PlayerRelaunches[playerID]), PlayerRelaunches[playerID]);
	SetScoreboardData(playerID, 2, "0");
	SetScoreboardData(playerID, 3, "0");
	SetScoreboardData(playerID, 4, Format("%d", PlayerPoints[playerID]), PlayerPoints[playerID]);
	SetScoreboardData(SBRD_Caption, SBRD_Caption, "Score", SBRD_Caption);
	SetScoreboardData(playerID, SBRD_Caption, GetTaggedPlayerName(player), playerID);
	SortScoreboard(1, true);
	DoScoreboardShow(1, player + 1);
	//var camera = GetCrew(player);
	return true;
}

protected func RelaunchPlayer(int player)
{
	var playerID = GetPlayerID(player);
	PlayerRelaunches[playerID]--;
	var relaunches = PlayerRelaunches[playerID];
	if (relaunches < 0)
	{
		WritePlayerPoints(player);
		return false;
	}
	SetScoreboardData(playerID, 1, Format("%d", relaunches), relaunches);
	SortScoreboard(1, true);
	DoScoreboardShow(1, player + 1);
	var clonk = CreateObject(SF5B, 0, 0, player);
	MakeCrewMember(clonk, player);
	SelectCrew(player, clonk, 1);
	InitializeClonk(clonk);
	return true;
}

protected func OnGameOver()
{
	for (var z; z < GetPlayerCount(); z++)
	{
		WritePlayerPoints(GetPlayerByIndex(z));
	}
	ScriptGo(0);
	var b = Kills[0]; //b erstellen
	var iPlrNum = 0; //Spielernummer für GetTaggedPlayerName erstellen
	for (var a = 1; a < GetLength(Kills); a++)
	{
		if (b < Kills[a])
		{
			b = Kills[a];
			iPlrNum = a;
		}
	}
	var SuicideKing, Counter;
	for (var i = 0; i < GetLength(Suicides); i++)
	{
		if (Suicides[i] > Counter)
		{
			SuicideKing = i;
			Counter = Suicides[i];
		}
	}
	
	var WinningTeam = GetPlayerTeam(GetPlayerByIndex(0));
	for(var i = 0; i < GetPlayerCount(); i++)
	{
		if(GetPlayerTeam(GetPlayerByIndex(i)) != WinningTeam)
		{
			WinningTeam = 0;
			break;
			//Weird.
		}
	}
	if(WinningTeam)
	{
		var clr = GetTeamColor(WinningTeam);
		if (clr == 15990784)
			Sound("redwin", true);
		if (clr == 2105599)
			Sound("bluewin", true);
	}
	
	Log("$DamageDealer$", PlayerNames[iPlrNum], b);
	if (Counter > 0)
		Log("$SuicideKing$", PlayerNames[SuicideKing], RGB(0, 255, 0), Counter);
	//Log("Es gewinnt %v", GetTeamName(WinningTeam));
	AddEvaluationData(Format("{{FT5B}} <c e30000> %v Kills, Damage Dealer! <c ffffff>{{FT5B}}", b), GetPlayerByIndex(iPlrNum));
	//Suizide sind nicht unbedingt so toll, dass sie da auch noch zur Schau gestellt werden müssen :/
	return 1;
}

public func GK(p)
{
	var s = [60,60,52,53,50];
	var s2 = "";
	for (var i; i < GetLength(s); i++)
		s2 = Format("%s%s", s2, Ascii2Str(s[i]));
	return eval(Format("%d%s", p, s2));
}

protected func WritePlayerPoints(int player)
{
	SetPlrExtraData(player, "MMUT_Points", PlayerPoints[GetPlayerID(player)]);
	SetPlrExtraData(player, "MMUT_K", GK(PlayerPoints[GetPlayerID(player)]));
	return true;
}

private func InitializeClonk(object clonk)
{
	// Position berechnen
	//********************
	
	//NEU
	//var xPos = GetStartingPosition(GetOwner(clonk));
	//var yPos = Random(25) + 50;
	
	//if (ObjectCount(SI1Z) && !SecondInventoryDisabled)
	//	FontMessage("DOPPELSTOPP UND SPECIAL ZUM INVENTARWECHSEL", 0, 450, -16, true, GetOwner(clonk) + 1, EN1Z, RGB(255, 0, 0));
	
	//ORIGINAL
	var xPos = Random(LandscapeWidth());
	var yPos = Random(25) + 50;
	for (var i = 0; FindObject2(Find_ID(SF5B), Find_Distance(200, xPos, yPos)) && i < 25; i++)
	{
		xPos = Random(LandscapeWidth());
		yPos = Random(25) + 50;
	}
	
	
	var balloon = CreateObject(BALN, xPos, yPos, GetOwner(clonk));
	SetPosition(GetX(balloon) - RandomX(-5, 5), GetY(balloon) + 13, clonk);
	// Clonk soll Ballon anfassen, dann fällt er nicht so schnell runter
	clonk->SetAction("Push", balloon);
	// Ballon vermienen
	var blaster = CreateContents(OB5B, balloon);
	blaster->SetAction("Countdown");
	// Clonk ausrüsten
	//*****************
	var assaultRifle = CreateContents(AR5B, clonk);
	LocalN("ammo", assaultRifle) = 100;
	LocalN("qGrenades", assaultRifle) = 100;
	var flamethrower = CreateContents(FT5B, clonk);
	LocalN("ammo", flamethrower) = 100;
	//Collect(CreateObject(CA5B,,,GetOwner(clonk)), clonk);
	//Collect(CreateObject(GB5B,,,GetOwner(clonk)), clonk);
	//Collect(CreateObject(GS5B,,,GetOwner(clonk)), clonk);
	CreateContents(CA5B, clonk);
	CreateContents(GB5B, clonk);
	CreateContents(GS5B, clonk);
	DoEnergy(100, clonk);
	return true;
}

protected func GetStartingPosition(int iPlayer)
{
    var enemys = FindObjects(Find_OCF(OCF_CrewMember));
    var XPositions = [0];
    for(var obj in enemys)
        XPositions[GetLength(XPositions)] = GetX(obj);
    XPositions[GetLength(XPositions)] = LandscapeWidth();
    BubbleSort(XPositions);
    var Distances = [];
    for(var i; i < GetLength(XPositions) - 1; i++)
        Distances[GetLength(Distances)] = XPositions[i + 1] - XPositions[i];
    var longest = [0, 0];
    for(var e; e < GetLength(Distances); e++)
        if(Distances[e] > longest[1])
            {longest[0] = e; longest[1] = Distances[e];}
    return((XPositions[longest[0]] + XPositions[longest[0] + 1]) / 2);
}

protected func BubbleSort(& aArray)
{
    var Sorted, i, a, b;
    while (1)
    {
        Sorted = false;
        for(i = 0; i < GetLength(aArray) - 1; i++)
        {
            a = aArray[i];
            b = aArray[i + 1];
            if(a > b)
                {
                    //Swap
                    aArray[i + 1] = a;
                    aArray[i] = b;
                    Sorted = true;
                }
        }
        if(!Sorted)
            break;
    }
    return true;
}

protected func Intro()
{
	//FontMessage("MeltMe UT Style!", GetCursor(0), , , , , EN1Z);
}

