

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
  var reader = open("../train.txt", iomode.r).reader();
  writeln("start..");
  var y_train : [0..41999, 0..9] real;
  var x_train : [0..41999, 0..783] real;
  var line : [0..784] int;
  var k = 0;
  writeln("reading..");
	while reader.read(line){
    var num = line[0];
    var y : [0..9] real;
    y[num] = 1.0;
    y_train[k, ..] = y;

    var x = line[1..] / 255.0;
    x_train[k, ..] = x;

    k += 1;
    /* if (k == 42000){
      break;
    } */
    //writeln(k);
	}

  reader.close();

  var BATCH_SIZE = 256;
  var lr = 0.01/BATCH_SIZE;

  var W1 : [0..783, 0..127] real;
  var W2 : [0..127, 0..63] real;
  var W3 : [0..63, 0..9] real;

  use Random;
  writeln('randomizing..');
  fillRandom(W1);
  W1 = [i in W1] i / 20.0;
  fillRandom(W2);
  W2 = [i in W2] i / 20.0;
  fillRandom(W3);
  W3 = [i in W3] i / 20.0;

  var rand_indx : [0..10000] real;

  use Time;
  var timer : Timer;
  timer.start();

  fillRandom(rand_indx);
  rand_indx = [indx in rand_indx] indx * (41999-BATCH_SIZE);
  writeln('learning..');
  for i in 0..3000 {
    //writeln(i);
    var indx = rand_indx[i] : int;
    var b_x = x_train[(0..BATCH_SIZE - 1) + indx, ..].reindex(0..BATCH_SIZE - 1, 0..783);
    var b_y = y_train[(0..BATCH_SIZE - 1) + indx, ..].reindex(0..BATCH_SIZE - 1, 0..9);


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
        writeln(yhat[0..9, ..]);
        writeln("Ground truth:");
        writeln(b_y[0..9, ..]);
        var loss_m = yhat - b_y;
        var loss = + reduce [j in loss_m] j**2;
        writeln("                                            Loss ", loss/BATCH_SIZE);
        writeln("                                            Time ", timer.elapsed());
        writeln("--------------------------------------------End of Epoch :(------------------------------------------------");
    }
  }
}
