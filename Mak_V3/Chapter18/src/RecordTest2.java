import java.util.HashMap;
import java.util.Scanner;

public class RecordTest2
{
    private static Scanner _standardIn = new Scanner(System.in);

    private static HashMap john;
    private static HashMap mary;

    private static void print(HashMap pers)
    {
        int age;

        // Allocate addr.
        HashMap addr = new HashMap();
        addr.put("street", PaddedString.create(16));
        addr.put("city", PaddedString.create(16));
        addr.put("state", PaddedString.create(2));
        addr.put("zip", PaddedString.create(5));

        // Allocate phs.
        StringBuilder phs[] = new StringBuilder[2];
        phs[0] = PaddedString.create(8);
        phs[1] = PaddedString.create(8);

        age = (Integer) pers.get("age");

        // Copy pers.address to addr.
        ((StringBuilder) addr.get("street")).setLength(0);
        ((StringBuilder) addr.get("street"))
            .append((StringBuilder) ((HashMap) pers.get("address")).get("street"));
        ((StringBuilder) addr.get("city")).setLength(0);
        ((StringBuilder) addr.get("city"))
            .append((StringBuilder) ((HashMap) pers.get("address")).get("city"));
        ((StringBuilder) addr.get("state")).setLength(0);
        ((StringBuilder) addr.get("state"))
            .append((StringBuilder) ((HashMap) pers.get("address")).get("state"));
        ((StringBuilder) addr.get("zip")).setLength(0);
        ((StringBuilder) addr.get("zip"))
            .append((StringBuilder) ((HashMap) pers.get("address")).get("zip"));

        // Copy pers.phones to phs.
        phs[0].setLength(0); phs[0].append(((StringBuilder[]) pers.get("phones"))[0]);
        phs[1].setLength(0); phs[1].append(((StringBuilder[]) pers.get("phones"))[1]);

        System.out.println();
        System.out.println("First name: " + (StringBuilder) pers.get("firstName"));
        System.out.println(" Last name: " + (StringBuilder) pers.get("lastName"));
        System.out.println("       Age: " + age);
        System.out.println("    Street: " + (StringBuilder) addr.get("street"));
        System.out.println("      City: " + (StringBuilder) addr.get("city"));
        System.out.println("     State: " + (StringBuilder) addr.get("state"));
        System.out.println("       ZIP: " + (StringBuilder) addr.get("zip"));
        System.out.println("  Phone #1: " + phs[0]);
        System.out.println("  Phone #2: " + phs[1]);
    }

    public static void main(String[] args)
    {
        // Allocate john.
        john = new HashMap();
        john.put("firstName", PaddedString.create(16));
        john.put("lastName", PaddedString.create(16));
        john.put("age", 0);

        // Allocate john.address.
        john.put("address", new HashMap());
        ((HashMap) john.get("address")).put("street", PaddedString.create(16));
        ((HashMap) john.get("address")).put("city", PaddedString.create(16));
        ((HashMap) john.get("address")).put("state", PaddedString.create(2));
        ((HashMap) john.get("address")).put("zip", PaddedString.create(5));

        // Allocate john.phones.
        john.put("phones", new StringBuilder[2]);
        ((StringBuilder[]) john.get("phones"))[0] = PaddedString.create(8);
        ((StringBuilder[]) john.get("phones"))[1] = PaddedString.create(8);

        // Allocate mary.
        mary = new HashMap();
        mary.put("firstName", PaddedString.create(16));
        mary.put("lastName", PaddedString.create(16));
        mary.put("age", 0);

        // Allocate mary.address
        mary.put("address", new HashMap());
        ((HashMap) mary.get("address")).put("street", PaddedString.create(16));
        ((HashMap) mary.get("address")).put("city", PaddedString.create(16));
        ((HashMap) mary.get("address")).put("state", PaddedString.create(2));
        ((HashMap) mary.get("address")).put("zip", PaddedString.create(5));

        // Allocate mary.phones.
        mary.put("phones", new StringBuilder[2]);
        ((StringBuilder[]) mary.get("phones"))[0] = PaddedString.create(8);
        ((StringBuilder[]) mary.get("phones"))[1] = PaddedString.create(8);

        // Initialize john's name.
        ((StringBuilder) john.get("firstName")).setLength(0);
        ((StringBuilder) john.get("firstName")).append("John");
        ((StringBuilder) john.get("lastName")).setLength(0);
        ((StringBuilder) john.get("lastName")).append("Doe");

        john.put("age", 24);

        // Initialize john.address.
        ((StringBuilder) ((HashMap) john.get("address")).get("street")).setLength(0);
        ((StringBuilder) ((HashMap) john.get("address")).get("street")).
            append("1680 25th Steet");
        ((StringBuilder) ((HashMap) john.get("address")).get("city")).setLength(0);
        ((StringBuilder) ((HashMap) john.get("address")).get("city")).
            append("San Pablo");
        ((StringBuilder) ((HashMap) john.get("address")).get("state")).setLength(0);
        ((StringBuilder) ((HashMap) john.get("address")).get("state")).
            append("CALIFORNIA".substring(0,2));
        ((StringBuilder) ((HashMap) john.get("address")).get("zip")).setLength(0);
        ((StringBuilder) ((HashMap) john.get("address")).get("zip")).
            append("94806");

        // Initialize john.phones.
        ((StringBuilder[]) john.get("phones"))[0].setLength(0);
        ((StringBuilder[]) john.get("phones"))[0].append("111-1111");
        ((StringBuilder[]) john.get("phones"))[1].setLength(0);
        ((StringBuilder[]) john.get("phones"))[1].append("222-2222");

        // Initialize mary's name.
        ((StringBuilder) mary.get("firstName")).setLength(0);
        ((StringBuilder) mary.get("firstName")).append("Mary");
        ((StringBuilder) mary.get("lastName")).setLength(0);
        ((StringBuilder) mary.get("lastName")).append("Jane");

        mary.put("age", 22);

        // Initialize mary.address.
        ((StringBuilder) ((HashMap) mary.get("address")).get("street")).setLength(0);
        ((StringBuilder) ((HashMap) mary.get("address")).get("street")).
            append("4899 Bela Drive");
        ((StringBuilder) ((HashMap) mary.get("address")).get("city")).setLength(0);
        ((StringBuilder) ((HashMap) mary.get("address")).get("city")).
            append("San Jose");
        ((StringBuilder) ((HashMap) mary.get("address")).get("state")).setLength(0);
        ((StringBuilder) ((HashMap) mary.get("address")).get("state")).
            append("CA");
        ((StringBuilder) ((HashMap) mary.get("address")).get("zip")).setLength(0);
        ((StringBuilder) ((HashMap) mary.get("address")).get("zip")).
            append("95129");

        // Initialize mary.phones.
        ((StringBuilder[]) mary.get("phones"))[0].setLength(0);
        ((StringBuilder[]) mary.get("phones"))[0].append("333-3333");
        ((StringBuilder[]) mary.get("phones"))[1].setLength(0);
        ((StringBuilder[]) mary.get("phones"))[1].append("444-4444");

        print(john);
        print(mary);
    }
}
