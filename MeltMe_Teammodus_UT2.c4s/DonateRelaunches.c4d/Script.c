#strict 2

static PlayerRelaunches;

protected func Activate(int player)
{
	var pObj = GetCursor(player);
	PlayerChooser(pObj);
	return true;
}

protected func PlayerChooser(object pObj)
{
	if (PlayerRelaunches[GetPlayerID(GetOwner(pObj))] <= 0)
	{
		MessageWindow("Du hast keine Relaunches zum verschenken", GetOwner(pObj), GetID(), "");
		return false;
	}
	CreateMenu(GetID(), pObj, this, 0, "Relaunch schenken:", 0, 1, true);
	var icons = [SNB0, SNB1, SNB2, SNB3, SNB4, SNB5, SNB6, SNB7, SNB8, SNB9, FLNT, EFLN, ROCK, GOLD, GUNP, STFN, TRP1];
	var prefix;
	var index;
	for (var i; i < GetPlayerCount(); i++)
	{
		index = GetPlayerByIndex(i);
		if (index == GetOwner(pObj))
			continue;
		if (Hostile(GetOwner(pObj), index))
			prefix = "Feind:  ";
		else 
			prefix = "Freund: ";
		AddMenuItem(Format("%s%s", prefix, GetTaggedPlayerName(index)), "MoveRelaunch", icons[index], pObj, 0, GetOwner(pObj));
	}
	return true;
}

protected func MoveRelaunch(id To, int From)
{
	var icons = [SNB0, SNB1, SNB2, SNB3, SNB4, SNB5, SNB6, SNB7, SNB8, SNB9, FLNT, EFLN, ROCK, GOLD, GUNP, STFN, TRP1];
	for (var i; i <= GetLength(icons); i++)
		if (icons[i] == To)
			break;
	if (i == GetLength(icons))
		return false;
	var ToID = GetPlayerID(i);
	var FromID = GetPlayerID(From);
	if (PlayerRelaunches[FromID] <= 0)
		return false;
	ShowMessage(Format("%s hat %s einen <c %x>Relaunch geschenkt!</c>", GetTaggedPlayerName(From), GetTaggedPlayerName(i), RGB(0, 255, 0)));
	PlayerRelaunches[FromID]--;
	PlayerRelaunches[ToID]++;
	SetScoreboardData(ToID, 1, Format("%d", PlayerRelaunches[ToID]), PlayerRelaunches[ToID]);
	SetScoreboardData(FromID, 1, Format("%d", PlayerRelaunches[FromID]), PlayerRelaunches[FromID]);
	SortScoreboard(1, true);
	return true;
}
