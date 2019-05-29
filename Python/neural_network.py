# Feed forward
X1 = relu(dot(X0, W1))              # score function
X2 = relu(dot(X1, W2))
Y_out = softmax(dot(X2, W3))        # loss function

# Back propagation
dy = Y_out - Y
dW3 = dot(T(X2), dy)                # gradient computation
dz2 = dot(dy, T(W3)) * relu'(X2)    # chain rule
dW2 = dot(T(X1), dz2)
dz1 = dot(dz2, T(W2)) * relu'(X1)
dW1 = dot(T(X0), dz1)

# Updating the parameters
W3 = W3 - lr * dW3;                 # Vanilla gradient descent
W2 = W2 - lr * dW2;
W1 = W1 - lr * dW1;
