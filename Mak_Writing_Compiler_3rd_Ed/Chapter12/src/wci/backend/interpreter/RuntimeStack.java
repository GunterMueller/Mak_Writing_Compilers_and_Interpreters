package wci.backend.interpreter;

import java.util.ArrayList;

/**
 * <h1>RuntimeStack</h1>
 *
 * <p>Interface for the interpreter's runtime stack.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public interface RuntimeStack
{
    /**
     * @return an array list of the activation records on the stack.
     */
    public ArrayList<ActivationRecord> records();

    /**
     * Get the topmost activation record at a given nesting level.
     * @param nestingLevel the nesting level.
     * @return the activation record.
     */
    public ActivationRecord getTopmost(int nestingLevel);

    /**
     * @return the current nesting level.
     */
    public int currentNestingLevel();

    /**
     * Pop an activation record off the stack.
     */

    public void pop();
    /**
     * Push an activation record onto the stack.
     * @param ar the activation record to push.
     */
    public void push(ActivationRecord ar);
}
