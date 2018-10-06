// chpl Source.chpl -o chpl.out
// ./chpl.out --n=500
// ./chpl.out --c=true
// ./chpl.out --a=2
//config const threads = 8;
/* export CHPL_TASKS=fifo; */
config const file = " ";
config const alg = 0;
config const c = false;
config const a = 1;

/* export CHPL_RT_NUM_THREADS_PER_LOCALE 'MAX_PHYSICAL'; */

var _n : int;
enum algoritm{
	Belman_ford = 1,
	Generic,
	SLF,
	LLL,
	Auction
};

proc write_to_file_raw(table){

    for i in 1.._n{
				var writer = open(i: string + ".node", iomode.cw).writer();
				for ((distance, predecessor), node) in zip(table[i,..], 1.._n){
					writer.writeln(node,":",predecessor,"(",distance,")");
				}
		}
}

proc bellman_ford(table, g, source){
	for i in table.domain{
		table[i] = (1 << 30, "-");
	}
	table[source][1] = 0;

	for i in g.domain {
		for j in g {
			var u = j[1] : int;
			var v = j[2] : int;
			var w = j[3] : int;
			if (table[u][1] + w < table[v][1])
			{
				table[v][1] = table[u][1] + w;
				table[v][2] = u : string; // + "-";
			}
			if (table[v][1] + w < table[u][1])
			{
				table[u][1] = table[v][1] + w;
				table[u][2] = v : string; // + "-";
			}
		}
	}
}

proc generic(table, g, source){
	use DistributedDeque;

	for i in table.domain{
		table[i] = (1 << 30, "-");
	}
	table[source][1] = 0;

	var Q = new DistDeque(int);
	var is: [1.._n] bool;
	for i in is{
  	i = false;
	}
	Q.pushBack(source);
	is[source] = true;

	while Q.getSize() != 0{
		var (x, u) = Q.popBack();
		is[u] = false;
		var v: int;
		for j in g{
			//writeln(j);
			if u == j[1] then v = j[2];
			else if u == j[2] then v = j[1];
			else
				continue;

			var w = j[3];
			if table[u][1] + w < table[v][1]{
				table[v][1] = table[u][1] + w;
				table[v][2] = u : string; // +"-";
				if !is[v]{
					Q.pushBack(v);
					is[v] = true;
				}
			}
		}
	}
}

proc slf(table, g, source){
	use DistributedDeque;

	for i in table.domain{
		table[i] = (1 << 30, "-");
	}
	table[source][1] = 0;

	var Q = new DistDeque(int);
	var is: [1.._n] bool;
	for i in is{
  	i = false;
	}
	Q.pushBack(source);
	is[source] = true;

	while Q.getSize() != 0{
		var (x, u) = Q.popBack();
		is[u] = false;
		var v: int;
		for j in g {
			if u == j[1] then v = j[2];
			else if u == j[2] then v = j[1];
			else
				continue;

			var w = j[3];
			if table[u][1] + w < table[v][1]
			{
				table[v][1] = table[u][1] + w;
				table[v][2] = u : string; // + "-";
				if !is[v] {
					if Q.getSize() == 0 then Q.pushBack(v);
					else {
						var (xx, uu) = Q.popBack();
						Q.pushBack(uu);
						if table[v][1] < uu then Q.pushFront(v);
						else
							Q.pushBack(v);
						is[v] = true;
					}
				}
			}
		}
	}
}

proc lll(table, g, source)
{
	/* use DistributedDeque;

	for i in table.domain{
		table[i] = (1 << 30, "-");
	}
	table[source][1] = 0;

	var Q = new DistDeque(int);
	var is: [1.._n] bool;
	for i in is{
  	i = false;
	}
	Q.pushBack(source);
	is[source] = true;

	var sum = 0;
	while Q.getSize() != 0{
		var u = Q.popBack();
		sum -= table[u][1];
		is[u] = false;

		var v  = 0 : int;
		for j in g {
			if u == j[1] then v = j[2];
			else if u == j[2] then v = j[1];
			else
				continue;

			var w = j[3];
			if table[u][1] + w < table[v][1] {
				table[v][1] = table[u][1] + w;
				table[v][2] = u : string; // +"-";
				if !is[v] {
					Q.pushBack(v);
					sum += table[v][1];
					var avg = (sum * 1.01) / Q.getSize();
					//
					var (xx, v) = Q.popFront();
					Q.pushFront(v);
					while table[v][1] > avg {
						var (xy, vv) = Q.popFront();
						Q.pushBack(v);
						v = vv;
					}
					is[v] = true;
				}//
			}
		}
	} */
}

proc routing_table(table, graph, al)
{
	if al == 0{
		for i in 1..4{
			routing_table(table,graph,i);
		}
		return -1;
	}

	//writeln(al : algoritm, ":");
	use Time;
	const startTime = getCurrentTime();

	if al == 1{
		forall i in 1.._n {
			bellman_ford(table[i, ..], graph, i);
		}
	}
	else if al == 2{
		forall i in 1.._n {
			generic(table[i, ..], graph, i);
		}
	}
	else if al == 3{
		forall i in 1.._n {
			slf(table[i, ..], graph, i);
		}
	}
	else if al == 4{
		forall i in 1.._n {
			lll(table[i, ..], graph, i);
		}
	}

	const stopTime = getCurrentTime();
	const elapsedTime = stopTime - startTime;
	//writeln("Elapsed time was: ", elapsedTime,"\n");
	return elapsedTime;
  /*writeln(alg);
	use Time;
	select alg {
    when 1 {
			const startTime = getCurrentTime();
		  forall i in 1.._n {
		    bellman_ford(table[i, ..], graph, i);
		  }
			const stopTime = getCurrentTime();
			writeln("Elapsed time was: ", stopTime - startTime);
		}
		when 2 {
			const startTime = getCurrentTime();
		  forall i in 1.._n {
		    generic(table[i, ..], graph, i);
		  }
			const stopTime = getCurrentTime();
			writeln("Elapsed time was: ", stopTime - startTime);
		}
    otherwise return;
  }*/
}

proc main() {
  var reader = open(file, iomode.r).reader();
  reader.read(_n);
  var graph: [1..0] (3*int);
	var x : [1..3] int;
	while reader.read(x){
		graph.push_back((x[1],x[2],x[3]));
	}
  reader.close();

  var table: [1.._n, 1.._n] (int, string);
  var elapsedTime = routing_table(table, graph, alg);

	var writer = open("time.txt", iomode.cw).writer();
	writer.write(elapsedTime);

	write_to_file_raw(table);

  /* var cond = true;
  var inp : int;
  if c {
		while cond{
			writeln("0 - graph");
    	writeln("x - node");
    	inp = read(int);
			if inp == 0{
				for (a,b,c) in graph{
					writeln(a," ",b," ",c);
				}
			}
    	else if inp > 0 && inp <= _n {
				writeln(table[inp, ..]);
			}
			else {
				cond = false;
			}
  	}
	} */
}
