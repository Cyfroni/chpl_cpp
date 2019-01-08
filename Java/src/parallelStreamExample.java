import java.util.Arrays;

public class parallelStreamExample {

    public static void main(String[] args) {
        int[] data = {1, 2, 3, 4, 5, 6, 7, 8, 9};

        Arrays.stream(data).parallel()
                .forEach(System.out::println);

    }

}
