import java.util.HashMap;
import java.util.Scanner;

public class RecordTest
{
    private static Scanner _standardIn = new Scanner(System.in);

    private static HashMap john;
    private static HashMap mary;

    private static void print(HashMap pers)
    {
        int age;
        HashMap addr = new HashMap();

        age = (Integer) pers.get("age");

        addr.put("street", (String) ((HashMap) pers.get("address")).get("street"));
        addr.put("city",   (String) ((HashMap) pers.get("address")).get("city"));
        addr.put("state",  (String) ((HashMap) pers.get("address")).get("state"));
        addr.put("zip",    (String) ((HashMap) pers.get("address")).get("zip"));

        System.out.println();
        System.out.println("First name: " + (String) pers.get("firstName"));
        System.out.println(" Last name: " + (String) pers.get("lastName"));
        System.out.println("       Age: " + age);
        System.out.println("    Street: " + (String) addr.get("street"));
        System.out.println("      City: " + (String) addr.get("city"));
        System.out.println("     State: " + (String) addr.get("state"));
        System.out.println("       ZIP: " + (String) addr.get("zip"));
    }

    public static void main(String[] args)
    {
        john = new HashMap();
        john.put("address", new HashMap());

        mary = new HashMap();
        mary.put("address", new HashMap());

        john.put("firstName", "John");
        john.put("lastName", "Doe");
        john.put("age", 24);
        ((HashMap) john.get("address")).put("street", "1680 25th Street");
        ((HashMap) john.get("address")).put("city", "San Pablo");
        ((HashMap) john.get("address")).put("state", "CA");
        ((HashMap) john.get("address")).put("zip", "94806");

        mary.put("firstName", "Mary");
        mary.put("lastName", "Jane");
        mary.put("age", 22);
        ((HashMap) mary.get("address")).put("street", "4899 Bela Drive");
        ((HashMap) mary.get("address")).put("city", "San Jose");
        ((HashMap) mary.get("address")).put("state", "CA");
        ((HashMap) mary.get("address")).put("zip", "95129");

        print(john);
        print(mary);
    }
}
