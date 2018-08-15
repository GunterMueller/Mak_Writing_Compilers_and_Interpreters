/**
 * <h1>Cloner</h1>
 *
 * <p>Pascal Runtime Library:
 * Create a deep clone of an object to pass it by value.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
import java.io.*;

public class Cloner
{
    public static Object deepClone(Object original)
        throws PascalRuntimeException
    {
        try {
            // Write the original object to a byte array stream.
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            ObjectOutputStream oos = new ObjectOutputStream(baos);
            oos.writeObject(original);

            // Construct a copy of the original object from the stream.
            ByteArrayInputStream bais =
                new ByteArrayInputStream(baos.toByteArray());
            ObjectInputStream ois = new ObjectInputStream(bais);

            // Return the copy as the deep clone.
            return ois.readObject();
        }
        catch (Exception ex) {
            throw new PascalRuntimeException("Deep clone failed.");
        }
    }
}
