package wci.backend.interpreter;

/**
 * <h1>Cell</h1>
 *
 * <p>Interface for the interpreter's runtime memory cell.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public interface Cell
{
    /**
     * Set a new value into the cell.
     * @param newValue the new value.
     */
    public void setValue(Object newValue);

    /**
     * @return the value in the cell.
     */
    public Object getValue();
}
