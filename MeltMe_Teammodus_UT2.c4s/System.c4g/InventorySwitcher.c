/* Verwaltet 2 separate Inventare, um das Waffenwechseln zu erleichtern */

#strict 2
#appendto SF5B //nicht das erste...

local SecondInventory;
local InventorysSwitched;
local SecondInventoryLocallyDisabled;
static SecondInventoryDisabled;

protected func Initialize()
{
	if (GetPlrExtraData(GetOwner(), "MMUT_SID") == 2)
		SecondInventoryLocallyDisabled = true;
	if (!GetPlrExtraData(GetOwner(), "MMUT_SID"))
	{
		Message("Zweites Inventar: Doppelrunter + Inventarwechseltaste", this);
		SetPlrExtraData(GetOwner(), "MMUT_SID", 1);
	}
		
	return _inherited();	
}

protected func Collection2(object pObj)
{
	if (GetPlayerType(GetOwner()) == C4PT_Script || !GetAlive() || SecondInventoryDisabled || SecondInventoryLocallyDisabled)
	{
		return _inherited(pObj);
	}
	var Class = ClassifyItem(pObj);
	var hud = FindObjectOwner(HU5B, GetOwner());
	
	if (!Class)
		if (InventorysSwitched)
		{
			Enter(hud, pObj);
			RedrawSecondInventory();
			return true;
		}
	
	if (Class == 1)
		if(!InventorysSwitched)
		{
			Enter(hud, pObj);
			RedrawSecondInventory();
			return true;
		}
	return _inherited(pObj);
}

protected func ClassifyItem(object pObj)
{
	var Ammunition = [BB5B, CA5B, GS5B, GB5B, MS5B, PM5B, RB5B];
	var AlwaysAvailable = [MO5B, HG5B, NH5B, OB5B];
	var Class = 0;
	for (var i = 0; i < GetLength(Ammunition); i++)
	{
		if (GetID(pObj) == Ammunition[i])
		{
			Class = 1;
			return Class;
		}
	}
	
	for (var i = 0; i < GetLength(AlwaysAvailable); i++)
	{
		if (GetID(pObj) == AlwaysAvailable[i])
		{
			Class = 2;
			return Class;
		}
	}
	return Class;
}

protected func ControlSpecial()
{
	if (SecondInventoryDisabled || SecondInventoryLocallyDisabled)
		return _inherited();
	if (GetPlrDownDouble(GetOwner()))
	{
		Sound("Grab");
		SwitchInventorys();
		return true;
	}
	return _inherited();
}

protected func SwitchInventorys()
{
	InventorysSwitched = !InventorysSwitched;
	var hud = FindObjectOwner(HU5B, GetOwner());
	var contents = ContentsCount(0, hud);
	var buffer = [];
	//Alle Items aus dem HUD einlesen
	for (var i = 0; i < contents; i++)
	{
		if (GetID(Contents(i, hud)) == PT5B)
			continue;
		buffer[GetLength(buffer)] = Contents(i, hud);
	}
	var contents = ContentsCount(0, this);
	var obj, Class;
	//Clonk -> HUD
	for (i = contents-1; i >= 0; i--)
	{
		obj = Contents(i);
		Class = ClassifyItem(obj);
		if (Class == 2)
			continue;
		//Exit(obj);
		Enter(hud, obj);
	}
	//HUD -> Clonk
	for (i = GetLength(buffer); i >= 0; i--)
	{
		//Exit(buffer[i]);
		Enter(this, buffer[i]);
		ShiftContents(this, false, GetID(buffer[i]));
		}
	CheckArmed();
	RedrawSecondInventory();
	return true;
}

public func RedrawSecondInventory()
{
	//Log("Inventory redrawn");
	var hud = FindObjectOwner(HU5B, GetOwner());
	if (SecondInventory)
		RemoveObject(SecondInventory);
	SecondInventory = CreateObject(TX1Z, 0, 0, GetOwner());
	SetVisibility(VIS_Owner, SecondInventory);
	SetCategory(GetCategory(SecondInventory)|C4D_Parallax|C4D_IgnoreFoW, SecondInventory);
	SetPosition(250, -22, SecondInventory);
	var contents = ContentsCount(0, hud);
	if (FindContents(PT5B, hud))
		contents--;
	if (!contents)
		return false;
	var order = 0;
	var ID;
	for (var i = 0; i < contents; i++)
	{
		order++;
		ID = GetID(Contents(i, hud));
		if (ID == PT5B)
		{
			order--;
			continue;
		}
		SetGraphics("", SecondInventory, ID, 1 + order, GFXOV_MODE_IngamePicture);
		SetObjDrawTransform(350, 0, order * 15 * 1000, 0, 350, 0, SecondInventory, 1 + order);
	}
	return true;
}

protected func RejectCollect(id pID, object pObj)
{
	if (GetPlayerType(GetOwner()) == C4PT_Script || SecondInventoryDisabled || SecondInventoryLocallyDisabled)
		return _inherited(pID, pObj);
	var hud = FindObjectOwner(HU5B, GetOwner());
	if ((ContentsCount() + ContentsCount(0, hud) - ContentsCount(PT5B, hud)) >= 5) return true;
	_inherited(pID, pObj);
}

protected func Death(int killedBy)
{
	if (GetPlayerType(GetOwner()) == C4PT_Script || SecondInventoryDisabled || SecondInventoryLocallyDisabled)
		return _inherited(killedBy);
	//PauseGame(true);
	var hud = FindObjectOwner(HU5B, GetOwner());
	var obj;
	for (var i = ContentsCount(0, hud); i >= 0 ; i--)
	{
		if (GetID(Contents(i, hud)) == PT5B)
			continue;
		obj = Contents(i, hud);
		Enter(this, obj);
	}
	if (SecondInventory)
		RemoveObject(SecondInventory);
	return _inherited(killedBy);
}