package wci.intermediate.typeimpl;

import wci.intermediate.TypeForm;

/**
 * <h1>TypeFormImpl</h1>
 *
 * <p>Type forms for a Pascal type specification.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public enum TypeFormImpl implements TypeForm
{
    SCALAR, ENUMERATION, SUBRANGE, ARRAY, RECORD;

    public String toString()
    {
        return super.toString().toLowerCase();
    }
}
