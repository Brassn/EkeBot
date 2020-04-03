//Beginne 1/(1+10^((diff)/400))  (Elo-Formel)

#strict 2
global func CalculatePoints(int a, int b)
{
var diff = b - a;
if (diff < -408)
	return 0;
if (diff >  408)
	return 50;
if (Inside(diff, -408, -392))
	return 4;
if (Inside(diff, -392, -376))
	return 4;
if (Inside(diff, -376, -360))
	return 5;
if (Inside(diff, -360, -344))
	return 5;
if (Inside(diff, -344, -328))
	return 6;
if (Inside(diff, -328, -312))
	return 6;
if (Inside(diff, -312, -296))
	return 7;
if (Inside(diff, -296, -280))
	return 8;
if (Inside(diff, -280, -264))
	return 8;
if (Inside(diff, -264, -248))
	return 9;
if (Inside(diff, -248, -232))
	return 10;
if (Inside(diff, -232, -216))
	return 10;
if (Inside(diff, -216, -200))
	return 11;
if (Inside(diff, -200, -184))
	return 12;
if (Inside(diff, -184, -168))
	return 13;
if (Inside(diff, -168, -152))
	return 14;
if (Inside(diff, -152, -136))
	return 15;
if (Inside(diff, -136, -120))
	return 16;
if (Inside(diff, -120, -104))
	return 17;
if (Inside(diff, -104, -88))
	return 18;
if (Inside(diff, -88, -72))
	return 19;
if (Inside(diff, -72, -56))
	return 20;
if (Inside(diff, -56, -40))
	return 21;
if (Inside(diff, -40, -24))
	return 22;
if (Inside(diff, -24, -8))
	return 23;
if (Inside(diff, -8, 8))
	return 25;
if (Inside(diff, 8, 24))
	return 26;
if (Inside(diff, 24, 40))
	return 27;
if (Inside(diff, 40, 56))
	return 28;
if (Inside(diff, 56, 72))
	return 29;
if (Inside(diff, 72, 88))
	return 30;
if (Inside(diff, 88, 104))
	return 31;
if (Inside(diff, 104, 120))
	return 32;
if (Inside(diff, 120, 136))
	return 33;
if (Inside(diff, 136, 152))
	return 34;
if (Inside(diff, 152, 168))
	return 35;
if (Inside(diff, 168, 184))
	return 36;
if (Inside(diff, 184, 200))
	return 37;
if (Inside(diff, 200, 216))
	return 38;
if (Inside(diff, 216, 232))
	return 39;
if (Inside(diff, 232, 248))
	return 39;
if (Inside(diff, 248, 264))
	return 40;
if (Inside(diff, 264, 280))
	return 41;
if (Inside(diff, 280, 296))
	return 41;
if (Inside(diff, 296, 312))
	return 42;
if (Inside(diff, 312, 328))
	return 43;
if (Inside(diff, 328, 344))
	return 43;
if (Inside(diff, 344, 360))
	return 44;
if (Inside(diff, 360, 376))
	return 44;
if (Inside(diff, 376, 392))
	return 45;
if (Inside(diff, 392, 408))
	return 45;
}
