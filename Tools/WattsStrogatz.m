function G = WattsStrogatz(N, K, beta)
% H = WattsStrogatz(N, K, beta) returns a Watts - Strogatz model graph with N
% nodes, N * K edges, mean node degree 2 * K, and rewiring probability beta.
%
% beta = 0 is a ring lattice, and beta = 1 is a random graph.

% Connect each node to its K next and previous neighbors. This constructs
% indices for a ring lattice.
    s = repelem((1 : N)', 1, K);
t = s + repmat(1 : K, N, 1);
t = mod(t - 1, N)+ 1;

% Rewire the target node of each edge with probability beta
for source = 1 : N
switchEdge = rand(K, 1) < beta;

newTargets = rand(N, 1);
newTargets(source) = 0;
newTargets(s(t == source)) = 0;
newTargets(t(source, ~ switchEdge)) = 0;

[ ~ , ind] = sort(newTargets, 'descend');
t(source, switchEdge) = ind(1 : nnz(switchEdge));
end

G = graph(s, t, randi([1 10], 1, N * K));

A =[G.Edges.EndNodes( : , 1)'; G.Edges.EndNodes( : , 2)'; G.Edges.Weight'];
fileID = fopen('Graph.txt', 'w');
fprintf(fileID, '%d\r\n', N);
fprintf(fileID, '%3d %3d %12d\r\n', A);
fclose(fileID);

end

% plot(G, 'EdgeLabel', G.Edges.Weight)