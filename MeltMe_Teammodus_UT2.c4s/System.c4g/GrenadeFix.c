/*-- Granaten haben auch einen CountDown --*/

#strict 2
#appendto GR5B

protected func Initialize() 
{
  AddEffect("TimeOut", this, 1, 300, this);
  return _inherited();
}

protected func FxTimeOutTimer()
{
    BlowUp();
    return -1;
}
