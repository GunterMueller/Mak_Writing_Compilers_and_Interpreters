package wci.backend.compiler;

import java.util.ArrayList;

/**
 * <h1>LocalVariables</h1>
 *
 * <p>Maintain a method's local variables array.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class LocalVariables
{
    // List of booleans to keep track of reserved local variables. The ith
    // element is true if the ith variable is being used, else it is false.
    // The final size of the list is the total number of local variables
    // used by the method.
    private ArrayList<Boolean> reserved;

    /**
     * Constructor.
     * @param index initially reserve local variables 0 through index-1.
     */
    public LocalVariables(int index)
    {
        reserved = new ArrayList<Boolean>();

        for (int i = 0; i <= index; ++i) {
            reserved.add(true);
        }
    }

    /**
     * Reserve a local variable.
     * @return the index of the newly reserved variable.
     */
    public int reserve()
    {
        // Search for existing but unreserved local variables.
        for (int i = 0; i < reserved.size(); ++i) {
            if (! reserved.get(i)) {
                reserved.set(i, true);
                return i;
            }
        }

        // Reserved a new variable.
        reserved.add(true);
        return reserved.size()-1;
    }

    /**
     * Release a local variable that's no longer needed.
     * @param index the index of the variable.
     */
    public void release(int index)
    {
        reserved.set(index, false);
    }

    /**
     * Return the count of local variables needed by the method.
     * @return the count.
     */
    public int count()
    {
        return reserved.size();
    }
}
