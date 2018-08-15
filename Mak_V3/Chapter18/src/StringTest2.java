public class StringTest2
{
    private static StringBuilder name1;
    private static StringBuilder name2;
    private static StringBuilder cube1[][][];
    private static StringBuilder cube2[][][];

    private static void xfer1(StringBuilder str1, StringBuilder str2)
    {
        str2.setLength(0);
        str2.append(str1);
    }

    private static void noxfer1(StringBuilder str1, StringBuilder str2)
    {
        str2.setLength(0);
        str2.append(str1);
    }

    private static void xfer2(StringBuilder cb1[][][], StringBuilder cb2[][][])
    {
        cb2[1][0][1] = cb1[1][0][1];
    }

    private static void noxfer2(StringBuilder cb1[][][], StringBuilder cb2[][][])
    {
        cb2[1][0][1] = cb1[1][0][1];
    }

    public static void main(String[] args)
        throws Exception
    {
        name1 = new StringBuilder();
        name2 = new StringBuilder();
        cube1 = new StringBuilder[2][2][2];
        cube1[0][0][0] = PaddedString.create(5);
        cube1[0][0][1] = PaddedString.create(5);
        cube1[0][1][0] = PaddedString.create(5);
        cube1[0][1][1] = PaddedString.create(5);
        cube1[1][0][0] = PaddedString.create(5);
        cube1[1][0][1] = PaddedString.create(5);
        cube1[1][1][0] = PaddedString.create(5);
        cube1[1][1][1] = PaddedString.create(5);
        cube2 = new StringBuilder[2][2][2];
        cube2[0][0][0] = PaddedString.create(5);
        cube2[0][0][1] = PaddedString.create(5);
        cube2[0][1][0] = PaddedString.create(5);
        cube2[0][1][1] = PaddedString.create(5);
        cube2[1][0][0] = PaddedString.create(5);
        cube2[1][0][1] = PaddedString.create(5);
        cube2[1][1][0] = PaddedString.create(5);
        cube2[1][1][1] = PaddedString.create(5);

        name1.setLength(0); name1.append("AAAAA");
        name2.setLength(0); name2.append("BBBBB");
        System.out.println("'" + name1 + ", " + name2 + "'");

        noxfer1((StringBuilder) Cloner.deepClone(name1),
                (StringBuilder) Cloner.deepClone(name2));
        System.out.println("'" + name1 + ", " + name2 + "'");

        xfer1((StringBuilder) Cloner.deepClone(name1), name2);
        System.out.println("'" + name1 + ", " + name2 + "'");

        cube1[1][0][1].setLength(0); cube1[1][0][1].append("1,0,1");
        cube2[1][0][1].setLength(0); cube2[1][0][1].append("xxxxx");
        System.out.println("'" + cube1[1][0][1] + ", " + cube2[1][0][1] + "'");

        noxfer2((StringBuilder[][][]) Cloner.deepClone(cube1),
                (StringBuilder[][][]) Cloner.deepClone(cube2));
        System.out.println("'" + cube1[1][0][1] + ", " + cube2[1][0][1] + "'");

        xfer2((StringBuilder[][][]) Cloner.deepClone(cube1), cube2);
        System.out.println("'" + cube1[1][0][1] + ", " + cube2[1][0][1] + "'");
    }
}
