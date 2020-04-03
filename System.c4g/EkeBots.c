// EKE Bot
// By Balu
// Some parts improved by Gurkenglas
// Ich kommentiere nur, wenn mir langweilig ist. Ansonsten schreib ich wohl leider nur unkommentiert.

#strict 2 //Vollständig ;)
#appendto SF5B

static const EB_FlameThrower = 2;
static const EB_AssaultRifle = 3;
static const EB_Uzi = 4;
static const EB_Pistol = 5;
static const EB_GrenadeFire = 6;
static const EB_HandGrenade = 7;
static const EB_Molotov = 8;
static const EB_Jetpack = 9;
static const EB_RocketLauncher = 10;
static const EB_AB_Bullet = 11;
static const EB_AB_Missile = 12;
static const EB_AB_Bomb = 13;

//Schwierigkeitsstufen
static const EB_Easy = 5;
static const EB_Medium = 3;
static const EB_Hard = 0;
static EB_Difficulty; //Aktueller Schwierigkeitsgrad

static EB_Blacklist; //Array, der nicht aufsammelbare Items speichert. Gilt für alle Bots
static EB_Tactic; //Siehe AI Director
static EB_EnableDebugging; //Aktiviert Debug Modus

local Enemy, ThrowingAllowed; //Hauptziel + Werfen erlaubt (Molotovs + Granaten)
local Position; //Array zur Stuck-Kontrolle
local ComputerControlled; //SFT ist Scriptgesteuert
local GuidingMissile; //Gerade gelenkte Rakete
local GuidingAimX,GuidingAimY; //Arrays fürs Raketen lenken [Wegpunkte System]
local Item, TimeUsed; //Item, das gerade versucht wird aufzusammeln + Zeit, die dafür genutzt wird es aufzusammeln
local SpecialAction; //Gibt an, ob der Bot gerade eine spezielle Aufgabe hat
static EB_BaseIDs;

static GameHost;

protected func Initialize() //Automatisches Initialisieren bei Script Spielern
{
	if (GetPlayerType(GetOwner()) == C4PT_Script)
		SetComputerControlled();
	if (!ObjectCount(ST1C))
		CreateObject(ST1C);
	inherited();
}

public func SetComputerControlled()
{
	if (ComputerControlled)
		return false;
	if (GetType(EB_BaseIDs) != C4V_Array)
	{
		EB_BaseIDs = [0];
		var id, i;
		while (id = GetDefinition(i, C4D_Structure))
		{
			if (id == BK5B)
				if (GetPlrKnowledge(GetOwner(), id))
				{
					EB_BaseIDs[0] = BK5B;
					i++;
					continue;
				}
			if (GetPlrKnowledge(GetOwner(), id))
				if (GetDefCoreVal("Base", "DefCore", id, 0))
					EB_BaseIDs[GetLength(EB_BaseIDs)] = id;
			i++;
		}
		if (!EB_BaseIDs[0])
			DeleteArrayItem(0, EB_BaseIDs);
	}
	ComputerControlled = true;
	AddEffect("Caller", this, 1, 3, this);
	AddEffect("SlowCaller", this, 1, 50, this);
	Position = CreateArray(3);
	Schedule("Baloon()", 1, 3); //Ballon Verhalten in MeltMe
	FxSlowCallerTimer(); //Quickstart
	return true;
}

public func IsComputerControlled()
{
	if (ComputerControlled)
		return true;
	return false;
}

protected func CheckStucked()
{
	if (!GetCommand() || GetCommand() == "Wait" || GetCommand() == "Build" || SpecialAction || IsOnAirbike())
		return true;
	if (Inside(Position[0], GetX() - 10, GetX() + 10))
	{
		if (Inside(Position[1], GetY() - 10, GetY() + 10))
		{
			Position[2]++;
			if (Position[2] == 1)
				if (CanFire(EB_Jetpack))
				{
					Jump();
					ShiftContents(, , JP5B);
					Schedule("Contents(0) ->~ControlThrow(this)", 5);
				}
			if (Position[2] == 3)
			{
				if (GetAction() == "Hangle")
					SetComDir([COMD_Left, COMD_Right][Random(2)]);
				if (GetAction() == "Scale")
					SetComDir(COMD_Up);
			}
			if (Position[2] == 4)
			{
				var Dir = 0;
				if (GetCommand())
					if (GetX() < GetCommand(, 2))
						Dir = 1;
				if (!GetCommand())
					Dir = GetDir();
				var SavedItem = Item;
				if (GetID(GetCommand(, 1)) == NH5B)
					AddCommand(this, "Dig", , GetX(GetCommand(, 1)), GetY(GetCommand(, 1)));
				else 
					SetCommand(this, "Dig", , GetX() + (200 * Dir - 100), GetY());
				Item = SavedItem;
			}
			/*
    if(Position[2] == 5 && Enemy) //Als MeltMe-Taktik
     if(EB_Tactic == 1)
      if(Inside(GetY(Enemy),GetY()-50,GetY()+50))
       if(!IsFiring() && !InLiquid())
        if(CanFire(EB_FlameThrower))
         if(ObjectDistance(Enemy) > 500)
          {
           var Dir;
           if(GetX() < GetX(Enemy))
            Dir = 1;
           if(GetCommand() != "Wait")
            AddCommand(this,"Wait",,,,,,300);
           SetComDir(Dir);
           ShiftContents(this,,FT5B);
           Contents(0)->ControlThrow(this);
           SpecialAction = true;
           return(true);
          }
    */
			if (Position[2] == 5)
				Jump();
		}
		else 
			Position = CreateArray(3);
	}
	else 
		Position = CreateArray(3);
	if (!Position[1])
	{
		Position[0] = GetX();
		Position[1] = GetY();
		Position[2] = 0;
		if (GetCommand() == "MoveTo")
			if (!Inside(GetCommand(, 2), 0, LandscapeWidth()))
				FinishCommand(this, true, 0);
	}
	return true;
}

local Timer;

protected func FxSlowCallerTimer()
{
	if (!Item && TimeUsed)
		TimeUsed = 0;
	if (GuidingMissile)
		return false;
	if (IsOnAirbike())
	{
		var Command, pTarget, iX, iY, pTarget2, Data, iRetries;
		if (EB_GetCommand(this, Command, pTarget, iX, iY, pTarget2, Data, iRetries))
			SetCommand(IsOnAirbike(), Command, pTarget, iX, iY, pTarget2, Data, iRetries);
		return true;
	}
	if (Contained() && GetID(Contained()) == _PLO)
		return false;
	if (Item)
	{
		TimeUsed++;
		InitBlacklist();
		if (TimeUsed > 20)
		{
			EB_Blacklist[GetLength(EB_Blacklist)] = Item;
			SetCommand(this, "");
			Item = 0;
			TimeUsed = 0;
		}
	}
	Timer++;
	if (Timer >= 3 && EB_Difficulty == EB_Hard)
	{
		ThrowingAllowed = true;
		Timer = 0;
	}
	var friend = FindObject2(Find_AtPoint(), Find_OCF(OCF_CrewMember), Find_Not(Find_Hostile(GetOwner())), Find_Exclude(this));
	if (GetCommand() == "MoveTo" && friend && GetCommand(friend) == "MoveTo")
	{
		AddCommand(this, "Wait", , , , , , 30); //Nicht direkt auf Freund laufen
		SetComDir(COMD_Stop);
	}
	TryToBuild();
	Shopping();
	CheckStucked();
	CheckTerrain();
	CheckContents();
	if (!GetCommand())
	{
		var Command, pTarget, iX, iY, pTarget2, Data, iRetries;
		if (EB_GetCommand(this, Command, pTarget, iX, iY, pTarget2, Data, iRetries))
			SetCommand(this, Command, pTarget, iX, iY, pTarget2, Data, iRetries);
		Item = 0;
	}
	if (!GetCommand() && Item)
		Item = 0;
	return 1;
}

protected func Baloon() //Behandelt den Ballon in MeltMe
{
	if (GetAction() == "Push" && GetID(GetActionTarget()) == BALN)
	{
		var Bal = GetActionTarget();
		SetCommand(this, "UnGrab"); //Loslassen
		AppendCommand(this, "Follow", Bal);
		ScheduleCall(this, "Baloon2", 100, , Bal);
		Holster();
		ScheduleCall(Contents(0), "ControlThrow", 10, 1, this);
		ScheduleCall(this, "Holster", 20, 1);
	}
}

protected func Baloon2(Bal)
{
	// Noch auf dem Ballon?
	var fOnBal;
	for (var e in FindObjects(Find_InRect(-20,-20,40,40))) 
	{
		if (e == Bal)
			fOnBal = true;
	}
	if (!fOnBal)
	{
		SetCommand(this, ""); //Zur Sicherheit!
		return false;
	}
	// Welche Richtung?
	var xr = GetX(), yr = GetY(), xDirr = GetPhysical("Walk") / 3500, yDirr = 0;
	var xl = xr, yl = yr, xDirl = -xDirr, yDirl = yDirr;
	var fRevert;
	SimFlight(xr, yr, xDirr, yDirr, 25);
	SimFlight(xl, yl, xDirl, yDirl, 25);
	if (GBackLiquid(xl, yl) || (!GBackLiquid(xr, yr) && yr > yl))
	{
		fRevert = true;
		xr = xl;
		yr = yl;
	}
	// Zielort des Ballons bestimmen
	var xBal = GetX(Bal), yBal = GetY(Bal);
	PathFree2(xBal, yBal, xBal + GetXDir(Bal) * 16, yBal + GetYDir(Bal) * 16);
	// Ziel des Bots noch zu explosionsgefährdet?
	while (Distance(xBal, yBal, xr, yr) < 100)
		xr += [1, -1][fRevert];
	// Alle Mann von Bord. 
	SetCommand(this, "MoveTo", , xr, yr);
	//Schedule("AppendCommand(this,\"Wait\",,,,,50)", 1);
}

protected func FxCallerTimer()
{
	if (GetMenu())
		CloseMenu();
	if (GuidingMissile)
	{
		GuideMissile(); //Mittlerweile wird gehandelt, falls Schaden einkassiert wird.
		return false;
	}
	if (Contained() && GetID(Contained()) == _PLO)
		return false;
	var bike;
	if(bike = IsOnAirbike())
	{
		if(GetXDir(bike) > 0)
			{
				SetDir(1, bike);
				SetDir(1, this);
			}
		else
			{
				SetDir(0, bike);
				SetDir(0, this);
			}
	}
	/*else
  if(Contained())
   SetCommand(this,"Exit");*/
	if (GetAction() == "Fight")
		ControlDig();
	if (InLiquid())
		WaterJump();
	CanGetOnAirbike(); //Evtl. auf Airbike steigen
	//Springen Flammen nahe sind.
	if (!OnFire() && GetAction() != "Jump")
		if (FindObject2(Find_Distance(13), Find_OCF(OCF_OnFire)))
			if (!IsFiring())
				Jump();
	if (EB_Difficulty <= EB_Medium)
		Attack();
	else if (!Random(10))
		Attack();
	var fFleeAbysm;
	if (GetProcedure() == "FLIGHT")
		if (Max(Abs(GetYDir() + 5), Abs(GetXDir())) > 70 || (GetScenBottomOpen() && LandscapeWidth() - GetY() < GetYDir() && (fFleeAbysm = true)))
			if (HasWeapon(EB_Jetpack))
			{
				if (!HasAmmo(EB_Jetpack) && FindContents(GS5B))
				{
					ShiftContents(, , JP5B);
					CheckArmed();
					Contents(0)->~Activate(this);
				}
				if (HasAmmo(EB_Jetpack))
				{
					ShiftContents(, , JP5B);
					CheckArmed();
					Contents(0)->~ControlThrow(this);
					if (fFleeAbysm)
						SetComDir(COMD_Up);
				}
			}
	if (GetAction() == "JetpackFly")
	{
		if (GetXDir() > 0)
			SetDir(1);
		if (GetXDir() < 0)
			SetDir(0);
	}
	return true;
}

protected func FxCallerDamage(object pObj, int Number, int Dmg, iCause)
{
	if (Dmg > 0)
		return Dmg;
	if (GuidingMissile)
	{
		Contents(0)->~ControlDig(this);
		GuidingMissile = 0; //Aufhören
	}
	if ((GetEnergy() * 1000 + Dmg) < 4)
	{
		if (FindContents(NH5B))
		{
			ShiftContents(, , NH5B, true);
			Contents(0)->~Activate(this);
		}
		else 
		{
			var pAim = FindObject2(Find_Hostile(GetOwner()), Find_OCF(OCF_Alive), Find_Distance(250), Sort_Distance());
			var pFriend = FindObject2(Find_Not(Find_Hostile(GetOwner())), Find_Exclude(this), Find_OCF(OCF_Alive), Find_Distance(150), Sort_Distance());
			if (pAim && !pFriend)
				Suicide();
		}
	}
	if (!Random(EB_Difficulty) && (GetEnergy() * 1000 + Dmg) < 30000)
		if (ShiftContents(, , NH5B, true))
			Contents(0)->~Activate(this);
	return Dmg;
}

protected func Suicide() //Hat doch alles keinen Zweck mehr :((
{
	if (EB_Difficulty != EB_Hard)
		return false;
	//Ja...Inventarwechsel kann man sich sparen, kommt eh aufs selbe hinaus
	if (FindContents(GB5B))
		FindContents(GB5B)->~Activate(this);
	if (FindContents(HG5B))
		FindContents(HG5B)->~Activate(this);
	return true;
}

protected func Death(int killedBy)
{
	RemoveEffect("Caller", this);
	RemoveEffect("SlowCaller", this);
	return inherited(killedBy);
}

protected func CheckTerrain()
{
	if (GetCommand())
		return false;
	if(!IsOnAirbike())
		for(var bike in FindObjects(Find_Distance(500), Find_ID(AB5B)))
			if(!GetActionTarget(,bike))
				{
					SetCommand(this,"Follow",bike);
					return true;
				}
	var ID, MaxDist;
	for (var i = 0; i < 11; i++)
	{
		if (i == 0)
		{
			ID = NH5B;
			MaxDist = 500;
		}
		if (i == 1)
		{
			ID = FT5B;
			MaxDist = 400;
		}
		if (i == 2)
		{
			ID = AR5B;
			MaxDist = 400;
		}
		if (i == 3)
		{
			ID = UZ5B;
			if (FindContents(AR5B))
				continue;
			MaxDist = 200;
		}
		if (i == 4)
		{
			ID = GB5B;
			MaxDist = 300;
		}
		if (i == 5)
		{
			ID = CA5B;
			MaxDist = 300;
		}
		if (i == 6)
		{
			ID = GS5B;
			MaxDist = 300;
		}
		if (i == 7)
		{
			ID = RL5B;
			MaxDist = 300;
			if (FindObject(RB5B) && !GetObject2Drop(FindObject(RB5B)))
				continue;
		}
		if (i == 8)
		{
			ID = RB5B;
			MaxDist = 300;
		}
		if (i == 9)
		{
			ID = CNKT;
			MaxDist = 200;
		}
		if (FindContents(ID))
			continue;
		var pObj = FindObject2(Find_ID(ID), Find_Distance(MaxDist), Sort_Distance());
		if (pObj && Contained(pObj) && GetID(Contained(pObj)) != OS1C)
			pObj = FindObject2(Find_ID(ID), Find_NoContainer(), Find_Distance(100), Sort_Distance());
		InitBlacklist();
		if (!InLiquid(pObj))
			if (GetCommand() != "Follow")
				if (GetCommand(, 1) != pObj)
					if (!Contents(4) || GetObject2Drop(pObj))
						if (GetIndexOf(pObj, EB_Blacklist) == -1)
						{
							Item = pObj; //Wird gerade aufgesammelt
							if (Contained(pObj) && GetID(Contained(pObj)) == OS1C)
								SetCommand(this, "MoveTo", Contained(pObj));
							else 
								SetCommand(this, "Get", pObj);
							return true;
						}
	}
}

global func FindBuilder(object Construction)
{
	var x = GetX(Construction);
	var y = GetY(Construction);
	var clonks = FindObjects(Find_OCF(OCF_Alive), Find_Distance(100, x, y), Find_Not(Find_Hostile(GetOwner(Construction))));
	for (var foo in clonks) 
		if (GetCommand(foo, 1) == Construction)
			return foo;
	return false;
}

protected func TryToBuild()
{
	if (SpecialAction)
		return false;
	if (FindBase(GetOwner()))
		if (ObjectDistance(FindBase(GetOwner())) < 300)
			return false;
	//Schon eine Baustelle vorhanden?
	var base = FindObject2(Find_Category(C4D_Structure), Find_Distance(300), Find_Not(Find_Hostile(GetOwner())), Find_Not(Find_OCF(OCF_Fullcon))); //??
	if (base && GetOwner(base) != NO_OWNER)
	{
		SetCommand(this, "Build", base); //Bauen!
		return true;
	}
	//Schon eine Basis ohne Flagge da?
	base = FindObjects(Find_Category(C4D_Structure), Find_Distance(3000), Find_OCF(OCF_Entrance), Find_Not(Find_Hostile(GetOwner())));
	if (base[0])
	{
		var flag = FindObject2(Find_ID(FLAG), Find_Distance(300), Find_Owner(GetOwner()), Find_Or(Find_NoContainer(), Find_Container(this)));
		if (flag)
			for (var foo in base) 
			{
				if (GetBase(foo) == -1)
					if (flag)
					{
						SetCommand(this, "Get", flag);
						AppendCommand(this, "Put", foo, 1, 0, 0, 0, FLAG);
						return true;
					}
			}
		return false;
	}
	/*
 //Wird bald eine erstellt?
 var members = FindObjects(Find_OCF(OCF_Alive),Find_Not(Find_Hostile(GetOwner())));
 for(var bar in members)
  if(GetCommand(bar) == "Construct")
   {SetCommand(this,"Construct",bar);
    return(true);
   }
 */
	//Selber bauen?
	if (!FindContents(CNKT, this))
		return false;
	for (var ID in EB_BaseIDs) 
	{
		//var X = GetX();
		//var Y = GetY();
		if (GetPlrKnowledge(GetOwner(), ID))
		{
			SetCommand(this, "Construct", 0, 0, 0, 0, ID);
			return true;
		}
		continue;
	}
	return false;
}

protected func Shopping() //Mit Waffen eindecken ;)
{
	var ShoppingList = [AR5B, UZ5B, CA5B, NH5B, GB5B]; //Das wünsch ich mir!
	//Schon in einer Base?
	if (Contained() && !Hostile(GetOwner(Contained()), GetOwner()))
	{
		if (GetBase(Contained()) != -1)
		{
			//Erstmal was Verkaufen
			var sell = GetObject2Drop(, , true);
			for (var bar in sell) 
				Sell(GetBase(Contained()), FindContents(bar));
			//Kaufen!
			for (var foo in ShoppingList) 
			{
				if (FindContents(foo))
					continue;
				if (foo == UZ5B)
					continue;
				if (ContentsCount() >= 5)
					if (!GetObject2Drop(, foo))
						continue;
				Buy(foo, GetOwner(), GetBase(Contained()), this, false);
				if (foo == AR5B)
					if (!FindContents(AR5B))
						Buy(UZ5B, GetOwner(), GetBase(Contained()), this, false);
			}
			AddCommand(this, "Exit");
			return true;
		}
	}
	//Base suchen!
	var base = GetNearBase();
	if (!base)
		return false;
	var counter;
	if (ContentsCount() < 5 || GetObject2Drop(, , true))
	{
		for (foo in ShoppingList) 
		{
			if (FindContents(foo))
				continue;
			if (foo == UZ5B && FindContents(AR5B))
				continue;
			if (GetHomebaseMaterial(GetBase(base), foo))
				if (GetWealth(GetBase(base)) >= GetValue(, foo))
				{
					counter++;
					break;
				}
		}
		if (counter >= 1)
			AddCommand(this, "Enter", base);
		return true;
	}
	return false;
}

protected func Ejection(object pObj)
{
	if (!ComputerControlled || GetID(pObj) == PT5B)
		return inherited(pObj);
	if (Contained() && !Hostile(GetOwner(Contained()), GetOwner()))
		if (GetBase(Contained()) != -1)
			Sell(GetBase(Contained()), pObj);
	return false;
}

protected func WaterJump()
{
	if (InLiquid() && !OnFire())
		if (!GBackSemiSolid(0, -1))
		{
			var XDir = 34 * GetDir() - 17;
			var X = GetX();
			var Y = GetY();
			var YDir = -BoundBy(GetPhysical("Swim") / 2500, 24, 38);
			var Hit = SimFlight(X, Y, XDir, YDir);
			XDir = 34 * GetDir() - 17;
			if (Hit && Y <= GetY())
			{
				SetXDir(XDir); //Vllt. ein wenig cheaty
				DolphinJump();
				return true;
			}
		}
	return false;
}

protected func GetNearBase()
{
	var bases = FindObjects(Find_Category(C4D_Structure), Find_Not(Find_Hostile(GetOwner())), Find_OCF(OCF_Entrance), Find_Distance(500), Sort_Distance());
	for (var pObj in bases) 
	{
		if (GetBase(pObj) != -1)
			return pObj;
	}
	return false;
}

protected func Incineration()
{
	if (!ComputerControlled)
		return _inherited();
	var Neo;
	if (!FindContents(NH5B))
		Neo = FindObject2(Find_ID(NH5B), Find_NoContainer(), Find_Distance(400), Sort_Distance());
	var base;
	if (Neo)
		SetCommand(this, "Get", Neo);
	else if (base = GetNearBase())
		SetCommand(this, "Enter", base);
	else if (Enemy)
		SetCommand(this, "Follow", Enemy);
	return _inherited();
}

protected func InitBlacklist()
{
	if (GetType(EB_Blacklist) != C4V_Array)
		EB_Blacklist = CreateArray(0);
	return true;
}

protected func RejectCollect(id pID, object pObj)
{
	if (!ComputerControlled)
		return inherited(pID, pObj);
	if (HasWeapon(ConvertID2Const(pID)))
		return true;
	if (pID == NH5B)
		if (FindContents(NH5B))
			if (GetEnergy() < 55)
			{
				FindContents(NH5B)->~Activate(this);
				if (!Contained(pObj))
					AddCommand(this, "Get", pObj);
				return inherited(pID, pObj);
			}
	if (pID == HG5B)
		if (GetAction(pObj) == "Activated")
			return true;
	if (pID == OB5B)
		if (Hostile(GetOwner(), GetOwner(pObj)))
			return true;
	var Drop;
	Drop = GetObject2Drop(pObj);
	var X = GetX(), Y = GetY(), XDir, YDir = 20;
	if (!SimFlight(X, Y, XDir, YDir))
		return inherited(pID, pObj);
	if (Distance(X, Y, GetX(), GetY()) < 30)
		if (Drop)
		{
			Exit(Drop, AbsX(X), AbsY(Y));
			if (inherited(pID, pObj))
				RejectCollect(pID, pObj);
		}
	return inherited(pID, pObj);
}

protected func Collection2(object pObj)
{
	if (!ComputerControlled)
		return _inherited(pObj);
	if (pObj == Item)
		Item = 0;
	InitBlacklist(); //Sicherstellen dass schon Initialisiert
	var i = GetIndexOf(pObj, EB_Blacklist);
	if (i > -1)
		DeleteArrayItem(i, EB_Blacklist);
	if(GetID(pObj) == CC5B)
	{
		pObj ->~ Activate(this); //:O
		return true;
	}
	return _inherited(pObj);
}

protected func GetObject2Drop(object pObj, id DefID, bool unneeded)
{
	if (!ComputerControlled)
		return _inherited(pObj);
	if (!pObj && !DefID && !unneeded)
		return false;
	var Priority4 = [NH5B, AR5B, FT5B]; //Überlebensnotwendig
	var Priority3 = [UZ5B, CA5B, GB5B, GS5B, RL5B, RB5B, CC5B]; //Häufig zu gebrauchen
	var Priority2 = [PM5B, HG5B, MO5B, JP5B, MS5B, CNKT, FLAG]; //Unter Umständen zu gebrauchen
	var Priority1 = [BB5B, GP5B, HP5B, TB5B, OB5B]; //Nicht zu gebrauchen
	//Eclipse besänftigen...Die Warnings nerven.
	GetLength(Priority4);GetLength(Priority3);GetLength(Priority2);GetLength(Priority1);
	//Ermittelt unnötige Objekte
	if (unneeded)
	{
		var ID, PrioCurr, Found, Ret = [];
		for (var i; i < ContentsCount(); i++)
		{
			ID = GetID(Contents(i));
			for (var o = 3; o < 5; o++)
			{
				PrioCurr = VarN(Format("Priority%d", o));
				for (var g = 0; g < GetLength(PrioCurr); g++)
				{
					if (PrioCurr[g] == ID)
					{
						Found = true;
						break;
					}
				}
				if (Found)
					break;
			}
			if (!Found)
			{
				Ret[GetLength(Ret)] = ID;
			}
			Found = false;
			continue;
		}
		return Ret;
	}
	//Und normal gehts weiter:
	CheckForReload();
	if (!Contents(4))
		return false;
	var ID, PrioCurr;
	var pID = GetID(pObj);
	if (DefID)
		pID = DefID;
	if (FindContents(pID))
		return false;
	var Priority;
	for (var i = 1; i < 5; i++)
	{
		PrioCurr = VarN(Format("Priority%d", i));
		for (var e = 0; e < GetLength(PrioCurr); e++)
			if (PrioCurr[e] == pID)
				Priority = i;
	}
	if (Priority <= 0)
		Priority = 1;
	//Message("Ermittelte Priorität: %d",pObj,Priority);
	if (pID == GS5B && FindContents(GS5B))
		if (CheckForReload(pID))
			return false;
	for (var i = 1; i <= Priority; i++)
	{
		PrioCurr = VarN(Format("Priority%d", i));
		for (var e = 0; e < GetLength(PrioCurr); e++)
		{
			if (Priority > i)
				if (FindContents(PrioCurr[e]))
					return FindContents(PrioCurr[e]);
			if (Priority == i)
				if (ContentsCount(PrioCurr[e]) >= 2)
					return FindContents(PrioCurr[e]);
		}
	}
	return 0;
}

protected func CheckContents()
{
	return true;
}

protected func CheckForReload(id Urgent)
{
	var Reloaded;
	if (HasWeapon(EB_FlameThrower))
	{
		if (HasAmmo(EB_FlameThrower) <= 0)
			if (FindContents(GS5B))
			{
				ShiftContents(, , FT5B);
				CheckArmed();
				Contents(0)->~Activate(this);
				Reloaded = true;
			}
		if (Urgent == GS5B)
			if (HasAmmo(EB_FlameThrower) < 50)
			{
				ShiftContents(, , FT5B);
				CheckArmed();
				Contents(0)->~Activate(this);
				return true;
			}
	}
	if (HasWeapon(EB_AssaultRifle))
	{
		if (HasAmmo(EB_AssaultRifle) <= 0)
			if (FindContents(CA5B))
			{
				ShiftContents(, , AR5B);
				if (LocalN("mode", Contents(0)) == "grenades")
					Contents(0)->~ControlSpecial2(this);
				CheckArmed();
				Contents(0)->~Activate(this);
				;
				Reloaded = true;
			}
		if (Urgent == CA5B)
			if (HasAmmo(EB_AssaultRifle) < 50)
			{
				ShiftContents(, , AR5B);
				if (LocalN("mode", Contents(0)) == "grenades")
					Contents(0)->~ControlSpecial2(this);
				CheckArmed();
				Contents(0)->~Activate(this);
				return true;
			}
	}
	if (HasWeapon(EB_Uzi))
		if (HasAmmo(EB_Uzi) <= 0)
			if (FindContents(CA5B))
			{
				ShiftContents(, , UZ5B);
				CheckArmed();
				Contents(0)->~Activate(this);
				Reloaded = true;
			}
	if (HasWeapon(EB_GrenadeFire))
		if (HasAmmo(EB_GrenadeFire) <= 0)
			if (FindContents(GB5B))
			{
				ShiftContents(, , AR5B);
				if (LocalN("mode", Contents(0)) == "Cartridges")
					Contents(0)->~ControlSpecial2(this);
				CheckArmed();
				Contents(0)->~Activate(this);
				Reloaded = true;
			}
	if (HasWeapon(EB_RocketLauncher))
		if (HasAmmo(EB_RocketLauncher) <= 0)
			if (!IsFiring())
			{
				if (!FindContents(MS5B))
					if (FindContents(RB5B))
					{
						ShiftContents(, , RB5B);
						Contents(0)->~Activate(this);
					}
				if (FindContents(MS5B))
				{
					if (ShiftContents(, , RL5B))
					{
						CheckArmed();
						Contents(0)->~Activate(this);
						Reloaded = true;
					}
				}
			}
	if (HasWeapon(EB_AB_Bullet))
		if (HasAmmo(EB_AB_Bullet) <= 0)
		{
			if (FindContents(CA5B))
				IsOnAirbike()->~Activate(this);
			Reloaded = true;
		}
	//Fertig 
	if (Reloaded)
		return true;
	return false;
}

protected func Attack()
{
	if (IsOnAirbike())
	{
		AirbikeAttack();
		return true;
	}
	var Dir;
	if (WildcardMatch(GetAction(), "*Swim*"))
		return false;
	var Debug = EB_EnableDebugging;
	var Mode = CanHitBy(Dir, Debug);
	if (Mode != IsFiring() && !SpecialAction)
		StopFiring();
	if (!Mode)
		return false;
	if (Mode == IsFiring())
	{
		SetDir(Dir);
		SetComDir(COMD_Stop);
		if (GetDir() != Dir && GetCommand() != "Wait")
			AddCommand(this, "Wait", , , , , , 20);
		return true;
	}
	if (GetCommand() == "Build" || GetCommand() == "Dig")
	{
		SetCommand(this, "");
		SetAction("Walk");
		CheckArmed();
	}
	if (Mode != EB_Pistol && GetID(Contents(0)) == PT5B)
		Holster();
	if (Mode == EB_FlameThrower)
	{
		if (Contents(0) && GetID(Contents(0)) != FT5B)
			if (!ShiftContents(, , FT5B))
				return false;
		CheckArmed(); //Damit die Action aktualisiert wird
		if (GetDir() != Dir && GetCommand() != "Wait")
			AddCommand(this, "Wait", , , , , , 20);
		SetComDir(COMD_Stop);
		SetDir(Dir);
		if (!IsFiring())
			Contents(0)->~ControlThrow(this);
		return true;
	}
	if (Mode == EB_AssaultRifle)
	{
		if (!ShiftContents(, , AR5B))
			return false;
		CheckArmed(); //Damit die Action aktualisiert wird
		if (LocalN("mode", Contents(0)) != "Cartridges")
			Contents(0)->~ControlSpecial2(this);
		if (GetDir() != Dir && GetCommand() != "Wait")
			AddCommand(this, "Wait", , , , , , 20);
		SetComDir(COMD_Stop);
		SetDir(Dir);
		if (!IsFiring())
			Contents(0)->~ControlThrow(this);
		return true;
	}
	if (Mode == EB_Uzi)
	{
		if (!ShiftContents(, , UZ5B, true))
			return false;
		CheckArmed(); //Damit die Action aktualisiert wird
		if (GetDir() != Dir && GetCommand() != "Wait")
			AddCommand(this, "Wait", , , , , , 20);
		SetComDir(COMD_Stop);
		SetDir(Dir);
		if (!IsFiring())
			Contents(0)->~ControlThrow(this);
		return true;
	}
	if (Mode == EB_Pistol)
	{
		if (GetID(Contents(0)) != PT5B)
			Holster();
		if (GetDir() != Dir && GetCommand() != "Wait")
			AddCommand(this, "Wait", , , , , , 20);
		SetComDir(COMD_Stop);
		SetDir(Dir);
		if (!IsFiring())
			Contents(0)->~ControlThrow(this);
		return true;
	}
	if (Mode == EB_GrenadeFire)
	{
		if (!ShiftContents(, , AR5B, true))
			return false;
		CheckArmed();
		if (LocalN("mode", Contents(0)) != "Grenades")
			Contents(0)->~ControlSpecial2(this);
		SetDir(Dir);
		if (!IsFiring())
			Contents(0)->~ControlThrow(this);
		return true;
	}
	if (Mode == EB_HandGrenade)
	{
		if (!ShiftContents(, , HG5B, true))
			if (FindContents(GB5B))
			{
				FindContents(GB5B)->~Activate(this);
				ShiftContents(, , HG5B, true);
			}
		if (GetID(Contents(0)) != HG5B)
			return false;
		Contents(0)->~Activate(this);
		if (Dir != -1)
			DropBeveled(Contents(0), Dir);
		else 
			Exit(Contents(0));
		ThrowingAllowed = false;
		return true;
	}
	if (Mode == EB_Molotov)
	{
		if (!ShiftContents(, , MO5B, true))
			if (ShiftContents(, , GS5B, true))
			{
				Contents(0)->~Activate(this);
				ShiftContents(, , MO5B, true);
			}
		if (GetID(Contents(0)) != MO5B)
			return false;
		DropBeveled(Contents(0), Dir);
		ThrowingAllowed = false;
		return true;
	}
	if (Mode == EB_RocketLauncher)
	{
		if (IsFiring())
			return false;
		if (GetID(Contents(0)) != RL5B)
		{
			ShiftContents(, , RL5B);
			CheckArmed();
		}
		if (GetID(Contents(0)) != RL5B)
			return false;
		SetCommand(this, "");
		SetComDir(COMD_None);
		SetDir(Dir);
		GuidingAimX = [GetX(Enemy)];
		GuidingAimY = [GetY(Enemy)];
		Contents(0)->~ControlThrow(this); //Schießen
		Contents(0)->~ControlDig(this); //Steuerung übernehmen
		GuidingMissile = LocalN("missile", Contents(0));
	}
}

public func KilledClonk(object pClonk)
{
	/*if(!Random(3))
		return false;*/
//	var str = Format("$Mess%d$",Random(34)+1); //Localized stings on demand -> Geht nicht
	var str;
	var foo = Random(34)+1;
	if(foo == 1) str = "$Mess1$";
	if(foo == 2) str = "$Mess2$";
	if(foo == 3) str = "$Mess3$";
	if(foo == 4) str = "$Mess4$";
	if(foo == 5) str = "$Mess5$";
	if(foo == 6) str = "$Mess6$";
	if(foo == 7) str = "$Mess7$";
	if(foo == 8) str = "$Mess8$";
	if(foo == 9) str = "$Mess9$";
	if(foo == 10) str = "$Mess10$";
	if(foo == 11) str = "$Mess11$";
	if(foo == 12) str = "$Mess12$";
	if(foo == 13) str = "$Mess13$";
	if(foo == 14) str = "$Mess14$";
	if(foo == 15) str = "$Mess15$";
	if(foo == 16) str = "$Mess16$";
	if(foo == 17) str = "$Mess17$";
	if(foo == 18) str = "$Mess18$";
	if(foo == 19) str = "$Mess19$";
	if(foo == 20) str = "$Mess20$";
	if(foo == 21) str = "$Mess21$";
	if(foo == 22) str = "$Mess22$";
	if(foo == 23) str = "$Mess23$";
	if(foo == 24) str = "$Mess24$";
	if(foo == 25) str = "$Mess25$";
	if(foo == 26) str = "$Mess26$";
	if(foo == 27) str = "$Mess27$";
	if(foo == 28) str = "$Mess28$";
	if(foo == 29) str = "$Mess29$";
	if(foo == 30) str = "$Mess30$";
	if(foo == 31) str = "$Mess31$";
	if(foo == 32) str = "$Mess32$";
	if(foo == 33) str = "$Mess33$";
	if(foo == 34) str = "$Mess34$";
	Message(str, this);
	return true;
}

protected func GetR360(pObj)
{
	return (GetR(pObj) + 360) % 360;
}

protected func DeleteArrayItem(int Position, & Array)
{
	for (var i = Position; i < GetLength(Array); i++)
		Array[i] = Array[i + 1];
	SetLength(Array, GetLength(Array) - 1);
	return 1;
}

global func SearchMissileEnemy()
{
	if (!this)
		return false;
	return FindObject2(	Find_Distance(500),
						Find_OCF(OCF_CrewMember),
						Find_Hostile(GetOwner()),
						Find_NoContainer(),
						Find_Not(Find_Action("Swim")),
						Find_PathFree());
}

protected func GuideMissile()
{
	if (GetID(Contents(0)) != RL5B || !LocalN("guiding", Contents(0)))
	{
		GuidingMissile = 0;
		return false;
	}
	if (GetType(GuidingAimX) != C4V_Array)
	{
		GuidingAimX = [];
		GuidingAimY = [];
	}
	//Gegner weg... neuen suchen
	if (!GetAlive(Enemy))
	{
		Enemy = GuidingMissile->SearchMissileEnemy();
		if (!Enemy)
			Contents(0)->~ControlThrow(this);
	}
	//Wegpunkt 1 setzen, falls Weg frei
	var myX = GetX(GuidingMissile);
	var myY = GetY(GuidingMissile);
	
	if (PathFree(myX, myY, GetX(Enemy), GetY(Enemy)))
	{
		GuidingAimX = [GetX(Enemy)];
		GuidingAimY = [GetY(Enemy)];
	}
	else 
	{
		if (GetLength(GuidingAimX) == 1)
		{
			GuidingAimX[GetLength(GuidingAimX)] = GetX(Enemy);
			GuidingAimY[GetLength(GuidingAimY)] = GetY(Enemy);
		}
		if (PathFree(GuidingAimX[GetLength(GuidingAimX) - 2], GuidingAimY[GetLength(GuidingAimY) - 2], GetX(Enemy), GetY(Enemy)))
		{
			GuidingAimX[GetLength(GuidingAimX) - 1] = GetX(Enemy);
			GuidingAimY[GetLength(GuidingAimY) - 1] = GetY(Enemy);
		}
		else 
		{
			//var Optimated = SeperateCoordinates(GetX(Enemy), GetY(Enemy));
			//GuidingAimX[GetLength(GuidingAimX)] = Optimated[0];
			//GuidingAimY[GetLength(GuidingAimY)] = Optimated[1];
			
			GuidingAimX[GetLength(GuidingAimX)] = GetX(Enemy);
			GuidingAimY[GetLength(GuidingAimY)] = GetY(Enemy);
		}
	}
	//Gegner erreicht? 
	if (ObjectDistance(Enemy, GuidingMissile) < 15)
	{
		Contents(0)->~ControlThrow(this); //BAAAM
		GuidingMissile = 0;
		return true;
	}
	
	if(EB_EnableDebugging)
	{
		for(var i = 1; i<GetLength(GuidingAimX); i++)
			CreateParticle("NoGravSpark",AbsX(GuidingAimX[i]),AbsY(GuidingAimY[i]),0,0,40,RGB(255,0,0));
		CreateParticle("NoGravSpark",AbsX(GuidingAimX[0]),AbsY(GuidingAimY[0]),0,0,40,RGB(0,255,0));
	}
	 
	//Am Wegpunkt angekommen?
	if (Inside(myX, GuidingAimX[0] - 5, GuidingAimX[0] + 5))
		if (Inside(myY, GuidingAimY[0] - 5, GuidingAimY[0] + 5))
		{
			DeleteArrayItem(0, GuidingAimY);
			DeleteArrayItem(0, GuidingAimX);
		}
	var R = GetR360(GuidingMissile);
	var X = GuidingAimX[0];
	var Y = GuidingAimY[0];
	//Wegpunkt anfliegen
	var TargetR = Angle(myX, myY, X, Y);
	if (Inside(R, TargetR - 5, TargetR + 5))
	{
		if (LocalN("command", GuidingMissile) != "Straight")
			Contents(0)->~ControlDown(this);
	}
	else 
	{
		var right = TargetR - R;
		var left = 360 - right;
		if (right < 0)
		{
			left = Abs(right);
			right = 360 - left;
		}
		if (right <= left)
		{
			if (LocalN("command", GuidingMissile) != "Right")
				Contents(0)->~ControlRight(this);
		}
		if (left < right)
		{
			if (LocalN("command", GuidingMissile) != "Left")
				Contents(0)->~ControlLeft(this);
		}
	}
	return true;
}

//Gegebene X/Y Koordinaten verschieben, um Abstand zur Landschaft zu erreichen
global func SeperateCoordinates(int X, int Y)
{
	var SumX, SumY;
	var iAngle = [0, 45, 90, 135, 180, 225, 270];
	var iSin, iCos;
	var X2, Y2;
	for (var i; i < GetLength(iAngle); i++)
	{
		X2 = X;
		Y2 = Y;
		iSin = Sin(iAngle[i], 30);
		iCos = -Cos(iAngle[i], 30);
		PathFree2(X2, Y2, iSin, iCos);
		SumX += X2;
		SumY += Y2;
	}
	var iMiddleX = SumX / GetLength(iAngle);
	var iMiddleY = SumY / GetLength(iAngle);
	return [iMiddleX, iMiddleY];
}

//Airbike Steuerung
//------------------------------------------------------------------------------

/*
protected func SetCommand(object pObj, string szCommand, object pTarget, int iX, int iY, object pTarget2, Data, int iRetries)
{
	if(!IsComputerControlled() || !IsOnAirbike())
		return inherited(pObj, szCommand, pTarget, iX, iY, pTarget2, Data, iRetries);
	if(IsComputerControlled())
		Command2Airbike(szCommand, pTarget, iX, iY, pTarget2, Data);
	return false;
}
*/

public func AirbikeDrop(object airbike)
{
	if(CanFire(EB_Jetpack))
		{
			ShiftContents(,,JP5B);
			Contents(0) ->~ ControlThrow(this);
			SetCommand(this, "Follow", airbike);
		}
	return true;
}

protected func CanGetOnAirbike()
{
	var ret;
	if(ret = inherited())
		SetCommand(this,"");
	return ret;
}

protected func Command2Airbike(string strCommand, object pTarget, int iTx, int iTy, object pTarget2, int iData, object pCmdObj)
{
	var Bike = IsOnAirbike();
	Message("%s",this,strCommand);
	if (Bike && strCommand == "MoveTo")
	{
		SetCommand(Bike, "MoveTo", pTarget, iTx, iTy);
		SetCommand(this, "");
		return true;
	}
	return true;
}  

protected func ControlCommandFinished(string strCommand, object pTarget, int iTx, int iTy, object pTarget2, Data)
{
	if (!ComputerControlled)
		return false;
	//FxSlowCallerTimer();
	return 1;
}

protected func AirbikeAttack()
{
	var Mode;
	var Dir;
	var Bike = IsOnAirbike();
	if (Mode = ByBulletWeaponNew(-1))
		Dir = 0;
	else
		if (Mode = ByBulletWeaponNew(1))
			Dir = 1;
	var Firing = IsFiring();
	if (Firing && Mode != Firing)
		StopFiring();
	if(!Mode)
		return false;
	//Feuern
	if(GetDir(Bike) == Dir && Mode == Firing)
		return true; //Passt alles
	if(GetDir(Bike) != Dir && Dir==0)
	{
		if(Firing)
			StopFiring();
		SetCommand(Bike,"");
		//Bike->ControlLeft(this);
		return true;
	}
	if(GetDir(Bike) != Dir && Dir==1)
	{
		if(Firing)
			StopFiring();
		SetCommand(Bike,"");
		//Bike->ControlRight(this);
		return true;
	}
	
	if (Mode == EB_AB_Bullet)
	{
		if (LocalN("mode", Bike) != "Cartridges")
		{
			Bike->ControlSpecial2(this);
			if (LocalN("mode", Bike) != "Cartridges")
			{
				Bike->ControlSpecial2(this);
				if (LocalN("mode", Bike) != "Cartridges")
					Bike->ControlSpecial2(this);
			}
		}
		Bike->ControlThrow(this);
		return true;
	}
}

protected func IsOnAirbike()
{
	if (GetAction() == "AirbikeFly")
		return GetActionTarget();
	return false;
}

//------------------------------------------------------------------------------

protected func DropBeveled(object pObj, int Dir)
{
	var Y = ExitYDistance(GetID(pObj), true);
	Exit(pObj, , Y, , 4 * Dir - 2);
	return true;
}

protected func ExitYDistance(pID, Dropping)
{
	if (pID == HG5B || pID == MO5B)
		if (Dropping)
			return 9;
		else 
			return 6;
	return 0;
}

protected func StopFiring()
{
	var Bike;
	if (Bike = IsOnAirbike())
	{
		Bike->~ControlThrow(this);
		return true;
	}
	if (IsFiring() && GetID(Contents(0)) != RL5B)
		Contents(0)->~ControlThrow(this);
	return 1;
}

protected func ConvertID2Const(id pID)
{
	if (pID == FT5B)
		return EB_FlameThrower;
	if (pID == AR5B)
		return EB_AssaultRifle;
	if (pID == UZ5B)
		return EB_Uzi;
	return false;
}

//Conditions
//-----------------------------------------------------------
protected func IsFiring()
{
	var Bike;
	if (Bike = IsOnAirbike())
		if (WildcardMatch(GetAction(Bike), "Wait*") || GetAction(Bike) == "Shoot")
			return EB_AB_Bullet;
	if (!Contents(0))
		return false;
	var Action = GetAction(Contents(0));
	if (GetID(Contents(0)) == AR5B || GetID(Contents(0)) == UZ5B)
	{
		if (Action == "Shoot" || Action == "Stop")
		{
			if (GetID(Contents(0)) == AR5B)
				return EB_AssaultRifle;
			if (GetID(Contents(0)) == UZ5B)
				return EB_Uzi;
		}
	}
	if (GetID(Contents(0)) == FT5B)
		if (Action == "Shoot")
			return EB_FlameThrower;
	if (GetID(Contents(0)) == PT5B)
	{
		if (Action == "Wait")
			return EB_Pistol;
	}
	if (GetID(Contents(0)) == RL5B)
	{
		if (Action == "Reload" || Action == "Wait")
			return true;
		if (LocalN("guiding", Contents(0)))
			return EB_RocketLauncher;
	}
	return false;
}

protected func IsEnemy(object pObj)
{
	if (!Hostile(GetOwner(), GetOwner(pObj)))
		return false;
	if (GetOCF(pObj) & OCF_Alive || pObj->~IsMissile())
		return true;
	return false;
}

protected func CanHitBy(& Dir, bool Debug)
{
	if (GetID(Contents(0)) == RL5B)
		if (LocalN("missile", Contents(0)))
			if (!OnFire())
				if (GetProcedure() == "WALK")
				{
					//Wieder aufnehmen
					Contents(0)->~ControlDig(this);
					GuidingMissile = LocalN("missile", Contents(0));
					//Enemy = 0; //Gegner neu suchen
					return false;
				}
	CheckForReload(); //Nachladen? 
	if (InLiquid())
		return false;
	if (Contained())
		return false;
	//Die Abfragen
	var Mode;
	if (Mode = ByFlameThrowerNew(0))
	{
		Dir = 0;
		return Mode;
	}
	if (Mode = ByFlameThrowerNew(1))
	{
		Dir = 1;
		return Mode;
	}
	var iDir;
	if (EB_Difficulty == EB_Hard)
		if (Mode = ByFlameThrowerFallingNew(iDir))
		{
			Dir = iDir;
			return Mode;
		}
	if (Mode = ByBulletWeaponNew(-1))
	{
		Dir = 0;
		return Mode;
	}
	if (Mode = ByBulletWeaponNew(1))
	{
		Dir = 1;
		return Mode;
	}
	if (Mode = ByGrenadeFireNew(0, Debug))
	{
		Dir = 0;
		return Mode;
	}
	if (Mode = ByGrenadeFireNew(1, Debug))
	{
		Dir = 1;
		return Mode;
	}
	if (Mode = ByHandGrenadeNew(iDir))
	{
		Dir = iDir;
		return Mode;
	}
	if (Mode = ByMolotovNew(iDir))
	{
		Dir = iDir;
		return Mode;
	}
	if (Mode = ByRocketLauncherNew(iDir))
	{
		Dir = iDir;
		return Mode;
	}
	return false;
}

global func Hitable(Mode)
{
	if (!this)
		return false;
	if (Mode == EB_FlameThrower || Mode == EB_Molotov)
		if (GetOCF(this) & OCF_Inflammable || GetOCF(this) & OCF_OnFire)
			return true;
	if (Mode == "Bullets" || Mode == EB_GrenadeFire || Mode == EB_HandGrenade || Mode == EB_RocketLauncher)
		if (GetOCF(this) & OCF_Alive)
			return true;
	if (Mode == "Bullets" || Mode == EB_FlameThrower)
		if (this->~IsMissile())
			return true;
	return false;
}

protected func ByFlameThrowerNew(int Dir)
{
	var Direction = 2 * Dir - 1; //-1 Links, 1 Rechts
	if (!CanFire(EB_FlameThrower))
		return false;
	var rect = Find_InRect((Dir - 1) * 100, -50, 100, 100);
	var cont = Find_NoContainer();
	var ocf = Find_Or(Find_OCF(OCF_CrewMember), Find_OCF(OCF_Collectible));
	var exclude = Find_Exclude(this);
	var Aims = FindObjects(rect, cont, exclude, ocf, Sort_Distance());
	if (!Aims[0])
		return false;
	//Hindernis?
	var Obstacle = 200;
	for (var i = 0; i < 100; i++)
		if (GBackSolid(Direction * i, 2))
		{
			var MatName = MaterialName(GetMaterial(Direction * i, 2));
			if (MatName != "Ice" && MatName != "Snow")
				Obstacle = Direction * 1;
		}
	//Direkter Treffer
	for (var pObj in Aims) 
	{
		if (GetID(pObj) == GS5B || GetID(pObj) == HG5B)
			if (ObjectDistance(pObj) > 60)
				return false;
		if (!FindObject(NF5B))
			if (!Hostile(GetOwner(), GetOwner(pObj)))
				return false;
		if (!IsEnemy(pObj))
			continue;
		if (Inside(GetY(pObj) - GetY(), -5, 5))
			if (Obstacle > ObjectDistance(pObj))
				return EB_FlameThrower;
	}
	return false;
}

protected func ByFlameThrowerFallingNew(& Dir)
{
	if (!CanFire(EB_FlameThrower))
		return false;
	//var dist = Find_Distance(200);
	var rect = Find_InRect(-100, -100, 200, 100);
	//var hitable = Find_Func("Hitable", EB_FlameThrower);
	var ocf = Find_OCF(OCF_CrewMember);
	var enemy = Find_Hostile(GetOwner());
	var cont = Find_NoContainer();
	var exclude = Find_Exclude(this);
	var Aims = FindObjects(exclude, rect, enemy, ocf, cont, Sort_Distance());
	if (!Aims[0])
		return false;
	var danger = Find_And(Find_ID(GS5B), Find_ID(HG5B));
	var rect = Find_InRect(-100, -5, 100, 10);
	var Threats = FindObject2(rect, danger, Sort_Distance());
	//Hindernis links?
	var ObstacleL = 200;
	for (var i = 0; i < 100; i += 5)
		if (GBackSolid(i * -1, 2))
		{
			var MatName = MaterialName(GetMaterial(i * -1, 2));
			if (MatName != "Ice" && MatName != "Snow")
				ObstacleL = i;
		}
	rect = Find_InRect(-100, -5, 100, 10);
	Threats = FindObject2(rect, danger, Sort_Distance());
	//Hindernis rechts?
	var ObstacleR = 200;
	for (var i = 0; i < 100; i += 5)
		if (GBackSolid(i, 2))
		{
			var MatName = MaterialName(GetMaterial(i, 2));
			if (MatName != "Ice" && MatName != "Snow")
				ObstacleR = i;
		}
	for (pObj in Aims) 
	{
		//Indirekter Treffer
		if (!IsEnemy(pObj))
			continue;
		var X = GetX(pObj), Y = GetY(pObj), XDir = GetXDir(pObj), YDir = GetYDir(pObj), Hit;
		if (GetProcedure(pObj) == "FLIGHT")
			if (GetYDir(pObj) > 0)
				for (var i = 0; i < 60; i++)
				{
					Hit = Destination(X, Y, XDir, YDir, 1);
					if (Inside(Y - GetY(), -5, 5))
					{
						Dir = 0;
						if (GetX() < X)
							Dir = 1;
						if (!Dir && ObstacleL < Distance(GetX(), GetY(), X, Y))
							break;
						if (Dir && ObstacleR < Distance(GetX(), GetY(), X, Y))
							break;
						return EB_FlameThrower;
					}
					if (Hit)
						break;
				}
	}
}

protected func ByBulletWeaponNew(int Direction)
{
	//Kugeln?
	var Available;
	if (CanFire(EB_AB_Bullet))
		Available = EB_AB_Bullet;
	else if (CanFire(EB_AssaultRifle))
		Available = EB_AssaultRifle;
	else if (CanFire(EB_Uzi))
		Available = EB_Uzi;
	else if (CanFire(EB_Pistol))
		Available = EB_Pistol;
	if (!Available)
		return false;
	var ex = Find_Exclude(this);
	var line = Find_OnLine(0, 0, 210 * Direction, 0);
	var cont = Find_NoContainer();
	var ocf = Find_Or(Find_OCF(OCF_CrewMember), Find_OCF(OCF_Collectible));
	var Aims = FindObjects(ex, cont, line, ocf, Sort_Distance());
	if (!Aims[0])
		return false;
	for (var pObj in Aims) 
	{
		if (IsEnemy(pObj))
			if (PathFree(GetX(), GetY() - 3, GetX(pObj), GetY(pObj)))
				return Available;
		if (GetID(pObj) == GR5B) //Schießt auch auf feindliche Sturmgewehr-Granaten
			if (GetDir() == Direction) //Nur bei gleicher Richtung, nicht umdrehen
				if (Hostile(GetOwner(), GetOwner(pObj)))
					return Available;
		if (!FindObject(NF5B))
			if (GetOCF(pObj) & OCF_CrewMember)
				if (!Hostile(GetOwner(), GetOwner(pObj)))
					return false;
		if (GetID(pObj) == GS5B || GetID(pObj) == HG5B)
			return false;
	}
	return false;
}

protected func ByGrenadeFireNew(int Dir, Debug)
{
	//Geschossene Granaten?
	if (!CanFire(EB_GrenadeFire))
		return false;
	var Direction = 2 * Dir - 1; //-1 Links, 1 Rechts
	var X = GetX() + 4 * Direction;
	var Y = GetY() + 1;
	if(GBackSolid(AbsX(X), AbsY(Y)))
		return false;
	var XDir = 100 * Direction;
	var YDir = -10;
	var Hit;
	//var iDir;
	var Color;
	if (Debug)
		Color = RGB(255, 0, 0);
	var InRange = FindObjects(Find_InRect((Dir - 1) * 600, -10, 600, 600),
							  Find_NoContainer(),
							  Find_Exclude(this),
							  Find_Or(Find_OCF(OCF_CrewMember), Find_Category(C4D_Structure)), 
							 );
	if (!InRange[0])
		return false;
	//var Aims;
	var Repeats = 50;
	if (EB_Difficulty == EB_Easy)
		Repeats = 25;
	
	for (var i = 0; i < Repeats; i++)
	{
		Hit = Destination(X, Y, XDir, YDir, 1, Color);
		/*
		Aims = FindObjects(Find_Distance(6, AbsX(X), AbsY(Y)), 
						   Find_Or(Find_OCF(OCF_CrewMember), Find_Category(C4D_Structure), Find_Category(C4D_Vehicle)), 
						   Find_NoContainer(), 
						   Sort_Distance(AbsX(X), AbsY(Y))
						  );
		*/
		for (var pObj in InRange) 
		{
			if(Distance(X, Y, GetX(pObj), GetY(pObj)) > 6)
				continue;
			//if (GetCategory(pObj) & C4D_Vehicle && GetID(pObj) != GR5B)
			//	return false; //Nicht auf Fahrzeuge schießen
			if (Hostile(GetOwner(), GetOwner(pObj)))
				return EB_GrenadeFire;
			else if (!FindObject(NF5B))
				return false;
			if (GetCategory(pObj) & C4D_Structure)
				if (!Hostile(GetOwner(), GetOwner(pObj)))
					return false; //Nicht auf freundliche Gebäude schießen
		}
		if (!Hit)
			continue;
		if (EB_Difficulty != EB_Hard)
			break;
		X = GetX() + 4 * Direction;
		Y = GetY() + 1;
		XDir = 100 * Direction;
		YDir = -10;
		Hit = SimFlight(X, Y, XDir, YDir);
		if (!Hit)
			break; //Unterschiedliche Ergebnise
		if (Distance(GetX(), GetY(), X, Y) < 20)
			break; //Abstand muss größer sein als 20
		//Pseudo Granate erschaffen
		var pseudo = CreateObject(GR5B, AbsX(X), AbsY(Y), -1);
		var Contact = GetContact(pseudo, -1, 0);
		if (!Contact)
		{
			RemoveObject(pseudo);
			SimFlight(X, Y, XDir, YDir);
			pseudo = CreateObject(GR5B, AbsX(X), AbsY(Y), -1);
			Contact = GetContact(pseudo, -1, 0);
			if (!Contact)
			{
				RemoveObject(pseudo);
				continue;
			}
		}
		RemoveObject(pseudo);
		var strCNAT = "";
		
		if (Contact & CNAT_Left)
		{
			XDir = -2 * XDir / 3;
			YDir = +2 * YDir / 3;
			strCNAT = Format("%s %s", strCNAT, "Left");
		}
		if (Contact & CNAT_Right)
		{
			XDir = -2 * XDir / 3;
			YDir = +2 * YDir / 3;
			strCNAT = Format("%s %s", strCNAT, "Right");
		}
		if (Contact & CNAT_Bottom)
		{
			XDir = +2 * XDir / 3;
			YDir = -2 * YDir / 3;
			strCNAT = Format("%s %s", strCNAT, "Bottom");
		}
		if (Contact & CNAT_Top)
		{
			XDir = +2 * XDir / 3;
			YDir = -2 * YDir / 3;
			strCNAT = Format("%s %s", strCNAT, "Top");
		}
		
		var Ignore = true;
		if (Debug)
		{
			CustomMessage(strCNAT, this, GetOwner(), X - GetX(), Y - GetY() - 10); 
			Color = RGB(0, 255, 0);
		}
		for (var o = 0; o < 40; o++)
		{
			if (Debug)
				Color = RGB(0, 255, 0);
			Hit = Destination(X, Y, XDir, YDir, 1, Color, Ignore);
			/*
			Aims = FindObjects(Find_Distance(6, AbsX(X), AbsY(Y)), 
							   Find_Or(Find_OCF(OCF_CrewMember), Find_Category(C4D_Structure), Find_Category(C4D_Vehicle)), 
							   Find_NoContainer(), 
							   Sort_Distance(AbsX(X), AbsY(Y))
							  );
			*/
			for (var pObj in InRange) 
			{
				if(Distance(X, Y, GetX(pObj), GetY(pObj)) > 6)
					continue;
				if (GetCategory(pObj) & C4D_Vehicle)
					return false; //Nicht auf Fahrzeuge schießen
				if (Hostile(GetOwner(), GetOwner(pObj)))
					return EB_GrenadeFire;
				else if (!FindObject(NF5B))
					return false;
				if (GetCategory(pObj) & C4D_Structure)
					if (!Hostile(GetOwner(), GetOwner(pObj)))
						return false; //Nicht auf freundliche Gebäude schießen
			}
			Ignore = false;
			if (Hit)
				break;
		}
		break;
	}
	return false;
}

protected func ByHandGrenadeNew(& Dir)
{
	//Granaten?
	if (!HasWeapon(EB_HandGrenade) || !ThrowingAllowed)
		return false;
	var iDir, X, Y, XDir, YDir;
	var iDist;
	var act = Find_Action("FlamethrowerWalk");
	var dist = Find_Distance(100);
	var enemy = Find_Hostile(GetOwner());
	var Aims = FindObjects(act, dist, enemy, Sort_Distance());
	if (!Aims[0])
		return false;
	for (var pObj in Aims) 
		if (IsEnemy(pObj))
			if (pObj->~IsFiring())
				for (var i = 0; i < 3; i++)
				{
					if (i == 0)
					{
						iDir = DIR_Left;
						X = GetX();
						Y = GetY() + ExitYDistance(HG5B);
						XDir = -20;
						YDir = 0;
					}
					if (i == 1)
					{
						iDir = DIR_Right;
						X = GetX();
						Y = GetY() + ExitYDistance(HG5B);
						XDir = 20;
						YDir = 0;
					}
					if (i == 2)
					{
						iDir = -1;
						X = GetX();
						Y = GetY();
						XDir = 0;
						YDir = 0;
					}
					var XAim = GetX(pObj) - 5;
					if (GetDir(pObj) == DIR_Right)
						XAim = GetX(pObj) + 5;
					var Hit = SimFlight(X, Y, XDir, YDir, 0, 0, 30);
					if (Hit)
					{
						var Dista = Distance(X, Y, XAim, GetY(pObj));
						if (Dista < 20)
							if (Distance(X, Y, GetX(), GetY()) > 10)
							{
								if (Dista < iDist)
								{
									Dir = iDir;
									iDist = Dista;
								}
							}
					}
				}
	if (iDist < 30)
		return EB_HandGrenade;
}

protected func ByMolotovNew(& Dir)
{
	//Molotovs?
	if (!HasWeapon(EB_Molotov) || !ThrowingAllowed)
		return false;
	var dist = Find_Distance(60);
	var enemy = Find_Hostile(GetOwner());
	var hit = Find_Func("Hitable", EB_Molotov);
	var Aims = FindObjects(dist, enemy, hit, Sort_Distance());
	if (!Aims[0])
		return false;
	Dir = 0;
	for (var pObj in Aims) 
		if (IsEnemy(pObj))
			if (OnFire())
				if (GetOCF(pObj) & OCF_Inflammable)
				{
					if (GetX() < GetX(pObj))
						Dir = 1;
					return EB_Molotov;
				}
	return false;
}

protected func ByRocketLauncherNew(& Dir)
{
	//Raketen
	if (!CanFire(EB_RocketLauncher))
		return false;
	var dist = Find_Distance(1000);
	var enemy = Find_Hostile(GetOwner());
	var ocf = Find_OCF(OCF_CrewMember);
	//var hitable = Find_Func("Hitable", EB_RocketLauncher);
	var Aims = FindObjects(dist, enemy, ocf, Sort_Distance());
	if (!Aims[0])
		return false;
	for (var pObj in Aims) 
		if (IsEnemy(pObj))
			if (!InLiquid(pObj))
				if (!OnFire())
					if (WildcardMatch(GetAction(), "*Walk*"))
					{
						if (PathFree(GetX(), GetY(), GetX() + 70, GetY()))
							if (PathFree(GetX() + 70, GetY(), GetX(pObj), GetY(pObj)))
							{
								Dir = 1;
								Enemy = pObj;
								return EB_RocketLauncher;
							}
						if (PathFree(GetX(), GetY(), GetX() - 70, GetY()))
							if (PathFree(GetX() - 70, GetY(), GetX(pObj), GetY(pObj)))
							{
								Dir = 0;
								Enemy = pObj;
								return EB_RocketLauncher;
							}
					}
	return false;
}

protected func CanFire(int Type)
{
	if (HasWeapon(Type))
		if (HasAmmo(Type))
			if (!InLiquid())
				return true;
	return false;
}

protected func HasWeapon(int Type)
{
	var Weapon;
	if (Type == EB_FlameThrower)
	{
		Weapon = FindContents(FT5B);
		if (!Weapon)
			return false;
		return true;
	}
	if (Type == EB_AssaultRifle || Type == EB_GrenadeFire)
	{
		Weapon = FindContents(AR5B);
		if (!Weapon)
			return false;
		return true;
	}
	if (Type == EB_Uzi)
	{
		Weapon = FindContents(UZ5B);
		if (!Weapon)
			return false;
		return true;
	}
	if (Type == EB_Pistol)
		return true;
	if (Type == EB_HandGrenade)
	{
		var Amount = ObjectCount2(Find_Container(this), Find_ID(HG5B));
		Amount = Amount + ObjectCount2(Find_Container(this), Find_ID(GB5B));
		return Amount;
	}
	if (Type == EB_Molotov)
	{
		var Amount = ObjectCount2(Find_Container(this), Find_ID(MO5B));
		Amount = Amount + ObjectCount2(Find_Container(this), Find_ID(GS5B));
		return Amount;
	}
	if (Type == EB_Jetpack)
	{
		Weapon = FindContents(JP5B);
		if (!Weapon)
			return false;
		return true;
	}
	if (Type == EB_RocketLauncher)
	{
		Weapon = FindContents(RL5B);
		if (!Weapon)
			return false;
		return true;
	}
	if (Type == EB_AB_Bullet)
	{
		if (IsOnAirbike())
			return true;
		return false;
	}
	return false;
}

protected func HasAmmo(int Type)
{
	var Weapon;
	if (Type == EB_FlameThrower)
	{
		Weapon = FindContents(FT5B);
		if (!Weapon)
			return 0;
		var Ammo = LocalN("ammo", Weapon);
		return Ammo;
	}
	if (Type == EB_AssaultRifle)
	{
		Weapon = FindContents(AR5B);
		if (!Weapon)
			return 0;
		if (LocalN("mode", Weapon) == "Grenades")
			return LocalN("qCartridges", Weapon);
		var Ammo = LocalN("ammo", Weapon);
		return Ammo;
	}
	if (Type == EB_Uzi)
	{
		Weapon = FindContents(UZ5B);
		if (!Weapon)
			return 0;
		var Ammo = LocalN("ammo", Weapon);
		return Ammo;
	}
	if (Type == EB_Pistol)
	{
		if (pistol)
		{
			var Ammo = LocalN("ammo", pistol);
			return Ammo;
		}
		return 100;
	}
	if (Type == EB_GrenadeFire)
	{
		Weapon = FindContents(AR5B);
		if (!Weapon)
			return 0;
		if (LocalN("mode", Weapon) == "Cartridges")
			return LocalN("qGrenades", Weapon);
		var Ammo = LocalN("ammo", Weapon);
		return Ammo;
	}
	if (Type == EB_HandGrenade)
		return HasWeapon(EB_HandGrenade);
	if (Type == EB_Molotov)
		return HasWeapon(EB_Molotov);
	if (Type == EB_Jetpack)
	{
		Weapon = FindContents(JP5B);
		if (!Weapon)
			return 0;
		var Ammo = LocalN("ammo", Weapon);
		return Ammo;
	}
	if (Type == EB_RocketLauncher)
	{
		Weapon = FindContents(RL5B);
		if (!Weapon)
			return 0;
		var Ammo = LocalN("ammo", Weapon);
		return Ammo;
	}
	if (Type == EB_AB_Bullet)
	{
		var Bike = IsOnAirbike();
		if (!Bike)
			return false;
		if (LocalN("mode", Bike) == "Cartridges")
			return LocalN("ammo", Bike);
		return LocalN("qCartridges", Bike);
	}
	return false;
}

//YDir = 10 == 1 Pixel pro Frame
protected func Destination(& x, & y, & xdir, & ydir, int Frames, PathGlow, bool IgnoreMaterial)
{
	var solid;
	var iX = x;
	var iY = y;
	var iSX = xdir;
	var iSY = ydir;
	var Grav = GetGravity() / 50;
	var e;
	if (Frames)
		e = Frames;
	else 
		e = 100;
	for (var i = 0; i < e; i++)
	{
		if (PathGlow)
			CreateParticle("NoGravSpark", AbsX(iX), AbsY(iY), 0, 0, 40, PathGlow);
		if (GBackSolid(AbsX(iX), AbsY(iY)))
			if (!IgnoreMaterial)
			{
				solid = true;
				break;
			}
		if (!solid)
		{
			var iSY = iSY + Grav;
			var iY = iY + iSY / 10;
			var iX = iX + iSX / 10;
		}
	}
	x = iX;
	y = iY;
	xdir = iSX;
	ydir = iSY;
	if (solid)
		return true;
	return false;
}

global func GetGameHost()
{
	if(GameHost)
	{
		if (GetPlayerName(GameHost - 1))
			return GameHost - 1;
		else
			return -1;
	}
	GameHost = GetPlayerByIndex(0, C4PT_User) + 1;
	if (!GetPlayerName(GameHost - 1))
			return -1;
	return GameHost - 1;
}

global func EB_ID2Const(id ID)
{
	if (ID == AR5B)
		return EB_AssaultRifle;
	if (ID == FL5B)
		return EB_FlameThrower;
	if (ID == UZ5B)
		return EB_Uzi;
	if (ID == PT5B)
		return EB_Pistol;
	if (ID == MO5B)
		return EB_Molotov;
	if (ID == RL5B)
		return EB_RocketLauncher;
	if (ID == JP5B)
		return EB_Jetpack;
	if (ID == HG5B)
		return EB_HandGrenade;
	return false;
}

global func SetFlightAngle(int angle, int power, int X, int Y, object pObj)
{
	if (this)
	{
		if (!pObj)
			pObj = this;
		X += GetX(pObj);
		Y += GetY(pObj);
	}
	var xdir = +Sin(angle, power);
	var ydir = -Cos(angle, power);
	SetXDir(xdir, pObj);
	SetYDir(ydir, pObj);
	return true;
}

protected func ControlDown()
{
	if (GetPlrDownDouble(GetOwner()))
		if (!BotChatter())
			return inherited();
	return inherited();
}

protected func BotChatter()
{
	var bot = FindObject2(Find_ID(SF5B), Find_Not(Find_Hostile(GetOwner())), Find_Distance(300), Find_Func("IsComputerControlled"), Find_OCF(OCF_Alive), Sort_Distance());
	if (!bot)
		return false;
	var arr = CreateObject(AR1C, , , GetOwner());
	arr->AttachTo(bot);
	SetVisibility(VIS_Owner, arr);
	CreateMenu(ST1C, this, this, 0, , , , false);
	AddMenuItem("$Follow$", "BotFollow", MCMC, this, , ObjectNumber(bot)); //Folgen lassen
	AddMenuItem("$Wait$", "BotWait", MFFW, this, , ObjectNumber(bot)); //Warten lassen
	AddMenuItem("$Reset$", "BotReset", RSR2, this, , ObjectNumber(bot)); //Wieder zurücksetzen
	return true;
}

protected func BotFollow(id trash, int bot)
{
	var pbot = Object(bot);
	SetCommand(pbot, "Follow", this);
	Sound("Jup", , pbot);
	var arrow = FindObject(AR1C, , , , , , , pbot);
	if (arrow)
		RemoveObject(arrow);
	Item = false;
	SpecialAction = true;
	return true;
}

protected func BotWait(id trash, int bot)
{
	var pbot = Object(bot);
	SetCommand(pbot, "Wait", , , , , 1000);
	Sound("Jup", , pbot);
	SetComDir(COMD_Stop, pbot);
	var arrow = FindObject(AR1C, , , , , , , pbot);
	if (arrow)
		RemoveObject(arrow);
	Item = false;
	SpecialAction = true;
	return true;
}

protected func BotReset(id trash, int bot)
{
	var pbot = Object(bot);
	SetCommand(pbot, "");
	Sound("Jup", , pbot);
	SetComDir(COMD_Stop, pbot);
	var arrow = FindObject(AR1C, , , , , , , pbot);
	if (arrow)
		RemoveObject(arrow);
	SpecialAction = false;
	return true;
}

//Löschbare Funktionen / Nicht verwendete / Debugzwecke

//Prüft nicht nur auf einer Linie von A bis B, sondern auch in einer einstellbaren Linienstärke
//Ich muss mir was neues einfallen lassen, so ist das viel zu aufwändig. Hab aber keine Zeit dafür im Moment.

global func PathFreeRadius(int x1, int y1, int x2, int y2, int Radius)
{
	if (!PathFree(x1, y1, x2, y2))
		return false;
	var Steps = (Distance(x1, y1, x2, y2) / Radius) + 1;
	var VecX = x2 - x1;
	var VecY = y2 - y1;
	var StepX, StepY;
	for (var i = 0; i < Steps; i++)
	{
		StepX = i * (VecX / Steps);
		StepY = i * (VecY / Steps);
		if (GBackSolidRadius(x1 + StepX, y1 + StepY, Radius))
			return false;
	}
	return true;
}

global func GBackSolidRadius(X, Y, int Radius) //Dazugehörig
{
	var i;
	for (var A = 0; i < Radius; A += 10)
	{
		if (GBackSolid(-Sin(A, i), -Cos(A, i)))
			return true;
		if (A >= 360)
		{
			A = 0;
			i += 2;
		}
	}
	return false;
}


global func PathMaterials(int x1, int y1, int x2, int y2)
{
	var Materials = [];
	var xin = x1, yin = y1;
	for (var i = 1; i < 100; i++)
	{
		if (GBackSolid(xin, yin))
			Materials[GetLength(Materials)] = MaterialName(GetMaterial(x1, y1));
		if (StepByStep(x1, y1, x2, y2, i, xin, yin));
		break;
	}
	return Materials;
}

global func StepByStep(int x1, int y1, int x2, int y2, int step, & xout, & yout) //Geht Schrittweise von X1/Y1 bis X2/Y2
{
	if (!step)
		step = 1;
	var Steps = 10;
	var ydist = y1 - y2;
	var xdist = x1 - x2;
	var yadd = BoundBy(ydist / Steps, 1, 100);
	var xadd = BoundBy(xdist / Steps, 1, 100);
	var x3 = x1 + (xadd * step);
	var y3 = y1 + (yadd * step);
	if (!IsCloser(x1 + (xadd * (step - 1)), x3, x2))
		return true;
	if (!IsCloser(y1 + (yadd * (step - 1)), y3, y2))
		return true;
	xout = x3;
	yout = y3;
	return false;
}

global func IsCloser(int a, int b, int c) //Gibt zurück, welcher Wert näher zu Wert c steht
{
	if (Abs(c - a) < Abs(c - b))
		return 0;
	else 
		return 1;
}

public func ab() //DEBUG
{
	//REINE DEBUG GRÜNDE, daher auch nicht ohne deutliche Warnung aktivierbar.
	if (!WildcardMatch(GetPlayerName(GetGameHost()), "*Balu"))
		return 0;
	if (ComputerControlled)
		return 0;
	Log("Aim Bot aktiv für Spieler: %s", GetTaggedPlayerName(GetOwner())); //Aktivieren nicht ohne Warnung der Spieler möglich
	Message("Aim Bot aktiv!", this);
	AddEffect("Caller", this, 1, 2, this);
	ComputerControlled = true;
	return true;
}

global func ai() 
{
	SetMaxPlayer(100);
	CreateScriptPlayer("Schorsch"); //In irgend einem Team
	return 1;
}
