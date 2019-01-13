// chpl Source.chpl -o chpl.out
// ./chpl.out --n=500
// ./chpl.out --c=true
// ./chpl.out --a=2
config const file = " ";
config const alg = 0;
config const c = false;
config const a = 1;

var _n : int;
enum algoritm{
	Belman_ford = 1,
	Generic,
	SLF,
	LLL
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
		table[i] = (1 << 30, -1);
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
				table[v][2] = u;
			}
			if (table[v][1] + w < table[u][1])
			{
				table[u][1] = table[v][1] + w;
				table[u][2] = v;
			}
		}
	}
}

proc generic(table, g, source){
	use DistributedDeque;

	for i in table.domain{
		table[i] = (1 << 30, -1);
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
			if u == j[1] then v = j[2];
			else if u == j[2] then v = j[1];
			else
				continue;

			var w = j[3];
			if table[u][1] + w < table[v][1]{
				table[v][1] = table[u][1] + w;
				table[v][2] = u;
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
		table[i] = (1 << 30, -1);
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
				table[v][2] = u;
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
	use DistributedDeque;

	for i in table.domain{
		table[i] = (1 << 30, -1);
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
		var (x, u) = Q.popBack();
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
				table[v][2] = u;
				if !is[v] {
					Q.pushBack(v);
					sum += table[v][1];
					var avg = (sum * 1.01) / Q.getSize();
					//
					var (xx, uu) = Q.popFront();
					Q.pushFront(uu);
					v = uu;
					while table[v][1] > avg {
						var (xy, vv) = Q.popFront();
						Q.pushBack(v);
						(xy, vv) = Q.popFront();
						Q.pushFront(vv);
						v = vv;
					}
					is[v] = true;
				}//
			}
		}
	}
}

proc routing_table(table, graph, al)
{

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
	return elapsedTime;

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

  var table: [1.._n, 1.._n] (int, int);
  var elapsedTime = routing_table(table, graph, alg);

	var writer = open("time.txt", iomode.cw).writer();
	writer.write(elapsedTime);

	write_to_file_raw(table);

}
