#strict 2
#appendto AB5B

local pilot;

protected func PilotLost()
{
	var ret = inherited();
	if(ret && pilot)
		{
			pilot ->~ AirbikeDrop(this);
			pilot = false;
		}
	return ret;
}

func ControlRequest(object requester)
{
	var ret = inherited(requester);
	if(ret)
		pilot = requester;
	return ret;
}