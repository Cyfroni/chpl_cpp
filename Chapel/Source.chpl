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
// g - graph (i, j, a)
// s - start node

  [x in dp] x = (1 << 30, -1);          // initialize with big distance
                                        // and no predecessor
	dp[s][1] = 0;                         // distance from source to source is 0
	var V = [s];                          // initialize queue with source node

	 while !V.isEmpty(){
		var _i = V.pop_front();             // take from the top

		for (i,j,a) in g{                   // iterate over all edges
      if i != _i || j==s then continue; // process only edges from i
                                        // and ommit edges to source
			if dp[i][1] + a < dp[j][1]{       // relaxation
				dp[j][1] = dp[i][1] + a;        // update distance
				dp[j][2] = i;                   // update predecessor
				if !V.find(j)[1] {
					V.push_back(j);               // add v to V if it's not there already.
				}
			}
		}
	}
}

proc slf(dp, g, s){
// dp - vector of distances and predecessors (d, p)
// g - graph (i, j, a)
// s - start node

  [x in dp] x = (1 << 30, -1);               // initialize with big distance
                                             // and no predecessor
	dp[s][1] = 0;                              // distance from source to source is 0
	var V = [s];                               // initialize queue with source node

	while !V.isEmpty() {
		var _i = V.pop_front();                  // take from the top

		for (i,j,a) in g {                       // iterate over all edges
        if i != _i || j==s then continue;    // process only edges from i
                                             // and ommit edges to source
			if dp[i][1] + a < dp[j][1] {           // relaxation
				dp[j][1] = dp[i][1] + a;             // update distance
				dp[j][2] = i;                        // update predecessor
        if !V.find(j)[1] {
          if (!V.isEmpty() &&
             dp[j][1] <= dp[V.front()][1]) { // SLF rule for insertion
              V.push_front(j);
          }
          else {
              V.push_back(j);
          }
        }
			}
		}
	}
}

proc lll(dp, g, s){
// dp - vector of distances and predecessors (d, p)
// g - graph (i, j, a)
// s - start node

  [x in dp] x = (1 << 30, -1);                   // initialize with big distance
                                                 // and no predecessor
  dp[s][1] = 0;                                  // distance from source to source is 0
  var V = [s];                                   // initialize queue with source node

	while !V.isEmpty(){
		var _i = V.pop_front();

    for (i,j,a) in g {                           // iterate over all edges
      if i != _i || j==s then continue;          // process only edges from i
                                                 // and ommit edges to source
      if dp[i][1] + a < dp[j][1] {               // relaxation
        dp[j][1] = dp[i][1] + a;                 // update distance
        dp[j][2] = i;                            // update predecessor
				if !V.find(j)[1] {
					V.push_back(j);                        // insertion like in generic alg.
          var sum = + reduce [x in V] dp[x][1];  // sum of all node labels in V
					var c = sum / V.size;                  // c - average node label in V
					var f = V.front();
					while dp[f][1] > c {                   // LLL rule for rearrangement
						V.pop_front();
						V.push_back(f);
						f = V.front();
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
  try{
	  while reader.read(x){
	  graph.push_back((x[1],x[2],x[3]));
	} catch {
    writeln(graph.size);
  }
  reader.close();

  var table: [1.._n, 1.._n] (int, int);

  var elapsedTime = routing_table(table, graph, alg);

  writeln(elapsedTime);

	/* var writer = open("time.txt", iomode.cw).writer();
	writer.write(elapsedTime); */

	//write_to_file_raw(table);

}
