#strict 2
#appendto BU5B

local OldX;
local OldY;

static TracingActivated;

protected func Launch(object weapon, int dir, clonk)
{
    if(TracingActivated)
         AddEffect("Tracing", this, 1, 1, this);
    OldX = GetX();
    OldY = GetY();
    return inherited(weapon, dir, clonk);
}

local line;

protected func FxTracingTimer()
{
    if(line)
        RemoveObject(line);
    if(OldX + OldY > 0 && ObjectCount(TA1Z))
        line = DrawLine(OldX, OldY, GetX(), GetY());
    return true;
}

protected func FxTracingStart()
{
    FxTracingTimer();
    return true;
}

protected func Destruction()
{
    if(!TracingActivated)
        return true;
    if(line)
        RemoveObject(line);
    if(LastVictim)
        DrawLine(OldX, OldY, GetX(LastVictim), GetY());
    else
      if(OldX + OldY > 0 && ObjectCount(TA1Z))
          DrawLine(OldX, OldY, GetX(), GetY());
    return true;
}

local LastVictim;

protected func HitObject()
{
    var foo = inherited();
    if(foo)
       return true;
    LastVictim = false;
    return false;
}

protected func HitCreature(victim)
{
    LastVictim = victim;
    return inherited(victim);
}

protected func HitTin(victim)
{
    LastVictim = victim;
    return inherited(victim);
}

protected func HitBalloon(victim)
{
    LastVictim = victim;
    return inherited(victim);
}
