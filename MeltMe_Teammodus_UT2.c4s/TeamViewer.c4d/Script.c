/*-- Hängt sich an Clonks dran --*/

#strict 2

local OverlaysUsed;
local Target;

protected func Initialize() 
{
  return true;
}

public func SetTarget(object pClonk)
{
    if(!pClonk)
        {
            RemoveObject();
            return false;
        }
    Target = pClonk;
    AddEffect("Report", Target, 1, 0, this);
    SetAction("Attach", pClonk);
    SetVisibility(VIS_Allies | VIS_God);
    UpdateEnergyView();
    UpdateView();
    return true;
}

protected func UpdateEnergyView()
{
    if(!GetAlive(Target))
        return RemoveObject();
    SetGraphics("Border", this, GetID(), 1, GFXOV_MODE_Base);
    SetGraphics("Filling", this, GetID(), 2, GFXOV_MODE_Base);
    var energy = (GetEnergy(Target) * 100000) / GetPhysical("Energy", 0, Target);
    SetObjDrawTransform(1000, 0, - 8000, 0, 1000, -25000, this, 1);
    SetObjDrawTransform(10 * energy, 0, - 6000, 0, 1000, -24000, this, 2);
    SetClrModulation(RGBa(255, 255, 255, 150), this, 1);
    SetClrModulation(RGBa((100 - energy) * 2, 2 * energy, 0, 100), this, 2);
    return true;
}

protected func FxReportDamage(object pTarget, int iEffectNumber, int iDmgEngy, int iCause)
{
    ScheduleCall(this, "UpdateView",1);
    ScheduleCall(this, "UpdateEnergyView",1);
    return iDmgEngy;    
}

//Overlay 1 = Energieborder
//Overlay 2 = Energiebalken
//Overlay >2 = Verbleibende Neos
public func UpdateView()
{
    //Neos
    var x = ContentsCount(NH5B, Target);
    var i;
    for(i = 3;i <= OverlaysUsed;i++)
        SetGraphics(, this, , i);
    for(i = 3;i <= x + 2;i++)
        {
            SetGraphics(0, this, NH5B, i, GFXOV_MODE_Base);
            SetObjDrawTransform(900, 0, - 4000 + ((i-3) * 7000), 0, 900, -17000, this, i);
            SetClrModulation(RGBa(255,255,255,150), this, i);
        }
    OverlaysUsed = x + 2;
    //Energiebalken
    //...
    return true;
}

protected func AttachTargetLost()
{
    RemoveObject();
}
