/*-- Nachrichtenanzeiger --*/

#strict 2

local MyMessage;
static MSGObjects;

/*
public func Initialize() 
{
	return true;
}
*/

global func ShowMessage(string sMessage)
{
	var maxlines = 7;
	var obj = CreateObject(_MSG, 120, 100, NO_OWNER);
	obj -> AddMessage(sMessage);
	if (GetType(MSGObjects) != C4V_Array)
		MSGObjects = [obj];
	else
	{
		var temp1, temp2, sString, alpha;
		alpha = 255 / maxlines;
		temp1 = obj;
		var len = GetLength(MSGObjects);
		for (var i = 0; i < len+1; i++)
		{
			temp2 = MSGObjects[i];
			MSGObjects[i] =  temp1;
			sString = Format("@    <c %x>%s</c>", RGBa(200, 200, 200, BoundBy(255 - (i-2)*alpha, 0, 255)), LocalN("MyMessage", MSGObjects[i]));
			CustomMessage(sString, MSGObjects[i], NO_OWNER, 18, 0, 0, 0, 0, MSG_NoLinebreak);
			SetPosition(0, 120 + i * 15, MSGObjects[i]);
			temp1 = temp2;
			if (i >= maxlines)
			{
				CustomMessage("", MSGObjects[i], NO_OWNER);
				RemoveObject(MSGObjects[i]);
				SetLength(MSGObjects, maxlines);
			}
		}
	}
	return true;
}

public func AddMessage(string sMessage)
{
	MyMessage = sMessage;
	var sString;
	sString = Format("@    <c %x>%s|", RGBa(200, 200, 200, 0), sMessage);
	CustomMessage(sString, this, NO_OWNER, 18, 0, 0, 0, 0, MSG_NoLinebreak);
}