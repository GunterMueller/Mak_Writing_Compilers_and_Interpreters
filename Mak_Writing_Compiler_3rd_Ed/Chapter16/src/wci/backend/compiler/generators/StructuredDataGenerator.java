package wci.backend.compiler.generators;

import java.util.ArrayList;

import wci.intermediate.*;
import wci.intermediate.symtabimpl.*;
import wci.backend.*;
import wci.backend.compiler.*;

import static wci.intermediate.symtabimpl.SymTabKeyImpl.*;
import static wci.intermediate.symtabimpl.DefinitionImpl.*;
import static wci.intermediate.typeimpl.TypeFormImpl.*;
import static wci.intermediate.typeimpl.TypeKeyImpl.*;
import static wci.backend.compiler.Instruction.*;

/**
 * <h1>StructuredDataGenerator</h1>
 *
 * <p>Generate code to allocate arrays, records, and strings.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class StructuredDataGenerator extends CodeGenerator
{
    /**
     * Constructor.
     * @param the parent generator.
     */
    public StructuredDataGenerator(CodeGenerator parent)
    {
        super(parent);
    }

    /**
     * Generate code to allocate the structured data of a program,
     * procedure, or function.
     * @param routineId the routine's symbol table entry.
     */
    public void generate(SymTabEntry routineId)
    {
    }
}
