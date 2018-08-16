package wci.intermediate;

import java.util.ArrayList;

/**
 * <h1>SymTab</h1>
 *
 * <p>The framework interface that represents the symbol table.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public interface SymTab
{
    /**
     * Getter.
     * @return the scope nesting level of this entry.
     */
    public int getNestingLevel();

    /**
     * Create and enter a new entry into the symbol table.
     * @param name the name of the entry.
     * @return the new entry.
     */
    public SymTabEntry enter(String name);

    /**
     * Look up an existing symbol table entry.
     * @param name the name of the entry.
     * @return the entry, or null if it does not exist.
     */
    public SymTabEntry lookup(String name);

    /**
     * @return a list of symbol table entries sorted by name.
     */
    public ArrayList<SymTabEntry> sortedEntries();
}
