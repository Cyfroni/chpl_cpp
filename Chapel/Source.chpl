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

proc bellman_ford(dp, g, s){
// dp - vector of distances and predecessors (d, p)
// g - graph (i, j, a)
// s - start node

  [x in dp] x = (1 << 30, -1);        // initialize with big distance
                                      // and no predecessor

	dp[s][1] = 0;									      // distance from source to source is 0

	for x in g.domain {
		for (i,j,a) in g {								// for each edge in graph
			if (dp[i][1] + a < dp[j][1]){		// relaxation
				dp[j][1] = dp[i][1] + a;			// update distance
				dp[j][2] = i;									// update predecessor
			}
		}
	}
}

proc generic(dp, g, s){
// dp - vector of distances and predecessors (d, p)
// g - graph (u, v, w)
// source - start node

  [x in dp] x = (1 << 30, -1);       // initialize with big distance
                                     // and no predecessor
	dp[s][1] = 0;                      // distance from source to source is 0
	var V = [s];                       // Initialize queue with source node

	 while !V.isEmpty(){

		var _i = V.pop_front();          // take from the top

		for (i,j,a) in g{                // iterate over all edges
      var contain = [i,j].find(_i);  // find only edges with _i
      if !contain[1] then continue;  // if edge doesn't contain _i -> continue
      else if _i != i then i <=> j;  // also, make sure that i == _i

			if dp[i][1] + a < dp[j][1]{    // relaxation
				dp[j][1] = dp[i][1] + a;     // update distance
				dp[j][2] = i;                // update predecessor
				if !V.find(j)[1] {
					V.push_back(j);            // add v to V if it's not there already.
				}
			}
		}
	}
}

proc slf(dp, g, s){
// dp - vector of distances and predecessors (d, p)
// g - graph (u, v, w)
// source - start node

  [x in dp] x = (1 << 30, -1);           // initialize with big distance
                                         // and no predecessor

	dp[s][1] = 0;                          // distance from source to source is 0
	var V = [s];                           // Initialize queue with source node


	while !V.isEmpty() {
		var _i = V.pop_back();

		for (i,j,a) in g {
      var contain = [i,j].find(_i);      // find only edges with _i
      if !contain[1] then continue;      // if edge doesn't contain _i -> continue
      else if _i != i then i <=> j;      // also, make sure that i == _i

			if table[i][1] + a < table[j][1] { // relaxation
				table[j][1] = table[i][1] + a;   // update distance
				table[j][2] = i;                 // update predecessor
        if !V.find(j)[1] {
          if !V.isEmpty() && dp[j][1] < V.back() {
              Q.push_front(j);
          }
          else {
              Q.push_back(j);
          }
        }
			}
		}
	}
}

proc lll(table, g, source){
// dp - vector of distances and predecessors (d, p)
// g - graph (u, v, w)
// source - start node

  [x in dp] x = (1 << 30, -1);            // initialize with big distance
                                          // and no predecessor
  dp[s][1] = 0;                           // distance from source to source is 0
  var V = [s];                            // Initialize queue with source node

	while !V.isEmpty(){
		var _i = V.pop_front();

    for (i,j,a) in g {
      var contain = [i,j].find(_i);       // find only edges with _i
      if !contain[1] then continue;       // if edge doesn't contain _i -> continue
      else if _i != i then i <=> j;       // also, make sure that i == _i

			if table[i][1] + w < table[j][1] {
				table[j][1] = table[i][1] + a;
				table[j][2] = i;
				if !V.find(j)[1] {
					V.push_back(j);
          var sum = + reduce [x in V] x[1];
					var c = (sum * 1.01) /  V.size;
					v = Q.front();
					while table[j][1] > c {
						Q.pop_front();
						Q.push_back(j);
						j = Q.front();
					}
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

	write_to_file_raw(table);

}
