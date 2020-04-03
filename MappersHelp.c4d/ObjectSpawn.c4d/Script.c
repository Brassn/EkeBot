/*-- Objekt Spawn --*/

#strict 2

local Item;
local Hight;
local Timer;

public func Initialize() 
{
	Graphics();
	SetAction("Up");
	return true;
}

public func SetItem(id ItemID)
{
	Item = ItemID;
	if (Contents(0))
		RemoveObject(Contents(0));
	CreateContents(Item);
	Graphics();
	return true;
}

protected func Effect()
{
	if (Random(2))
		CreateParticle("NoGravSpark", RandomX(-5, 5), RandomX(5, 10), 0, -5, 25, RGBa(210, 210, 255, 100));
	return true;
}

protected func Down()
{
	Hight++;
	if (Hight > 5)
		SetAction("Up");
	SetObjDrawTransform(1000, 0, 0, 0, 1000, Hight * 1000, this, 1);
	//SetPosition(GetX(),GetY()+1);
	Effect();
	return true;
}

protected func Up()
{
	Hight--;
	if (Hight < -5)
		SetAction("Down");
	SetObjDrawTransform(1000, 0, 0, 0, 1000, Hight * 1000, this, 1);
	//SetPosition(GetX(),GetY()-1);
	Effect();
	return true;
}

protected func Graphics()
{
	if (Contents())
		SetGraphics(0, 0, Item, 1, 1);
	else 
		SetGraphics(0, 0, 0, 1, 1);
	return true;
}

protected func RejectEntrance(object pIntoObj)
{
	var Mode = EB_ID2Const(Item);
	if (Mode)
		if (pIntoObj->~HasWeapon(Mode))
			return true;
	if (Contents(0))
		if (!pIntoObj->~RejectCollect(Item, Contents(0)))
		{
			SetOwner(GetOwner(pIntoObj), Contents(0));
			Enter(pIntoObj, Contents(0));
			Sound("Grab", false, this);
			Graphics();
			if (!Timer)
				Timer = 720;
			AddEffect("Respawn", this, 1, Timer, this);
		}
	return true;
}

protected func FxRespawnTimer()
{
	CreateContents(Item);
	Graphics();
	return -1;
}

//UI toll, Syntax wie bei Hazard.
global func PlaceSpawnpoint(id ID, int X, int Y, int Time)
{
	var spawn = CreateObject(OS1C, X, Y, NO_OWNER);
	spawn->SetItem(ID);
	LocalN("Timer", spawn) = Time;
	return true;
}

