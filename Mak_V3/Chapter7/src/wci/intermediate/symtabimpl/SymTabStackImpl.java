package wci.intermediate.symtabimpl;

import java.util.ArrayList;

import wci.intermediate.*;

/**
 * <h1>SymTabStack</h1>
 *
 * <p>An implementation of the symbol table stack.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class SymTabStackImpl
    extends ArrayList<SymTab>
    implements SymTabStack
{
    private int currentNestingLevel;  // current scope nesting level

    /**
     * Constructor.
     */
    public SymTabStackImpl()
    {
        this.currentNestingLevel = 0;
        add(SymTabFactory.createSymTab(currentNestingLevel));
    }

    /**
     * Getter.
     * @return the current nesting level.
     */
    public int getCurrentNestingLevel()
    {
        return currentNestingLevel;
    }

    /**
     * Return the local symbol table which is at the top of the stack.
     * @return the local symbol table.
     */
    public SymTab getLocalSymTab()
    {
        return get(currentNestingLevel);
    }

    /**
     * Create and enter a new entry into the local symbol table.
     * @param name the name of the entry.
     * @return the new entry.
     */
    public SymTabEntry enterLocal(String name)
    {
        return get(currentNestingLevel).enter(name);
    }

    /**
     * Look up an existing symbol table entry in the local symbol table.
     * @param name the name of the entry.
     * @return the entry, or null if it does not exist.
     */
    public SymTabEntry lookupLocal(String name)
    {
        return get(currentNestingLevel).lookup(name);
    }

    /**
     * Look up an existing symbol table entry throughout the stack.
     * @param name the name of the entry.
     * @return the entry, or null if it does not exist.
     */
    public SymTabEntry lookup(String name)
    {
        return lookupLocal(name);
    }
}
