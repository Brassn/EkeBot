/* Verwaltet 2 separate Inventare, um das Waffenwechseln zu erleichtern */

#strict 2

static SecondInventoryDisabled;

protected func Activate(int player)
{
	if (SecondInventoryDisabled)
	{
		MessageWindow("Zweites Inventar ist global deaktiviert", player, GetID(), "Einstelung Zweites Inventar");
		return false;
	}
	var pObj = GetCursor(player);
	SettingsChooser(pObj);
	return true;
}

protected func SettingsChooser(object pObj)
{
	CreateMenu(GetID(), pObj, this, 0, GetName(this), 0, 1);
	AddMenuItem("aktivieren", "ActivateInv", TU1Z, pObj, 0, GetOwner(pObj));
	AddMenuItem("deaktivieren", "DeactivateInv", TD1Z, pObj, 0, GetOwner(pObj));
}

public func ActivateInv(id trash, int player)
{
	var crsr = GetCursor(player);
	LocalN("SecondInventoryLocallyDisabled", crsr) = false;
	SetPlrExtraData(player, "MMUT_SID", 1);
	crsr ->~ SwitchInventorys();
	crsr ->~ SwitchInventorys();
}

public func DeactivateInv(id trash, int player)
{
	var crsr = GetCursor(player);
	LocalN("SecondInventoryLocallyDisabled", crsr) = true;
	SetPlrExtraData(player, "MMUT_SID", 2);
	var hud = FindObjectOwner(HU5B, player);
	var contents = ContentsCount(0, hud);
	for (var i = 0; i < contents; i++)
	{
		if (GetID(Contents(i, hud)) == PT5B)
			continue;	
		Enter(crsr, Contents(0, hud));
	}
	crsr ->~ CheckArmed();
	crsr ->~ RedrawSecondInventory();
	return true;
}