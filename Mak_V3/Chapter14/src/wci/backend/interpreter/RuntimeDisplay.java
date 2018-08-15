package wci.backend.interpreter;

/**
 * <h1>RuntimeDisplay</h1>
 *
 * <p>Interface for the interpreter's runtime display.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public interface RuntimeDisplay
{
    /**
     * Get the activation record at a given nesting level.
     * @param nestingLevel the nesting level.
     * @return the activation record.
     */
    public ActivationRecord getActivationRecord(int nestingLevel);

    /**
     * Update the display for a call to a routine at a given nesting level.
     * @param nestingLevel the nesting level.
     * @param ar the activation record for the routine.
     */
    public void callUpdate(int nestingLevel, ActivationRecord ar);

    /**
     * Update the display for a return from a routine at a given nesting level.
     * @param nestingLevel the nesting level.
     */
    public void returnUpdate(int nestingLevel);
}
