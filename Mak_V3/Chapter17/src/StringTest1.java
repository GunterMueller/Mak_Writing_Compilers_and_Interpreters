public class StringTest1
{
    private static StringBuilder str5, strV;
    private static StringBuilder str8;
    private static StringBuilder list[];
    private static StringBuilder matrix[][];
    private static StringBuilder cube[][][];

    public static void main(String[] args)
    {
        str5 = PaddedString.create(5);
        strV = PaddedString.create(5);
        str8 = PaddedString.create(8);

        list = new StringBuilder[3];
        for (int i = 0; i < 3; ++i) {
            list[i] = PaddedString.create(5);
        }

        matrix = new StringBuilder[2][3];
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 3; ++j) {
                matrix[i][j] = PaddedString.create(5);
            }
        }

        cube = new StringBuilder[2][2][2];
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                for (int k = 0; k < 2; ++k) {
                    cube[i][j][k] = PaddedString.create(5);
                }
            }
        }

        // str5 := 'hello'
        // strV := str5
        // str8 := 'everyone'
        str5.setLength(0); str5.append("hello");
        strV.setLength(0); strV.append(str5);
        str8.setLength(0); str8.append("everyone");

        // writeln('''', str5, ', ', strV, ', ', str8, '''')
        System.out.println("'" + str5 + ", " + strV + ", " + str8 + "'");

        if (str5.toString().compareTo(str8.toString()) > 0) {
            System.out.println("greater");
        }

        if (str5.toString().compareTo("goodbye") > 0) {
            System.out.println("greater");
        }

        if ("nobody".compareTo(str8.toString()) >= 0) {
            System.out.println("greater");
        }

        // str5 := str8
        // writeln('''', str5, '''');
        str5.setLength(0); str5.append(str8.substring(0, 5));
        System.out.println("'" + str5 + "'");

        // str5 := 'hello';
        // str8 := str5
        // writeln('''', str8, '''')
        str5.setLength(0); str5.append("hello");
        str8.setLength(0); str8.append(str5).append(PaddedString.blanks(8, 5));
        System.out.println("'" + str8 + "'");

        str8.setCharAt(7, '!');
        System.out.println("'" + str8 + "'");

        list[0].setLength(0); list[0].append("Ron").append(PaddedString.blanks(5, 3));
        list[1].setLength(0); list[1].append("John").append(PaddedString.blanks(5, 4));
        System.out.println("'" + list[0] + "'");
        System.out.println("'" + list[1] + "'");

        matrix[0][1].setLength(0); matrix[0][1].append("  0,1");
        matrix[1][2].setLength(0); matrix[1][2].append("  1,2");
        System.out.println("'" + matrix[0][1] + ", " + matrix[1][2] + "'");

        cube[0][1][1].setLength(0); cube[0][1][1].append("0,1,1");
        cube[1][0][1].setLength(0); cube[1][0][1].append("1,0,1");
        System.out.println("'" + cube[0][1][1] + ", " + cube[1][0][1] + "'");

        cube[1][0][1].setCharAt(2, cube[0][1][1].charAt(4));
        System.out.println("'" + cube[1][0][1] + "'");

        if (cube[1][0][1].toString().compareTo(cube[0][1][1].toString()) > 0) {
            System.out.println("greater");
        }

        if (cube[1][0][1].charAt(2) > '0') {
            System.out.println("greater");
        }
    }
}
