public class ArrayTest
{
    private static int i, j, k;
    private static int v[];
    private static int vv[][];
    private static int vvv[][][];

    private static void doMatrix(int m[][])
    {
        for (i = 0; i < 5; ++i) {
            for (j = 0; j < 5; ++j) {
                m[i][j] = i*j;
            }
        }
    }

    private static void printCube(int c[][][])
    {
        System.out.println();
        for (j = 0; j < 3; ++j) {
            for (i = 0; i < 2; ++i) {
                for (k = 0; k < 4; ++k) {
                    System.out.print(String.format("%4d", c[i][j][k]));
                }
                System.out.print("     ");
            }
            System.out.println();
        }
    }

    public static void main(String[] args)
    {
        v = new int[10];
        vv = new int[5][5];
        vvv = new int[2][3][4];

        v[0] = 1;
        System.out.println(v[0]);

        vv[3][4] = 34;
        System.out.println(vv[3][4]);

        vvv[1][2][3] = 123;
        System.out.println(vvv[1][2][3]);

        for (i = 1; i < 10; ++i) {
            v[i] = 2*v[i-1];
        }

        System.out.println();
        for (i = 0; i < 10; ++i) {
            System.out.print(String.format("%4d", v[i]));
        }
        System.out.println();

        doMatrix(vv);

        System.out.println();
        for (i = 0; i < 5; ++i) {
            for (j = 0; j < 5; ++j) {
                System.out.print(String.format("%3d", vv[i][j]));
            }
            System.out.println();
        }

        for (i = 0; i < 2; ++i) {
            for (j = 0; j < 3; ++j) {
                for (k = 0; k < 4; ++k) {
                    vvv[i][j][k] = 100*i + 10*j + k;
                }
            }
        }

        printCube(vvv);
    }
}
