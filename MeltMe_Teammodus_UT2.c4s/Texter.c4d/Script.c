/*-- Schrift --*/

#strict 2

local TextSet;
local Overlays;
local UsedFont;
local TargetClr;
local UsedFade;

protected func Initialize()
{
    Overlays = [];
    return true;
}

//Wenn pObj angegeben ist, dann gelten x & y lokal zu diesem Objekt.
//Wenn pObj nicht angegeben ist, gelten x & y lokal zum aufrufenden Objekt, wenn vorhanden.
//Wenn Parallax ungleich 0, dann wird der Text in Relation zum Bildschirm gezeigt, für den angegebenen Spieler -1
//SourceID gibt das Quell-Font an, bei 0 wird Standard gesetzt
//Color gibt Farbmodulation an, bei 0 Schwarz.
//FadeEffect wählt den Effekt, der beim Erscheinen und gehen des Textes angewandt wird 
//FadeEffect: 0 = Standard, 1 = Gewöhnlicher FadeIn, 2 = Links nach Rechts
global func FontMessage(string Text, object pObj, int x, int y, bool Parallax, int player, id SourceID, Color, int FadeEffect)
{
    if(!FadeEffect)
        FadeEffect = 2;
    if(!SourceID)
        SourceID = ZF1Z;
    if(!pObj && this)
        pObj = this;
    var TxtObj = CreateObject(TX1Z, 0, 0, NO_OWNER);
    TxtObj -> SetFont(SourceID);
    TxtObj -> SetTxColor(Color);
    TxtObj -> SetFadeEffect(FadeEffect);
    if(player)
    {
        SetOwner(player - 1, TxtObj);
        SetVisibility(VIS_Owner, TxtObj);    
    }
    
    if(pObj)
    {
        TxtObj -> SetVertex(0, 0, x);
        TxtObj -> SetVertex(0, 1, y);
        TxtObj -> SetAction("Attach", pObj);
    }
    else
    {
        SetPosition(x, y, TxtObj, true); //Evtl. Lokal
        if(Parallax)
            SetCategory(GetCategory(TxtObj)|C4D_Parallax|C4D_IgnoreFoW, TxtObj);
    }
    TxtObj -> SetText(Text);
    return true;
}

public func SetFadeEffect(int Fade)
{
    UsedFade = Fade;
    return true;
}

public func SetFont(id SourceID)
{
    UsedFont = SourceID;
    return true;
}

public func SetTxColor(Color)
{
    if(!Color)
        Color = RGBa(0, 0, 0, 0);
    TargetClr = Color;
    return true;
}

protected func SetText(string Text)
{
    TextSet = Text;
    var X;
    var CustomX;
    var CustomY;
    for(var i = 0; i < GetLength(Text); i++)
    {
        Overlays[GetLength(Overlays)] = Ascii2Str(GetChar(Text, i));
        var test = CreateObject(UsedFont);
        if(SetGraphics(Ascii2Str(GetChar(Text, i)), this, UsedFont, i+1, GFXOV_MODE_Base))
        {
            SetClrModulation(RGBa(255, 255, 255, 255), this, i+1);
            var nr = AddEffect(Format("TxtAppear%d", UsedFade), this, 1, 2, this);
            EffectVar(0, this, nr) = i+1;
            CustomX = test -> GetWidth(GetChar(Text, i));
            CustomY = test -> GetHeight(GetChar(Text, i));
            if(GetChar(Text, i) == 32)
                CustomX = test -> GetWidth(73);
            EffectVar(1, this, nr) = X;
            EffectVar(2, this, nr) = CustomY * -1;
            X += CustomX + 3;
        }
       RemoveObject(test);
    }
    return true;
}

protected func FxTxtAppear1Timer(object pObj, int Number)
{
    var OverlayNr = EffectVar(0, pObj, Number);
    var AimX = EffectVar(1, pObj, Number);
    var AimY = EffectVar(2, pObj, Number);
    var TC = EffectVar(3, pObj, Number)++;
    SetObjDrawTransform(1000, 0, AimX * 1000, 0, 1000, AimY * 1000, pObj, OverlayNr);
    var r = GetRGBaValue(TargetClr, 1);
    var g = GetRGBaValue(TargetClr, 2);
    var b = GetRGBaValue(TargetClr, 3);
    SetClrModulation(RGBa(r, g, b, 255 - (TC * (255 / 10))), pObj, OverlayNr);
    if(TC >= 10)
    {
        var nr = AddEffect("TxtWait", this, 1, 150, this);
        EffectVar(0, this, nr) = OverlayNr;
        EffectVar(1, this, nr) = AimX;
        EffectVar(2, this, nr) = AimY;
        return -1;
    }
    return true;
}

protected func FxTxtAppear2Timer(object pObj, int Number)
{
    var OverlayNr = EffectVar(0, pObj, Number);
    var AimX = EffectVar(1, pObj, Number);
    var AimY = EffectVar(2, pObj, Number);
    var TC = EffectVar(3, pObj, Number)++;
    SetObjDrawTransform(1000, 0, (AimX * 1000) - 50000 + (TC * (50000 / 10)), 0, 1000, (AimY * 1000), pObj, OverlayNr);
    var r = GetRGBaValue(TargetClr, 1);
    var g = GetRGBaValue(TargetClr, 2);
    var b = GetRGBaValue(TargetClr, 3);
    SetClrModulation(RGBa(r, g, b, 255 - (TC * (255 / 10))), pObj, OverlayNr);
    if(TC >= 10)
    {
        var nr = AddEffect("TxtWait", this, 1, 150, this);
        EffectVar(0, this, nr) = OverlayNr;
        EffectVar(1, this, nr) = AimX;
        EffectVar(2, this, nr) = AimY;
        return -1;
    }
    return true;
}

protected func FxTxtWaitTimer(object pObj, int Number)
{
    var OverlayNr = EffectVar(0, pObj, Number);
    var AimX = EffectVar(1, pObj, Number);
    var AimY = EffectVar(2, pObj, Number);
    var nr = AddEffect(Format("TxtDisappear%d", UsedFade), this, 1, 2, this);
    EffectVar(0, this, nr) = OverlayNr;
    EffectVar(1, this, nr) = AimX;
    EffectVar(2, this, nr) = AimY;
    return -1;
}

protected func FxTxtDisappear1Timer(object pObj, int Number)
{
    var OverlayNr = EffectVar(0, pObj, Number);
    var AimX = EffectVar(1, pObj, Number);
    var AimY = EffectVar(2, pObj, Number);
    var TC = EffectVar(3, pObj, Number)++;
    SetObjDrawTransform(1000, 0, AimX * 1000, 0, 1000, AimY * 1000, pObj, OverlayNr);
    var r = GetRGBaValue(TargetClr, 1);
    var g = GetRGBaValue(TargetClr, 2);
    var b = GetRGBaValue(TargetClr, 3);
    SetClrModulation(RGBa(r, g, b, 0 + (TC * (255 / 10))), pObj, OverlayNr);
    if(TC >= 10)
        {
            ScheduleCall(pObj, "CheckForRemove", 1);
            return -1;
        }
    return true;
}

protected func FxTxtDisappear2Timer(object pObj, int Number)
{
    var OverlayNr = EffectVar(0, pObj, Number);
    var AimX = EffectVar(1, pObj, Number);
    var AimY = EffectVar(2, pObj, Number);
    var TC = EffectVar(3, pObj, Number)++;
    SetObjDrawTransform(1000, 0, (AimX * 1000) + (TC * (50000 / 10)), 0, 1000, (AimY * 1000), pObj, OverlayNr);
    var r = GetRGBaValue(TargetClr, 1);
    var g = GetRGBaValue(TargetClr, 2);
    var b = GetRGBaValue(TargetClr, 3);
    SetClrModulation(RGBa(r, g, b, 0 + (TC * (255 / 10))), pObj, OverlayNr);
    if(TC >= 10)
        {
            ScheduleCall(pObj, "CheckForRemove", 1);
            return -1;
        }
    return true;
}

protected func CheckForRemove()
{
    if(!GetEffect("TxtDisappear", this))
        RemoveObject(this);
}

global func Ascii2Str(int Nr, bool C2C)
{
    var ascii = [];
    ascii[33] = "Ausrufe";
    ascii[63] = "Frage";

    ascii[48] = "0";
    ascii[49] = "1";
    ascii[50] = "2";
    ascii[51] = "3";
    ascii[52] = "4";
    ascii[53] = "5";
    ascii[54] = "6";
    ascii[55] = "7";
    ascii[56] = "8";
    ascii[57] = "9";

    //ascii[32] = " ";
    ascii[65] = "A";
    ascii[66] = "B";
    ascii[67] = "C";
    ascii[68] = "D";
    ascii[69] = "E";
    ascii[70] = "F";
    ascii[71] = "G";
    ascii[72] = "H";
    ascii[73] = "I";
    ascii[74] = "J";
    ascii[75] = "K";
    ascii[76] = "L";
    ascii[77] = "M";
    ascii[78] = "N";
    ascii[79] = "O";
    ascii[80] = "P";
    ascii[81] = "Q";
    ascii[82] = "R";
    ascii[83] = "S";
    ascii[84] = "T";
    ascii[85] = "U";
    ascii[86] = "V";
    ascii[87] = "W";
    ascii[88] = "X";
    ascii[89] = "Y";
    ascii[90] = "Z";
    
    ascii[97] = "Sa";
    ascii[98] = "Sb";
    ascii[99] = "Sc";
    ascii[100] = "Sd";
    ascii[101] = "Se";
    ascii[102] = "Sf";
    ascii[103] = "Sg";
    ascii[104] = "Sh";
    ascii[105] = "Si";
    ascii[106] = "Sj";
    ascii[107] = "Sk";
    ascii[108] = "Sl";
    ascii[109] = "Sm";
    ascii[110] = "Sn";
    ascii[111] = "So";
    ascii[112] = "Sp";
    ascii[113] = "Sq";
    ascii[114] = "Sr";
    ascii[115] = "Ss";
    ascii[116] = "St";
    ascii[117] = "Su";
    ascii[118] = "Sv";
    ascii[119] = "Sw";
    ascii[120] = "Sx";
    ascii[121] = "Sy";
    ascii[122] = "Sz";
    ascii[60] = "<";
    ascii[47] = "/";
    
    if(C2C)
        if(Nr>96)
           if(ascii[Nr-32])
              return ascii[Nr-32];
    return ascii[Nr];
}
