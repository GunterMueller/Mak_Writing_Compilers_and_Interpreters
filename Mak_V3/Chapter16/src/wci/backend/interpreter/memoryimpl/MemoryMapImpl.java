package wci.backend.interpreter.memoryimpl;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Set;

import wci.intermediate.*;
import wci.intermediate.typeimpl.*;
import wci.backend.interpreter.*;

import static wci.intermediate.symtabimpl.SymTabKeyImpl.*;
import static wci.intermediate.symtabimpl.DefinitionImpl.*;
import static wci.intermediate.typeimpl.TypeFormImpl.*;
import static wci.intermediate.typeimpl.TypeKeyImpl.*;

/**
 * <h1>MemoryMapImpl</h1>
 *
 * <p>The interpreter's runtime memory map.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class MemoryMapImpl
    extends HashMap<String, Cell>
    implements MemoryMap
{
    /**
     * Constructor.
     * Create a memory map and allocate its memory cells
     * based on the entries in a symbol table.
     * @param symTab the symbol table.
     */
    public MemoryMapImpl(SymTab symTab)
    {
        ArrayList<SymTabEntry> entries = symTab.sortedEntries();

        // Loop for each entry of the symbol table.
        for (SymTabEntry entry : entries) {
            Definition defn = entry.getDefinition();

            // Not a VAR parameter: Allocate cells for the data type
            //                      in the hashmap.
            if ((defn == VARIABLE) || (defn == FUNCTION) ||
                (defn == VALUE_PARM) || (defn == FIELD))
            {
                String name = entry.getName();
                TypeSpec type = entry.getTypeSpec();
                put(name, MemoryFactory.createCell(allocateCellValue(type)));
            }

            // VAR parameter: Allocate a single cell to hold a reference
            //                in the hashmap.
            else if (defn == VAR_PARM) {
                String name = entry.getName();
                put(name, MemoryFactory.createCell(null));
            }
        }
    }

    /**
     * Return the memory cell with the given name.
     * @param name the name.
     * @return the cell.
     */
    public Cell getCell(String name)
    {
        return get(name);
    }

    /**
     * @return the list of all the names.
     */
    public ArrayList<String> getAllNames()
    {
        ArrayList<String> list = new ArrayList<String>();

        Set<String> names = keySet();
        Iterator<String> it = names.iterator();

        while (it.hasNext()) {
            list.add(it.next());
        }

        return list;
    }

    /**
     * Make an allocation for a value of a given data type for a memory cell.
     * @param type the data type.
     * @return the allocation.
     */
    private Object allocateCellValue(TypeSpec type)
    {
        TypeForm form = type.getForm();

        switch ((TypeFormImpl) form) {

            case ARRAY: {
                return allocateArrayCells(type);
            }

            case RECORD: {
                return allocateRecordMap(type);
            }

            default: {
                return null;  // uninitialized scalar value
            }
        }
    }

    /**
     * Allocate the memory cells of an array.
     * @param type the array type.
     * @return the allocation.
     */
    private Object[] allocateArrayCells(TypeSpec type)
    {
        int elmtCount = (Integer) type.getAttribute(ARRAY_ELEMENT_COUNT);
        TypeSpec elmtType = (TypeSpec) type.getAttribute(ARRAY_ELEMENT_TYPE);
        Cell allocation[] = new Cell[elmtCount];

        for (int i = 0; i < elmtCount; ++i) {
            allocation[i] =
                MemoryFactory.createCell(allocateCellValue(elmtType));
        }

        return allocation;
    }

    /**
     * Allocate the memory map for a record.
     * @param type the record type.
     * @return the allocation.
     */
    private MemoryMap allocateRecordMap(TypeSpec type)
    {
        SymTab symTab = (SymTab) type.getAttribute(RECORD_SYMTAB);
        MemoryMap memoryMap = MemoryFactory.createMemoryMap(symTab);

        return memoryMap;
    }
}
