package wci.intermediate.symtabimpl;

import java.util.ArrayList;
import java.util.HashMap;

import wci.intermediate.*;

/**
 * <h1>SymTabEntryImpl</h1>
 *
 * <p>An implementation of a symbol table entry.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class SymTabEntryImpl
    extends HashMap<SymTabKey, Object>
    implements SymTabEntry
{
    private String name;                     // entry name
    private SymTab symTab;                   // parent symbol table
    private ArrayList<Integer> lineNumbers;  // source line numbers

    /**
     * Constructor.
     * @param name the name of the entry.
     * @param symTab the symbol table that contains this entry.
     */
    public SymTabEntryImpl(String name, SymTab symTab)
    {
        this.name = name;
        this.symTab = symTab;
        this.lineNumbers = new ArrayList<Integer>();
    }

    /**
     * Getter.
     * @return the name of the entry.
     */
    public String getName()
    {
        return name;
    }

    /**
     * Getter.
     * @return the symbol table that contains this entry.
     */
    public SymTab getSymTab()
    {
        return symTab;
    }

    /**
     * Append a source line number to the entry.
     * @param lineNumber the line number to append.
     */
    public void appendLineNumber(int lineNumber)
    {
        lineNumbers.add(lineNumber);
    }

    /**
     * Getter.
     * @return the list of source line numbers for the entry.
     */
    public ArrayList<Integer> getLineNumbers()
    {
        return lineNumbers;
    }

    /**
     * Set an attribute of the entry.
     * @param key the attribute key.
     * @param value the attribute value.
     */
    public void setAttribute(SymTabKey key, Object value)
    {
        put(key, value);
    }

    /**
     * Get the value of an attribute of the entry.
     * @param key the attribute key.
     * @return the attribute value.
     */
    public Object getAttribute(SymTabKey key)
    {
        return get(key);
    }
}
