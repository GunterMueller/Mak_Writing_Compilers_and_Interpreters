package wci.backend.interpreter;

import wci.intermediate.SymTab;
import wci.backend.interpreter.memoryimpl.*;
import wci.intermediate.SymTabEntry;

import static wci.intermediate.symtabimpl.SymTabKeyImpl.*;

/**
 * <h1>MemoryFactory</h1>
 *
 * <p>A factory class that creates runtime components.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class MemoryFactory
{
    /**
     * Create a runtime stack.
     * @return the new runtime stacki.
     */
    public static RuntimeStack createRuntimeStack()
    {
        return new RuntimeStackImpl();
    }

    /**
     * Create a runtime display.
     * @return the new runtime display.
     */
    public static RuntimeDisplay createRuntimeDisplay()
    {
        return new RuntimeDisplayImpl();
    }

    /**
     * Create an activation record for a routine.
     * @param routineId the symbol table entry of the routine's name.
     * @return the new activation record.
     */
    public static ActivationRecord createActivationRecord(SymTabEntry routineId)
    {
        return new ActivationRecordImpl(routineId);
    }

    /**
     * Create a memory map from a symbol table.
     * @param value the value for the cell.
     * @return the new memory map.
     */
    public static MemoryMap createMemoryMap(SymTab symTab)
    {
        return new MemoryMapImpl(symTab);
    }

    /**
     * Create a memory cell.
     * @param value the value for the cell.
     * @return the new memory cell.
     */
    public static Cell createCell(Object value)
    {
        return new CellImpl(value);
    }
}
