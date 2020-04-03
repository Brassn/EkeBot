/* Waffen-Entferner */

#strict 2
#appendto AR5B
#appendto FT5B
#appendto SG5B

public func Departure(object clonk)
{
	if (GetID(clonk) == HU5B)
		return _inherited(clonk);
	if (!GetAlive(clonk))
		RemoveObject();
	AddEffect("FadeOut", this, 1, 5, this);
	return _inherited(clonk);
}

protected func FxFadeOutTimer(object pTarget, int EffectNumber)
{
	var alpha = ++EffectVar(0, pTarget, EffectNumber);
	SetClrModulation(RGBa(255, 255, 255, alpha));
	if (alpha >= 255)
	{
		RemoveObject(pTarget);
		return -1;
	}
	return true;
}

public func Entrance(pContainer)
{
	RemoveEffect("FadeOut", this);
	SetClrModulation(RGBa(255, 255, 255, 0), this);
	return _inherited(pContainer);
}
