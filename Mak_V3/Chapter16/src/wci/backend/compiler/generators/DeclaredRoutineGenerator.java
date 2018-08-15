package wci.backend.compiler.generators;

import java.util.ArrayList;

import wci.intermediate.*;
import wci.intermediate.symtabimpl.*;
import wci.backend.compiler.*;

import static wci.intermediate.symtabimpl.SymTabKeyImpl.*;
import static wci.intermediate.symtabimpl.DefinitionImpl.*;
import static wci.backend.compiler.Directive.*;
import static wci.backend.compiler.Instruction.*;

/**
 * <h1>DeclaredRoutineGenerator</h1>
 *
 * <p>Generate code for a declared procedure or function.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class DeclaredRoutineGenerator extends CodeGenerator
{
    private SymTabEntry routineId;
    private String routineName;

    private int functionValueSlot;  // function return value slot number

    /**
     * Constructor.
     * @param the parent generator.
     */
    public DeclaredRoutineGenerator(CodeGenerator parent)
    {
        super(parent);
    }

    /**
     * Generate code for a declared procedure or function
     * @param routineId the symbol table entry of the routine's name.
     */
    public void generate(SymTabEntry routineId)
        throws PascalCompilerException
    {
        this.routineId = routineId;
        this.routineName = routineId.getName();

        SymTab routineSymTab = (SymTab) routineId.getAttribute(ROUTINE_SYMTAB);
        localVariables = new LocalVariables(routineSymTab.maxSlotNumber());
        localStack = new LocalStack();

        // Reserve an extra variable for the function return value.
        if (routineId.getDefinition() == FUNCTION) {
            functionValueSlot = localVariables.reserve();
            routineId.setAttribute(SLOT, functionValueSlot);
        }

        generateRoutineHeader();
        generateRoutineLocals();

        // Generate code to allocate any arrays, records, and strings.
        StructuredDataGenerator structuredDataGenerator =
                                    new StructuredDataGenerator(this);
        structuredDataGenerator.generate(routineId);

        generateRoutineCode();
        generateRoutineReturn();
        generateRoutineEpilogue();
    }

    /**
     * Generate the routine header.
     */
    private void generateRoutineHeader()
    {
        String routineName = routineId.getName();
        ArrayList<SymTabEntry> parmIds =
            (ArrayList<SymTabEntry>) routineId.getAttribute(ROUTINE_PARMS);
        StringBuilder buffer = new StringBuilder();

        // Procedure or function name.
        buffer.append(routineName);
        buffer.append("(");

        // Parameter and return type descriptors.
        if (parmIds != null) {
            for (SymTabEntry parmId : parmIds) {
                buffer.append(typeDescriptor(parmId));
            }
        }
        buffer.append(")");
        buffer.append(typeDescriptor(routineId));

        emitBlankLine();
        emitDirective(METHOD_PRIVATE_STATIC, buffer.toString());
    }

    /**
     * Generate directives for the local variables.
     */
    private void generateRoutineLocals()
    {
        SymTab symTab = (SymTab) routineId.getAttribute(ROUTINE_SYMTAB);
        ArrayList<SymTabEntry> ids = symTab.sortedEntries();

        emitBlankLine();

        // Loop over all the routine's identifiers and
        // emit a .var directive for each variable and formal parameter.
        for (SymTabEntry id : ids) {
            Definition defn = id.getDefinition();

            if ((defn == VARIABLE) || (defn == VALUE_PARM)
                                   || (defn == VAR_PARM)) {
                int slot = (Integer) id.getAttribute(SLOT);
                emitDirective(VAR, slot + " is " + id.getName(),
                              typeDescriptor(id));
            }
        }

        // Emit an extra .var directive for an implied function variable.
        if (routineId.getDefinition() == FUNCTION) {
            emitDirective(VAR, functionValueSlot + " is " + routineName,
                          typeDescriptor(routineId.getTypeSpec()));
        }
    }

    /**
     * Generate code for the routine's body.
     */
    private void generateRoutineCode()
        throws PascalCompilerException
    {
        ICode iCode = (ICode) routineId.getAttribute(ROUTINE_ICODE);
        ICodeNode root = iCode.getRoot();

        emitBlankLine();

        // Generate code for the compound statement.
        StatementGenerator statementGenerator = new StatementGenerator(this);
        statementGenerator.generate(root);
    }

    /**
     * Generate the routine's return code.
     */
    private void generateRoutineReturn()
    {
        emitBlankLine();

        // Function: Return the value in the implied function variable.
        if (routineId.getDefinition() == FUNCTION) {
            TypeSpec type = routineId.getTypeSpec();

            emitLoadLocal(type, functionValueSlot);
            emitReturnValue(type);

            localStack.use(1);
        }

        // Procedure: Just return.
        else {
            emit(RETURN);
        }
    }

    /**
     * Generate the routine's epilogue.
     */
    private void generateRoutineEpilogue()
    {
        emitBlankLine();
        emitDirective(LIMIT_LOCALS, localVariables.count());
        emitDirective(LIMIT_STACK,  localStack.capacity());
        emitDirective(END_METHOD);
    }
}
