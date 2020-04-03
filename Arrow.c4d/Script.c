/*-- Pfeil --*/

#strict 2

local obj;

protected func Remove()
{
	RemoveObject();
}

protected func AttachTo(object pObj)
{
	obj = pObj;
	SetAction("Attach", obj);
	return true;
}
