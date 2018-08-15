package wci.backend.interpreter.memoryimpl;

import java.util.ArrayList;

import wci.backend.interpreter.*;

/**
 * <h1>RuntimeStackImpl</h1>
 *
 * <p>The interpreter's runtime stack.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class RuntimeStackImpl
    extends ArrayList<ActivationRecord>
    implements RuntimeStack
{
    private RuntimeDisplay display;  // runtime display

    /**
     * Constructor.
     */
    public RuntimeStackImpl()
    {
        display = MemoryFactory.createRuntimeDisplay();
    }

    /**
     * @return an array list of the activation records on the stack.
     */
    public ArrayList<ActivationRecord> records()
    {
        return this;
    }

    /**
     * Get the topmost activation record at a given nesting level.
     * @param nestingLevel the nesting level.
     * @return the activation record.
     */
    public ActivationRecord getTopmost(int nestingLevel)
    {
        return display.getActivationRecord(nestingLevel);
    }

    /**
     * @return the current nesting level.
     */
    public int currentNestingLevel()
    {
        int topIndex = size() - 1;
        return topIndex >= 0 ? get(topIndex).getNestingLevel() : -1;
    }

    /**
     * Push an activation record onto the stack for a routine being called.
     * @param ar the activation record to push.
     */
    public void push(ActivationRecord ar)
    {
        int nestingLevel = ar.getNestingLevel();

        add(ar);
        display.callUpdate(nestingLevel, ar);
    }

    /**
     * Pop an activation record off the stack for a returning routine.
     */
    public void pop()
    {
        display.returnUpdate(currentNestingLevel());
        remove(size() - 1);
    }
}
