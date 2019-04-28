var x = [1,2,3,4,5];

x = [i in x] i*2;

[i in x] i *= 2;

forall i in x {
	i *= 2;
}

writeln(x);
