/* Kleiner Hack an den Waffen, damit Munition auch aus dem Zweit-Inventar geladen wird. */

#strict 2
#appendto AR5B
#appendto FT5B
#appendto PT5B
#appendto RL5B
#appendto SG5B
#appendto UZ5B

protected func FindContents(id idType, object pObj)
{
	var inh = inherited(idType, pObj);
	if (!inh)
	{
		var hud = FindObjectOwner(HU5B, GetOwner(Contained()));
		inh = inherited(idType, hud);
	}
	return inh;
}

protected func RemoveObject(object pObj, bool fEjectContents)
{
	var inh = inherited(pObj, fEjectContents);
	if(Contained())
		Contained()->~RedrawSecondInventory();
	return inh;
}