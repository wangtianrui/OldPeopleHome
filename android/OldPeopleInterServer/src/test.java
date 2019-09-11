import java.util.ArrayList;
import java.util.List;

public class test {
    public static void main(String[] args) {
        int size = 6;
        int i = 0, step = 0;
        List<Integer> list = new ArrayList<>();
        for (int j = 0; j < size; j++) {
            list.add(j);
        }
        while (step < size / 2) {
            System.out.println(list.get(i));
            System.out.println(list.get(i + 1));
            i += 2;
            System.out.println("------------");
            step += 1;
        }
        if (step * 2 < size) {
            System.out.println(list.get(i));
        }
    }
}
