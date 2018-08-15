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
 * <h1>ProgramGenerator</h1>
 *
 * <p>Generate code for the main program.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class ProgramGenerator extends CodeGenerator
{
    private SymTabEntry programId;
    private String programName;

    /**
     * Constructor.
     * @param the parent generator.
     */
    public ProgramGenerator(CodeGenerator parent)
    {
        super(parent);
    }

    /**
     * Generate code for the main program.
     * @param node the root node of the program.
     */
    public void generate(ICodeNode node)
        throws PascalCompilerException
    {
        SymTabEntry programId = symTabStack.getProgramId();

        this.programId = programId;
        this.programName = programId.getName();
        localVariables = new LocalVariables(0);
        localStack = new LocalStack();

        emitDirective(CLASS_PUBLIC, programName);
        emitDirective(SUPER, "java/lang/Object");

        generateFields();
        generateConstructor();
        generateRoutines();
        generateMainMethod();
    }

    /**
     * Generate directives for the fields.
     */
    private void generateFields()
    {
        // Runtime timer and standard in.
        emitBlankLine();
        emitDirective(FIELD_PRIVATE_STATIC, "_runTimer",   "LRunTimer;");
        emitDirective(FIELD_PRIVATE_STATIC, "_standardIn", "LPascalTextIn;");

        SymTab symTab = (SymTab) programId.getAttribute(ROUTINE_SYMTAB);
        ArrayList<SymTabEntry> ids = symTab.sortedEntries();

        emitBlankLine();

        // Loop over all the program's identifiers and
        // emit a .field directive for each variable.
        for (SymTabEntry id : ids) {
            Definition defn = id.getDefinition();

            if (defn == VARIABLE) {
                emitDirective(FIELD_PRIVATE_STATIC, id.getName(),
                              typeDescriptor(id));
            }
        }
    }

    /**
     * Generate code for the main program constructor.
     */
    private void generateConstructor()
    {
        emitBlankLine();
        emitDirective(METHOD_PUBLIC, "<init>()V");

        emitBlankLine();
        emit(ALOAD_0);
        emit(INVOKENONVIRTUAL, "java/lang/Object/<init>()V");
        emit(RETURN);

        emitBlankLine();
        emitDirective(LIMIT_LOCALS, 1);
        emitDirective(LIMIT_STACK , 1);

        emitDirective(END_METHOD);
    }

    /**
     * Generate code for any nested procedures and functions.
     */
    private void generateRoutines()
        throws PascalCompilerException
    {
        DeclaredRoutineGenerator declaredRoutineGenerator =
            new DeclaredRoutineGenerator(this);
        ArrayList<SymTabEntry> routineIds =
            (ArrayList<SymTabEntry>) programId.getAttribute(ROUTINE_ROUTINES);

        // Generate code for each procedure or function.
        for (SymTabEntry id : routineIds) {
            declaredRoutineGenerator.generate(id);
        }
    }

    /**
     * Generate code for the program body as the main method.
     */
    private void generateMainMethod()
        throws PascalCompilerException
    {
        emitBlankLine();
        emitDirective(METHOD_PUBLIC_STATIC, "main([Ljava/lang/String;)V");

        generateMainMethodPrologue();

        // Generate code to allocate any arrays, records, and strings.
        StructuredDataGenerator structuredDataGenerator =
                                    new StructuredDataGenerator(this);
        structuredDataGenerator.generate(programId);

        generateMainMethodCode();
        generateMainMethodEpilogue();
    }

    /**
     * Generate the main method prologue.
     */
    private void generateMainMethodPrologue()
    {
        String programName = programId.getName();

        // Runtime timer.
        emitBlankLine();
        emit(NEW, "RunTimer");
        emit(DUP);
        emit(INVOKENONVIRTUAL, "RunTimer/<init>()V");
        emit(PUTSTATIC, programName + "/_runTimer", "LRunTimer;");

        // Standard in.
        emit(NEW, "PascalTextIn");
        emit(DUP);
        emit(INVOKENONVIRTUAL, "PascalTextIn/<init>()V");
        emit(PUTSTATIC, programName + "/_standardIn LPascalTextIn;");

        localStack.use(3);
    }

    /**
     * Generate code for the main method.
     */
    private void generateMainMethodCode()
        throws PascalCompilerException
    {
        ICode iCode = (ICode) programId.getAttribute(ROUTINE_ICODE);
        ICodeNode root = iCode.getRoot();

        emitBlankLine();

        // Generate code for the compound statement.
        StatementGenerator statementGenerator = new StatementGenerator(this);
        statementGenerator.generate(root);
    }

    /**
     * Generate the main method epilogue.
     */
    private void generateMainMethodEpilogue()
    {
        // Print the execution time.
        emitBlankLine();
        emit(GETSTATIC, programName + "/_runTimer", "LRunTimer;");
        emit(INVOKEVIRTUAL, "RunTimer.printElapsedTime()V");

        localStack.use(1);

        emitBlankLine();
        emit(RETURN);
        emitBlankLine();

        emitDirective(LIMIT_LOCALS, localVariables.count());
        emitDirective(LIMIT_STACK,  localStack.capacity());
        emitDirective(END_METHOD);
    }
}
