proc relu(array){
  var output = [i in array] if i < 0.0 then 0.0 else i;
  return output;
}

proc reluPrime(array){
  var output = [i in array] if i <= 0.0 then 0.0 else 1.0;
  return output;
}

proc dot(a, b){
  var output : [a.domain.dim(1), b.domain.dim(2)] real;
  forall (i, j) in output.domain {
    output[i, j] = + reduce [k in a.domain.dim(2)] a[i, k] * b[k, j];
  }
  return output;
}

proc softmax (z){
  var output = [i in z] exp(i);
  [i in output.domain.dim(1)] output[i, ..] /= + reduce output[i, ..];
  return output;
}

proc transpose(z){
  var output : [z.domain.dim(2), z.domain.dim(1)] real;
  [(i, j) in z.domain] output[j,i] = z[i,j];
  return output;
}

proc main() {
  var reader = open("../25.data", iomode.r).reader();
  var reader2 = open("../1.dnn", iomode.r).reader();
  var line, infoLine, dnnLine : string;
  var dnn : [1..0] int;

  reader.read(infoLine);
  var infoData = infoLine.split(',');
  var (dataAmount, dataLenght, categories) = (infoData[1], infoData[2], infoData[3]);

  reader2.read(dnnLine);
  reader2.close();
  var dnnData = dnnLine.split(',');

  dnn.push_back(dataAmount : int); // niepelne dane
  dnn.push_back(dataLenght : int - 1);
  for item in dnnData{
    dnn.push_back(item : int);
  }
  dnn.push_back(categories : int);

  var y_train : [1..dnn[1], 1..dnn[5]] real;
  var x_train : [1..dnn[1], 1..dnn[2]] real;
  var k = 1;
  writeln("reading..");
	while reader.read(line){
    var data = line.split(',');
    try{
      var l = 1;
      for item in data[2..]{
        x_train[k, l] = item : real;
        l += 1;
      }
      if data[1] == "B" {
        y_train[k, ..] = [1.0, 0.0];
      } else {
        y_train[k, ..] = [0.0, 1.0];
      }
      /* y_train[k] = data[10] : real; */
      k += 1;
    } catch e {
      continue;
    }
	}
  reader.close();

  for i in x_train.domain.dim(2){
    var sum = + reduce x_train[.., i];
    var std = + reduce [a in x_train[.., i]] a*a;
    var mean = sum / dnn[1];
    std = sqrt(std / dnn[1] - mean * mean);
    x_train[.., i] = [a in x_train[.., i]] (a - mean) / std;
  }

  var BATCH_SIZE = 10;
  var lr = 0.01/BATCH_SIZE;

  var W1 : [1..dnn[2], 1..dnn[3]] real;
  var W2 : [1..dnn[3], 1..dnn[4]] real;
  var W3 : [1..dnn[4], 1..dnn[5]] real;

  use Random;
  writeln('randomizing..');
  fillRandom(W1);
  W1 = [i in W1] i / 20.0;
  fillRandom(W2);
  W2 = [i in W2] i / 20.0;
  fillRandom(W3);
  W3 = [i in W3] i / 20.0;

  var rand_indx : [1..100000] real;

  use Time;
  var timer : Timer;
  timer.start();

  fillRandom(rand_indx);
  rand_indx = [indx in rand_indx] indx * (dnn[1]-BATCH_SIZE);
  writeln('learning..');
  for i in 1..100000 {
    var indx = rand_indx[i] : int;

    var b_x = x_train[(1..BATCH_SIZE) + indx, ..];//.reindex(0..BATCH_SIZE - 1, 0..dnn[2]);
    var b_y = y_train[(1..BATCH_SIZE) + indx, ..];//.reindex(0..BATCH_SIZE - 1, 0..dnn[5]);

    var seed = SeedGenerator.oddCurrentTime;
    for i in b_x.domain.dim(2){
      shuffle(b_x[.., i], seed);
    }
    for i in b_y.domain.dim(2){
      shuffle(b_y[.., i], seed);
    }
    var a1 = relu(dot(b_x, W1));
    var a2 = relu(dot(a1, W2));
    var yhat = softmax(dot(a2, W3));
    var dyhat = yhat - b_y;

    var dW3 = dot(transpose(a2), dyhat);
    var dz2 = dot(dyhat, transpose(W3)) * reluPrime(a2);
    var dW2 = dot(transpose(a1), dz2);
    var dz1 = dot(dz2, transpose(W2)) * reluPrime(a1);
    var dW1 = dot(transpose(b_x), dz1);

    W3 = W3 - lr * dW3;
    W2 = W2 - lr * dW2;
    W1 = W1 - lr * dW1;

    if (i % 100 == 0){
        writeln("-----------------------------------------------Epoch ", i, "--------------------------------------------------");
        writeln("Predictions:");
        writeln(yhat[(1..10) + indx, ..]);
        writeln("Ground truth:");
        writeln(b_y[(1..10) + indx, ..]);
        var loss_m = yhat - b_y;
        var loss = + reduce [j in loss_m] j**2;
        writeln("                                            Loss ", loss/BATCH_SIZE);
        writeln("                                            Time ", timer.elapsed());
        writeln("--------------------------------------------End of Epoch :(------------------------------------------------");
        /* writeln(W1); */
    }
  }
  writeln("time: ", timer.elapsed());


















}
