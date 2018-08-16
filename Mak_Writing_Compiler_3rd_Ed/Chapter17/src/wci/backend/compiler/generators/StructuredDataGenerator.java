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
     * Generate code to allocate the string variables of a program,
     * procedure, or function.
     * @param routineId the routine's symbol table entry.
     */
    public void generate(SymTabEntry routineId)
    {
        SymTab symTab = (SymTab) routineId.getAttribute(ROUTINE_SYMTAB);
        ArrayList<SymTabEntry> ids = symTab.sortedEntries();

        // Loop over all the symbol table's identifiers to generate
        // data allocation code for string variables.
        emitBlankLine();
        for (SymTabEntry id : ids) {
            if (id.getDefinition() == VARIABLE) {
                TypeSpec idType = id.getTypeSpec();

                if (idType.isPascalString()) {
                    generateAllocateString(id, idType);
                }
            }
        }
    }

    private static final String PADDED_STRING_CREATE =
        "PaddedString.create(I)Ljava/lang/StringBuilder;";

    /**
     * Generate code to allocate a string variable as a StringBuilder.
     * @param variableId the symbol table entry of the variable.
     * @param stringType the string data type.
     */
    private void generateAllocateString(SymTabEntry variableId,
                                        TypeSpec stringType)
    {
        int length = (Integer) stringType.getAttribute(ARRAY_ELEMENT_COUNT);

        // Allocate a blank-filled string of the correct length.
        emitLoadConstant(length);
        emit(INVOKESTATIC, PADDED_STRING_CREATE);
        localStack.increase(1);

        // Store the allocation into the string variable.
        emitStoreVariable(variableId);
        localStack.decrease(1);
    }
}
