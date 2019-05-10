config const file : string;
config const alg : int;

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

proc bellman_ford(dp, g, source){
// dp - vector of distances and predecessors (d, p)
// g - graph (u, v, w)
// source - start node
	for i in dp.domain{
		dp[i] = (1 << 30, -1);						// initialize with big distance
	}																		// and no predecessor

	dp[source][1] = 0;									// distance from source to source is 0

	for i in g.domain {
		for e in g {											// for each edge in graph
			var u = e[1] : int;
			var v = e[2] : int;
			var w = e[3] : int;
			if (dp[u][1] + w < dp[v][1]){		// relaxation
				dp[v][1] = dp[u][1] + w;			// update distance
				dp[v][2] = u;									// update predecessor
			}
		}
	}
}

proc generic(dp, g, source){
// dp - vector of distances and predecessors (d, p)
// g - graph (u, v, w)
// source - start node

	for i in dp.domain{
		dp[i] = (1 << 30, -1);
	}
	dp[source][1] = 0;

	var Q = [source];
	var is: [1.._n] bool;
	is[source] = true;

	 while !Q.isEmpty(){

		var u = Q.pop_back();
		is[u] = false;
		var v: int;
		for e in g{
			if u == e[1] then v = e[2];
			else if u == e[2] then v = e[1];
			else
				continue;

			var w = j[3];
			if dp[u][1] + w < dp[v][1]{
				dp[v][1] = dp[u][1] + w;
				dp[v][2] = u;
				if !is[v]{
					Q.push_back(v);
					is[v] = true;
				}
			}
		}
	}
}

proc generic2(table, g, source){
	use List;

	for i in table.domain{
		table[i] = (1 << 30, -1);
	}
	table[source][1] = 0;

	var Q = makeList(source);
	var is: [1.._n] bool;
	is[source] = true;

	 while Q.size != 0{

		var u = Q.pop_front();
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
					Q.push_front(v);
					is[v] = true;
				}
			}
		}
	}
}

proc generic3(table, g, source){
	use DistributedDeque;

	for i in table.domain{
		table[i] = (1 << 30, -1);
	}
	table[source][1] = 0;

	var Q = new DistDeque(int);
	var is: [1.._n] bool;
	Q.pushBack(source);
	is[source] = true;

	while Q.getSize() != 0{
		var (_, u) = Q.popBack();
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
// dp - vector of distances and predecessors (d, p)
// g - graph (u, v, w)
// source - start node

	for i in table.domain{
		table[i] = (1 << 30, -1);
	}
	table[source][1] = 0;

	var Q = [source];
	var is: [1.._n] bool;
	is[source] = true;

	while !Q.isEmpty() {
		var u = Q.pop_back();
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
						if !Q.isEmpty() && table[v][1] < Q.back() {
								Q.push_front(v);
						}
						else {
								Q.push_back(v);
						}
						is[v] = true;
				}
			}
		}
	}
}

proc lll(table, g, source){
// dp - vector of distances and predecessors (d, p)
// g - graph (u, v, w)
// source - start node

	for i in table.domain{
		table[i] = (1 << 30, -1);
	}
	table[source][1] = 0;

	var Q = [source];
	var is: [1.._n] bool;
	is[source] = true;
	var sum = 0;

	while !Q.isEmpty(){
		var u = Q.pop_back();
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
					Q.push_back(v);
					sum += table[v][1];
					var avg = (sum * 1.01) / Q.size;
					v = Q.front();
					while table[v][1] > avg {
						Q.pop_front();
						Q.push_back(v);
						v = Q.front();
					}
					is[v] = true;
				}
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

  writeln(elapsedTime);

	/* var writer = open("time.txt", iomode.cw).writer();
	writer.write(elapsedTime); */

	//write_to_file_raw(table);

}
