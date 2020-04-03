#strict 2

static PlayerRelaunches;

protected func Activate(int player) 
{
    if(GetGameHost() != player)
        MessageWindow("Host kann Teams der Spieler wechseln", player, GetID());
    else
    {
        var pObj = GetCursor(player);
        PlayerChooser(pObj);
    }
    return true;
}

protected func SwitchPlayer(id menuid, int player)
{
    var icons = [SNB0,SNB1,SNB2,SNB3,SNB4,SNB5,SNB6,SNB7,SNB8,SNB9,FLNT,EFLN,ROCK,GOLD,GUNP,STFN,TRP1];
    for(var i; i < GetLength(icons); i++)
        if(icons[i] == menuid)
            break;
    
    var playerID = GetPlayerID(player);
    SetPlayerTeam(player, i);
    SetScoreboardData(GetPlayerID(player), SBRD_Caption, GetTaggedPlayerName(player), GetPlayerID(player));
    var before = PlayerRelaunches[playerID];
    PlayerRelaunches[playerID]++;
    var cursor = GetCursor(player);
    /*var backpack = [];
    for(var e; e < ContentsCount(,cursor); e++)
        backpack[GetLength(backpack)] = Contents(e, cursor);*/
    RemoveObject(cursor , false);
    PlayerRelaunches[playerID] = before;
    cursor = GetCursor(player);
    /*for(var u; u < ContentsCount(, cursor); u++)
        RemoveObject(Contents(u, cursor));
    for(var w; w < GetLength(backpack); w++)
        Enter(cursor, backpack[w]);*/
    ShowMessage(Format("<c %x>Spieler %s wurde vom Spielhost in Team %s verschoben!</c>", RGB(0,255,0), GetPlayerName(player), GetTeamName(i)));
    PlayerMessage(player, Format("<c %x>Aktuelles Team: </c><c %x>%s</c>", RGB(,255,), GetTeamColor(i), GetTeamName(i)), cursor);
    return true;
}

protected func TeamChooser(id trash, int player)
{
    var pObj = GetCursor(GetGameHost());
    CreateMenu(GetID(), pObj, this, 0, "Team auswählen:", 0, 1);
    var icons = [SNB0,SNB1,SNB2,SNB3,SNB4,SNB5,SNB6,SNB7,SNB8,SNB9,FLNT,EFLN,ROCK,GOLD,GUNP,STFN,TRP1];
    for(var i; i < GetTeamCount(); i++)
        {
            if(GetTeamByIndex(i) != GetPlayerTeam(player))
                AddMenuItem(Format("%s", GetTeamName(GetTeamByIndex(i))), "SwitchPlayer", icons[GetTeamByIndex(i)], pObj, 0, player);
        }
}

protected func PlayerChooser(object pObj)
{
    CreateMenu(GetID(), pObj, this, 0, "Spieler wählen:", 0, 1);
    var icons = [SNB0,SNB1,SNB2,SNB3,SNB4,SNB5,SNB6,SNB7,SNB8,SNB9,FLNT,EFLN,ROCK,GOLD,GUNP,STFN,TRP1];
    //var prefix;
    var index;
    for(var i;i < GetPlayerCount();i++)
        {
            var index = GetPlayerByIndex(i);
            AddMenuItem(Format("%s", GetTaggedPlayerName(index)), "TeamChooser", icons[i], pObj, 0, index);
        }
    return true;
}
