/*-- Freunde sehen Informationen über den Clonk --*/

#strict 2
#appendto SF5B

local TeamViewer;

protected func Initialize()
{
    TeamViewer = CreateObject(TV1Z,,,GetOwner());
    TeamViewer -> SetTarget(this);
    return inherited();
}

public func GetTeamViewer()
{
    return TeamViewer;
}

protected func Collection2(object pObj)
{
    var ret = _inherited(pObj);
    GetTeamViewer() -> UpdateView();
    return ret;
}

protected func Ejection(object pObj)
{
    var ret = _inherited(pObj);
    GetTeamViewer() -> UpdateView();
    return ret;
}
