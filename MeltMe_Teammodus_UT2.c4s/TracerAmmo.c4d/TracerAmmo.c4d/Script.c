#strict 2

static TracingActivated;
static GameHost;

protected func Activate(int player) 
{
    if(GetGameHost() != player)
        MessageWindow("Optional sichtbare Kugeln", player, GetID());
    else
    {
        CreateMenu(GetID(), GetCursor(player), this);
        AddMenuItem("Aktivieren", "ActivateTracing", GetID(), GetCursor(player),,,"");
        AddMenuItem("Deaktivieren", "DeactivateTracing", GetID(), GetCursor(player),,,"");
    }
    return true;
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


public func ActivateTracing()
{
    if(TracingActivated && !ObjectCount(TA1Z))
        CreateObject(TA1Z,10,10,NO_OWNER);
    TracingActivated = true;
}

public func DeactivateTracing()
{
    TracingActivated = false;
    if(ObjectCount(GetID()) >= 1)
        RemoveObject(FindObject(TA1Z));
}
