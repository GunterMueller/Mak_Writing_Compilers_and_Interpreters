package wci.backend.compiler.generators;

import java.util.ArrayList;

import wci.intermediate.*;
import wci.intermediate.symtabimpl.*;
import wci.backend.compiler.*;

import static wci.intermediate.symtabimpl.DefinitionImpl.*;
import static wci.intermediate.symtabimpl.SymTabKeyImpl.*;
import static wci.intermediate.icodeimpl.ICodeNodeTypeImpl.*;
import static wci.intermediate.icodeimpl.ICodeKeyImpl.*;
import static wci.backend.compiler.Instruction.*;

/**
 * <h1>CallDeclaredExecutor</h1>
 *
 * <p>Generate code to call a declared procedure or function.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class CallDeclaredGenerator extends CallGenerator
{
    /**
     * Constructor.
     * @param the parent executor.
     */
    public CallDeclaredGenerator(CodeGenerator parent)
    {
        super(parent);
    }

    /**
     * Generate code to call to a declared procedure or function.
     * @param node the CALL node.
     */
    public void generate(ICodeNode node)
    {
        // Generate code for any actual parameters.
        if (node.getChildren().size() > 0) {
            generateActualParms(node);
        }

        // Generate code to make the call.
        generateCall(node);

        // Generate code for the epilogue.
        if (node.getChildren().size() > 0) {
            generateCallEpilogue(node);
        }

        // A function call leaves a value on the operand stack.
        SymTabEntry routineId = (SymTabEntry) node.getAttribute(ID);
        if (routineId.getDefinition() == DefinitionImpl.FUNCTION) {
            localStack.increase(1);
        }
    }

    /**
     * Generate code for the actual parameters of a call.
     * @param callNode the CALL parse tree node.
     */
    private void generateActualParms(ICodeNode callNode)
    {
        SymTabEntry routineId = (SymTabEntry) callNode.getAttribute(ID);
        ICodeNode parmsNode = callNode.getChildren().get(0);
        ArrayList<ICodeNode> actualNodes = parmsNode.getChildren();
        ArrayList<SymTabEntry> formalIds =
            (ArrayList<SymTabEntry>) routineId.getAttribute(ROUTINE_PARMS);
        ExpressionGenerator exprGenerator = new ExpressionGenerator(this);

        // Iterate over the formal parameters.
        for (int i = 0; i < formalIds.size(); ++i) {
            SymTabEntry formalId = formalIds.get(i);
            ICodeNode actualNode = actualNodes.get(i);
            SymTabEntry actualId = (SymTabEntry) actualNode.getAttribute(ID);
            TypeSpec formalType = formalId.getTypeSpec();
            TypeSpec actualType = actualNode.getTypeSpec();

            // VAR parameter: An actual parameter that is not structured
            //                needs to be wrapped.
            if (isWrapped(formalId)) {
                Integer wrapSlot = (Integer) actualId.getAttribute(WRAP_SLOT);

                // Already wrapped: Load the wrapper.
                if (wrapSlot != null) {
                    emitLoadLocal(null, wrapSlot);
                    localStack.increase(1);
                }

                // Actual parameter is itself a VAR parameter: No further
                //                                             wrapping.
                else if (actualId.getDefinition() == VAR_PARM) {
                    int actualSlot = (Integer) actualId.getAttribute(SLOT);
                    emitLoadLocal(null, actualSlot);
                    localStack.increase(1);
                }

                // Need to wrap: Reserve a temporary variable to hold the
                //               wrapper's address.
                else {
                    wrapSlot = localVariables.reserve();
                    actualId.setAttribute(WRAP_SLOT, wrapSlot);
                    generateWrap(actualNode, formalType, wrapSlot,
                                 exprGenerator);
                }
            }

            // Value parameter: Actual parameter is a constant string.
            else if ((formalType == Predefined.charType) &&
                    (actualNode.getType() == STRING_CONSTANT)) {
                int value = ( (String) actualNode.getAttribute(VALUE))
                    .charAt(0);
                emitLoadConstant(value);
                localStack.increase(1);
            }

            // Value parameter: All other types.
            else {
                exprGenerator.generate(actualNode);
                emitRangeCheck(formalType);

                // real formal := integer actual
                if ((formalType == Predefined.realType) &&
                    (actualType.baseType() == Predefined.integerType))
                {
                    emit(I2F);
                }

                // Structured data needs to be cloned.
                else if (needsCloning(formalId)) {
                    cloneActualParameter(formalType);
                }
            }
        }
    }

    /**
     * Wrap an actual parameter to pass it by reference.
     * in a procedure or function call.
     * @param actualNode the parse tree node of the actual parameter.
     * @param formalType the type specification of the formal parameter
     * @param wrapSlot the slot number of the wrapper
     * @param exprGenerator the expression code generator.
     */
    private void generateWrap(ICodeNode actualNode, TypeSpec formalType,
                              int wrapSlot, ExpressionGenerator exprGenerator)
    {
        // Wrap the value of an actual parameter.
        String wrapper = varParmWrapper(formalType);  // selected wrapper

        // Create the wrapper.
        emit(NEW, wrapper);
        emit(DUP);
        localStack.increase(2);

        // Generate code to evaluate the actual parameter value.
        exprGenerator.generate(actualNode);

        // Invoke the wrapper's constructor to wrap the parameter value.
        String init = wrapper + "/<init>(" + typeDescriptor(formalType) + ")V";
        emit(INVOKENONVIRTUAL, init);
        localStack.decrease(1);

        // Store wrapper's address into a temporary variable and
        // leave a copy on the operand stack.
        emit(DUP);
        emitStoreLocal(null, wrapSlot);
        localStack.use(1);
    }

    /**
     * Clone an actual parameter value to be passed by value
     * in a procedure or function call.
     * @param formalType the type specification of the formal parameter
     */
    private void cloneActualParameter(TypeSpec formalType)
    {
        emit(INVOKESTATIC, "Cloner.deepClone(Ljava/lang/Object;)" +
                           "Ljava/lang/Object;");
        emitCheckCast(formalType);
    }

    /**
     * Generate code to make the call.
     * @param callNode the CALL parse tree node.
     */
    private void generateCall(ICodeNode callNode)
    {
        SymTabEntry routineId = (SymTabEntry) callNode.getAttribute(ID);
        String routineName = routineId.getName();
        ArrayList<SymTabEntry> parmIds =
            (ArrayList<SymTabEntry>) routineId.getAttribute(ROUTINE_PARMS);
        StringBuilder buffer = new StringBuilder();

        // Procedure or function name.
        buffer.append(programName);
        buffer.append("/");
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

        // Generate a call to the routine.
        emit(INVOKESTATIC, buffer.toString());

        if (parmIds != null) {
            localStack.decrease(parmIds.size());
        }
    }

    /**
     * Generate code for the call epilogue.
     * @param callNode the CALL parse tree node.
     */
    private void generateCallEpilogue(ICodeNode callNode)
    {
        SymTabEntry routineId = (SymTabEntry) callNode.getAttribute(ID);
        ICodeNode parmsNode = callNode.getChildren().get(0);
        ArrayList<ICodeNode> actualNodes = parmsNode.getChildren();
        ArrayList<SymTabEntry> formalIds =
            (ArrayList<SymTabEntry>) routineId.getAttribute(ROUTINE_PARMS);

        // Iterate over the formal parameters.
        for (int i = 0; i < formalIds.size(); ++i) {
            SymTabEntry formalId = formalIds.get(i);
            ICodeNode actualNode = actualNodes.get(i);
            TypeSpec formalType = formalId.getTypeSpec();

            // Wrapped parameters only.
            if (isWrapped(formalId)) {
                SymTabEntry actualId =
                                (SymTabEntry) actualNode.getAttribute(ID);

                // If the actual parameter is itself a VAR parameter,
                // keep it wrapped. Otherwise unwrap its value.
                if (actualId.getDefinition() != VAR_PARM) {
                    generateUnwrap(actualId, formalType, programName);
                }
            }
        }
    }

    /**
     * Generate the code to unwrap an actual parameter value.
     * @param actualId the symbol table entry of the actual identifier.
     * @param formalType the type specification of the formal parameter
     * @param programName the name of the program.
     */
    private void generateUnwrap(SymTabEntry actualId, TypeSpec formalType,
                                String programName)
    {
        SymTab symTab = actualId.getSymTab();
        int actualSlot = (Integer) actualId.getAttribute(SLOT);
        int wrapSlot = (Integer) actualId.getAttribute(WRAP_SLOT);
        String typeDesc = typeDescriptor(formalType);
        int nestingLevel = symTab.getNestingLevel();
        String wrapper = varParmWrapper(formalType);  // selected wrapper

        // Load the wrapper and get its value.
        emitLoadLocal(null, wrapSlot);
        emit(GETFIELD, wrapper + "/value", typeDesc);

        // Store the value back into the original variable.
        if (nestingLevel == 1) {
            String actualName = programName + "/" + actualId.getName();
            emit(PUTSTATIC, actualName, typeDesc);
        }
        else {
            emitStoreLocal(formalType, actualSlot);
        }

        localStack.use(1, 2);
    }
}
