#strict 2

// Variation of PathFree / ForLine from Standard.cpp

global func PathDigCount(int xStart, int yStart, int xEnd, int yEnd, onFail)
{
	if(this)
	{
		return ROCK->PathDigCount(xStart, yStart, xEnd, yEnd);
	}

	var x, y;
	var d, aincr, bincr;
	var dx = Abs(xEnd - xStart);
	var dy = Abs(yEnd - yStart);

	var digCount = 0;

	x = xStart;
	y = yStart;
	if(GBackSolid(x, y))
	{
		if(GetMaterialVal("DigFree", "Material", GetMaterial(x, y)))
		{
			++digCount;
		}
		else
		{
			return onFail;
		}
	}

	if(Abs(xEnd - xStart) < Abs(yEnd - yStart))
	{
		var xincr = 2 * (xEnd > xStart) - 1;

		d = 2 * dx - dy;

		aincr = 2 * (dx - dy);
		bincr = 2 * dx;

		if(yEnd >= yStart)
		{
			for(y = yStart + 1; y <= yEnd; ++y)
			{
				if(d >= 0)
				{
					x += xincr;
					d += aincr;
				}
				else
				{
					d += bincr;
				}

				if(GBackSolid(x, y))
				{
					if(GetMaterialVal("DigFree", "Material", GetMaterial(x, y)))
					{
						++digCount;
					}
					else
					{
						return onFail;
					}
				}
			}
		}
		else
		{
			for(y = yStart - 1; y >= yEnd; --y)
			{
				if(d >= 0)
				{
					x += xincr;
					d += aincr;
				}
				else
				{
					d += bincr;
				}

				if(GBackSolid(x, y))
				{
					if(GetMaterialVal("DigFree", "Material", GetMaterial(x, y)))
					{
						++digCount;
					}
					else
					{
						return onFail;
					}
				}
			}
		}
	}
	else
	{
		var yincr = 2 * (yEnd > yStart) - 1;

		d = 2 * dy - dx;

		aincr = 2 * (dy - dx);
		bincr = 2 * dy;

		if(xEnd >= xStart)
		{
			for(x = xStart + 1; x <= xEnd; ++x)
			{
				if(d >= 0)
				{
					y += yincr;
					d += aincr;
				}
				else
				{
					d += bincr;
				}

				if(GBackSolid(x, y))
				{
					if(GetMaterialVal("DigFree", "Material", GetMaterial(x, y)))
					{
						++digCount;
					}
					else
					{
						return onFail;
					}
				}
			}
		}
		else
		{
			if(GBackSolid(x, y))
			{
				if(GetMaterialVal("DigFree", "Material", GetMaterial(x, y)))
				{
					++digCount;
				}
				else
				{
					return onFail;
				}
			}
			for(x = xStart - 1; x >= xEnd; --x)
			{
				if(d >= 0)
				{
					y += yincr;
					d += aincr;
				}
				else
				{
					d += bincr;
				}

				if(GBackSolid(x, y))
				{
					if(GetMaterialVal("DigFree", "Material", GetMaterial(x, y)))
					{
						++digCount;
					}
					else
					{
						return onFail;
					}
				}
			}
		}
	}

	return digCount;
}