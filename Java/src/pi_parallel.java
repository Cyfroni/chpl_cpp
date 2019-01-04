public class pi_parallel {

    public static void main(String[] args) {

        int num_steps = 10000000;
        double step = 1.0 / num_steps;

        long start = System.nanoTime();

        double pi_sum = 0.0;
        for (int i = 0; i < num_steps; i++){
            double x = (i + 0.5) * step;
            pi_sum += 4 / (1 + Math.pow(x, 2));
        }
        double pi = pi_sum * step;

        long end = System.nanoTime();

        System.out.println("time:\t" + (end - start)/1000000000.0);
        System.out.println("pi:\t\t" + (pi));
    }
}
