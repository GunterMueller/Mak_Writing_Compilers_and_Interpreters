public class ParmsTest3
{
    private static int i, j, k;
    private static int v[];
    private static int vv[][];
    private static int vvv[][][];

    private static void doCube1(int c[][][])
    {
        for (i = 0; i < 2; ++i) {
            for (j = 0; j < 3; ++j) {
                for (k = 0; k < 4; ++k) {
                    c[i][j][k] = 100*i + 10*j + k;
                }
            }
        }
    }

    private static void doCube2(int c[][][])
    {
        for (i = 0; i < 2; ++i) {
            for (j = 0; j < 3; ++j) {
                for (k = 0; k < 4; ++k) {
                    c[i][j][k] = 200*i + 10*j + k;
                }
            }
        }
    }

    private static void doCube3(int c1[][][])
    {
        for (i = 0; i < 2; ++i) {
            for (j = 0; j < 3; ++j) {
                for (k = 0; k < 4; ++k) {
                    c1[i][j][k] = 200*i + 10*j + k;
                }
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
        throws PascalRuntimeException
    {
        vvv = new int[2][3][4];

        doCube1(vvv);
        printCube((int[][][]) Cloner.deepClone(vvv));

        doCube2((int[][][]) Cloner.deepClone(vvv));
        printCube((int[][][]) Cloner.deepClone(vvv));

        doCube3(vvv);
        printCube((int[][][]) Cloner.deepClone(vvv));
    }
}
