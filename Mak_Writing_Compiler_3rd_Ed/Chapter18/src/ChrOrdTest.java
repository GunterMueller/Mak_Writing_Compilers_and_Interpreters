public class ChrOrdTest
{
    private static int i;
    private static char c;

    public static void main(String[] args)
    {
        c = 'x';
        i = (int) c;
        c = (char) i;

        System.out.println("i = " + i + ", c = " + c);
    }
}
