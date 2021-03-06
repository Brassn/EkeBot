#strict 2

global func DrawLine(int x1, int y1, int x2, int y2)
{
    y1 += 5;
    var line = CreateObject(LI1C, AbsX(x1), AbsY(y1), GetController(this));
    line -> Show(x2, y2);
    return line;
}

local x1, y1, x2, y2;

protected func Show(int toX, int toY)
{
    x1 = GetX();
    y1 = GetY();
    x2 = toX;
    y2 = toY;
    var dist = Distance(x1, y1, x2, y2);
    var height = 8;
    SetObjDrawTransform(2000,,0,,1000 * (dist / height) - (1000 * (dist / height) * 10) / 100, 4000 * (dist / height) * -1);
    var ang = Angle(x1, y1, x2 , y2);
    SetR(ang);
}

protected func Initialize()
{
    var eff = AddEffect("FadeOut", this, 1, 1, this);
    EffectVar(0, this, eff) = 0;
    EffectVar(1, this, eff) = 255;
    EffectVar(2, this, eff) = 255;
    EffectVar(3, this, eff) = 255;
    if(ObjectCount(TA1Z) == 2) //In Spielerfarbe
    {
        var clr = GetPlrColorDw(GetOwner(this));
        EffectVar(1, this, eff) = GetRGBaValue(clr, 1);
        EffectVar(2, this, eff) = GetRGBaValue(clr, 2);
        EffectVar(3, this, eff) = GetRGBaValue(clr, 3);
    }
    //SetClrModulation(RGBa(255,0,255,0), this);
}

protected func FxFadeOutTimer(pTarget, EffectNumber)
{
 var alpha = EffectVar(0,pTarget,EffectNumber)+=15;
 var r = EffectVar(1,pTarget,EffectNumber);
 var g = EffectVar(2,pTarget,EffectNumber);
 var b = EffectVar(3,pTarget,EffectNumber);
 SetClrModulation(RGBa(r,g,b,alpha));
 if(alpha >= 255)
  {
   RemoveObject(pTarget);
   return -1;
  }
 return true;
}
