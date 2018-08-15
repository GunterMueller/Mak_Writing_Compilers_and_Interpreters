package wci.intermediate;

import wci.intermediate.typeimpl.*;

/**
 * <h1>TypeFactory</h1>
 *
 * <p>A factory for creating type specifications.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class TypeFactory
{
    /**
     * Create a type specification of a given form.
     * @param form the form.
     * @return the type specification.
     */
    public static TypeSpec createType(TypeForm form)
    {
        return new TypeSpecImpl(form);
    }

    /**
     * Create a string type specification.
     * @param value the string value.
     * @return the type specification.
     */
    public static TypeSpec createStringType(String value)
    {
        return new TypeSpecImpl(value);
    }
}
