package wci.backend.interpreter.memoryimpl;

import java.util.ArrayList;

import wci.backend.interpreter.*;

/**
 * <h1>RuntimeDisplayImpl</h1>
 *
 * <p>The interpreter's runtime display.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class RuntimeDisplayImpl
    extends ArrayList<ActivationRecord>
    implements RuntimeDisplay
{
    /**
     * Constructor.
     */
    public RuntimeDisplayImpl()
    {
        add(null);  // dummy element 0 (never used)
    }

    /**
     * Get the activation record at a given nesting level.
     * @param nestingLevel the nesting level.
     * @return the activation record.
     */
    public ActivationRecord getActivationRecord(int nestingLevel)
    {
        return get(nestingLevel);
    }

    /**
     * Update the display for a call to a routine at a given nesting level.
     * @param nestingLevel the nesting level.
     * @param ar the activation record for the routine.
     */
    public void callUpdate(int nestingLevel, ActivationRecord ar)
    {
        // Next higher nesting level: Append a new element at the top.
        if (nestingLevel >= size()) {
            add(ar);
        }

        // Existing nesting level: Set at the specified level.
        else {
            ActivationRecord prevAr = get(nestingLevel);
            set(nestingLevel, ar.makeLinkTo(prevAr));
        }
    }

    /**
     * Update the display for a return from a routine at a given nesting level.
     * @param nestingLevel the nesting level.
     */
    public void returnUpdate(int nestingLevel)
    {
        int topIndex = size() - 1;
        ActivationRecord ar = get(nestingLevel);  // AR about to be popped off
        ActivationRecord prevAr = ar.linkedTo();  // previous AR it points to

        // Point the element at that nesting level to the
        // previous activation record.
        if (prevAr != null) {
            set(nestingLevel, prevAr);
        }

        // The top element has become null, so remove it.
        else if (nestingLevel == topIndex) {
            remove(topIndex);
        }
    }
}
