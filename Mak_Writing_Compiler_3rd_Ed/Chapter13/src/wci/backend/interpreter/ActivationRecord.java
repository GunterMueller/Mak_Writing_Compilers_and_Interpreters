package wci.backend.interpreter;

import java.util.ArrayList;

import wci.intermediate.*;

/**
 * <h1>ActivationRecord</h1>
 *
 * <p>Interface for the interpreter's runtime activation record.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public interface ActivationRecord
{
    /**
     * Getter.
     * @return the symbol table entry of the routine's name.
     */
    public SymTabEntry getRoutineId();

    /**
     * Return the memory cell for the given name from the memory map.
     * @param name the name.
     * @return the cell.
     */
    public Cell getCell(String name);

    /**
     * @return the list of all the names in the memory map.
     */
    public ArrayList<String> getAllNames();

    /**
     * Getter.
     * @return the scope nesting level.
     */
    public int getNestingLevel();

    /**
     * @return the activation record to which this record is dynamically linked.
     */
    public ActivationRecord linkedTo();

    /**
     * Make a dynamic link from this activation record to another one.
     * @param ar the activation record to link to.
     * @return this activation record.
     */
    public ActivationRecord makeLinkTo(ActivationRecord ar);
}
