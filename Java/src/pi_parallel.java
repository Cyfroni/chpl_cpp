import java.util.stream.IntStream;

public class pi_parallel {

    private static int num_steps = 1000000000;
    private static double step = 1.0 / num_steps;
    private static int threads = 4;

    private static double partial_sum(Integer part) {
        double pi_sum = 0.0;
        for (int i = part; i < num_steps; i += threads) {
            double x = (i + 0.5) * step;
            pi_sum += 4 / (1 + x * x);
        }
        return pi_sum;
    }

    public static void main(String[] args) {

        double pi_sum = IntStream.range(0, threads).parallel()
                .mapToDouble(pi_parallel::partial_sum)
                .reduce(0.0, (a, b) -> a + b);

        double pi = pi_sum * step;

        System.out.println(pi);
    }
}
