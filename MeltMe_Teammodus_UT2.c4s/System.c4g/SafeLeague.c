/*-- Entfernt gewisse Regeln bei Ligaspielen--*/

#strict 2

#appendto TS1Z
#appendto DR1Z

protected func Initialize()
{
	if (GetLeague())
		return RemoveObject();
	return true;
}
