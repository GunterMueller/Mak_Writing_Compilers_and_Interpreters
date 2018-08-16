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
     * Generate code to allocate the string, array, and record variables
     * of a program, procedure, or function.
     * @param routineId the routine's symbol table entry.
     */
    public void generate(SymTabEntry routineId)
    {
        SymTab symTab = (SymTab) routineId.getAttribute(ROUTINE_SYMTAB);
        ArrayList<SymTabEntry> ids = symTab.sortedEntries();

        // Loop over all the symbol table's identifiers to generate
        // data allocation code for string, array, and record variables.
        emitBlankLine();
        for (SymTabEntry id : ids) {
            if (id.getDefinition() == VARIABLE) {
                TypeSpec idType = id.getTypeSpec();

                if (isStructured(idType)) {
                    TypeForm idForm = idType.getForm();

                    if (idType.isPascalString() || (idForm == RECORD)) {
                        generateAllocateData(id, idType);
                    }
                    else {
                        generateAllocateArray(id, idType);
                    }
                }
            }
        }
    }

    /**
     * Generate data to allocate a string, array, or record variable.
     * @param variableId the variable's symbol table entry.
     * @param type the variable's data type.
     */
    private void generateAllocateData(SymTabEntry variableId, TypeSpec type)
    {
        TypeForm form = type.getForm();

        if (type.isPascalString()) {
            generateAllocateString(variableId, type);
        }
        else if (form == ARRAY) {
            generateAllocateElements(variableId, type, 1);
        }
        else if (form == RECORD) {
            generateAllocateRecord(variableId, type);
        }
    }

    /**
     * Generate code to allocate data for an array variable.
     * @param variableId the symbol table entry of the variable.
     * @param arrayType the array type.
     */
    private void generateAllocateArray(SymTabEntry variableId,
                                       TypeSpec arrayType)
    {
        TypeSpec elmtType = arrayType;
        int dimCount = 0;

        // Count the dimensions and emit a load constant of each size.
        emitBlankLine();
        do {
            int size = (Integer) elmtType.getAttribute(ARRAY_ELEMENT_COUNT);
            ++dimCount;
            emitLoadConstant(size);
            elmtType = (TypeSpec) elmtType.getAttribute(ARRAY_ELEMENT_TYPE);
        } while ((elmtType.getForm() == ARRAY) &&
                 (! elmtType.isPascalString()));

        // The array element type.
        elmtType = elmtType.baseType();
        TypeForm elmtForm = elmtType.getForm();
        String typeName =
              elmtType == Predefined.integerType ? "int"
            : elmtType == Predefined.realType    ? "float"
            : elmtType == Predefined.booleanType ? "boolean"
            : elmtType == Predefined.charType    ? "char"
            : elmtForm == ENUMERATION            ? "int"
            : elmtForm == RECORD                 ? "java/util/HashMap"
            : elmtType.isPascalString()          ? "java/lang/StringBuilder"
            :                                      null;

        // One-dimensional array.
        if (dimCount == 1) {
            Instruction newArray = isStructured(elmtType) ? ANEWARRAY
                                                          : NEWARRAY;
            emit(newArray, typeName);
        }

        // Multidimensional array.
        else {
            emit(MULTIANEWARRAY, typeDescriptor(variableId.getTypeSpec()),
                 Integer.toString(dimCount));
        }

        localStack.use(dimCount+1, dimCount);

        // Allocate data for structured elements.
        if (isStructured(elmtType)) {
            generateAllocateData(variableId, variableId.getTypeSpec());
        }

        // Store the allocation into the array variable.
        generateStoreData(variableId);
    }

    /**
     * Generate code that loops over the array dimensions to allocate
     * data for each element.
     * @param variableId the symbol table entry of the variable.
     * @param elmtType the data type of the array element.
     * @param dimensionIndex the first dimension is 1, second is 2, etc.
     */
    private void generateAllocateElements(SymTabEntry variableId,
                                          TypeSpec elmtType, int dimensionIndex)
    {
        int count = (Integer) elmtType.getAttribute(ARRAY_ELEMENT_COUNT);
        int tempIndex = localVariables.reserve();  // temporary loop variable
        Label loopStartLabel = Label.newLabel();
        Label loopExitLabel  = Label.newLabel();

        // Initialize temporary variable to 0.
        emitBlankLine();
        emitLoadConstant(0);
        emitStoreLocal(Predefined.integerType, tempIndex);

        // Top of loop: Compare the temporary variable to the element count.
        emitLabel(loopStartLabel);
        emitLoadLocal(Predefined.integerType, tempIndex);
        emitLoadConstant(count);
        emit(IF_ICMPGE, loopExitLabel);

        emitBlankLine();
        emit(DUP);  // duplicate dimension address for use by each element

        localStack.use(4, 3);

        elmtType = (TypeSpec) elmtType.getAttribute(ARRAY_ELEMENT_TYPE);

        // Allocate data for a string or record element.
        if (elmtType.isPascalString() || (elmtType.getForm() == RECORD)) {
            emitLoadLocal(Predefined.integerType, tempIndex);
            localStack.increase(1);

            generateAllocateData(null, elmtType);
        }

        // Allocate data for the next array dimension.
        else {
            emitLoadLocal(Predefined.integerType, tempIndex);
            emit(AALOAD);
            localStack.use(2);

            generateAllocateElements(variableId, elmtType, dimensionIndex+1);
        }

        // Bottom of loop: Increment the temporary variable and
        //                 branch back to the top of the loop.
        emit(IINC, tempIndex, 1);
        emit(GOTO, loopStartLabel);
        emitLabel(loopExitLabel);

        // Pop off the extraneous duplicated dimension address.
        if (dimensionIndex > 1) {
            emit(POP);
            localStack.decrease(1);
        }

        localVariables.release(tempIndex);
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
        generateStoreData(variableId);
    }

    /**
     * Generate code to allocate a record variable as a HashMap.
     * @param variableId the symbol table entry of the variable.
     * @param recordType the record data type.
     */
    private void generateAllocateRecord(SymTabEntry variableId,
                                        TypeSpec recordType)
    {
        SymTab recordSymTab = (SymTab) recordType.getAttribute(RECORD_SYMTAB);

        // Allocate a hash map.
        emitBlankLine();
        emit(NEW, "java/util/HashMap");
        emit(DUP);
        emit(INVOKENONVIRTUAL, "java/util/HashMap/<init>()V");
        localStack.use(2, 1);

        // Allocate the record fields.
        generateAllocateFields(recordSymTab);

        // Store the allocation into the record variable.
        generateStoreData(variableId);
    }

    /**
     * Generate code to allocate array and record variables and fields.
     * @param symTab the record's symbol table containing fields.
     */
    protected void generateAllocateFields(SymTab symTab)
    {
        ArrayList<SymTabEntry> fields = symTab.sortedEntries();

        // Loop over all the symbol table's field identifiers
        // to generate data allocation code.
        for (SymTabEntry fieldId : fields) {
            TypeSpec idType = fieldId.getTypeSpec().baseType();

            // Load the field name.
            emit(DUP);
            emitLoadConstant(fieldId.getName());
            localStack.increase(2);

            // Allocate data for a string, array, or record field.
            if (isStructured(idType)) {
                TypeForm idForm = idType.getForm();

                if (idType.isPascalString() || (idForm == RECORD)) {
                    generateAllocateData(fieldId, idType);
                }
                else {
                    generateAllocateArray(fieldId, idType);
                }
            }

            // Initialize a scalar field.
            else {
                Object value = BackendFactory.defaultValue(idType);

                if (idType == Predefined.integerType) {
                    emitLoadConstant((Integer) value);
                }
                else if (idType == Predefined.realType) {
                    emitLoadConstant((Float) value);
                }
                else if (idType == Predefined.booleanType) {
                    emitLoadConstant((Boolean) value ? 1 : 0);
                }
                else /* idType == Predefined.charType */ {
                    emitLoadConstant((Character) value);
                }

                // Promote a scalar value to an object.
                emit(INVOKESTATIC, valueOfSignature(idType));
                localStack.increase(1);

                generateStoreData(fieldId);
            }
        }
    }

    /**
     * Generate code to store allocated data into a variable, record, field, or
     * array element.
     * @param variableId the variable's symbol table entry, or null to store
     * into an array element.
     */
    private void generateStoreData(SymTabEntry variableId)
    {
        // Store into an array element.
        if (variableId == null) {
            emit(AASTORE);
            localStack.decrease(3);
        }

        // Store into a record field.
        else if (variableId.getDefinition() == FIELD) {
            emit(INVOKEVIRTUAL,
                 "java/util/HashMap.put(Ljava/lang/Object;Ljava/lang/Object;)" +
                 "Ljava/lang/Object;");
            emit(POP);

            localStack.decrease(3);
        }

        // Store into a variable.
        else {
            emitStoreVariable(variableId);
            localStack.decrease(1);
        }
    }
}
