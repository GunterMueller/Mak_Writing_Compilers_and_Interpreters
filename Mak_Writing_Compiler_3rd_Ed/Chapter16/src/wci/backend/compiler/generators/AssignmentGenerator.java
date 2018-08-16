package wci.backend.compiler.generators;

import java.util.ArrayList;

import wci.intermediate.*;
import wci.intermediate.icodeimpl.*;
import wci.intermediate.symtabimpl.*;
import wci.backend.compiler.*;

import static wci.intermediate.symtabimpl.SymTabKeyImpl.*;
import static wci.intermediate.symtabimpl.DefinitionImpl.*;
import static wci.intermediate.typeimpl.TypeFormImpl.*;
import static wci.intermediate.typeimpl.TypeKeyImpl.*;
import static wci.intermediate.icodeimpl.ICodeNodeTypeImpl.*;
import static wci.intermediate.icodeimpl.ICodeKeyImpl.*;
import static wci.backend.compiler.Instruction.*;

/**
 * <h1>AssignmentExecutor</h1>
 *
 * <p>Generate code for an assignment statement.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class AssignmentGenerator extends StatementGenerator
{
    /**
     * Constructor.
     * @param the parent executor.
     */
    public AssignmentGenerator(CodeGenerator parent)
    {
        super(parent);
    }

    /**
     * Generate code for an assignment statement.
     * @param node the root node of the statement.
     */
    public void generate(ICodeNode node)
    {
        TypeSpec assignmentType = node.getTypeSpec();

        // The ASSIGN node's children are the target variable
        // and the expression.
        ArrayList<ICodeNode> assignChildren = node.getChildren();
        ICodeNode targetNode = assignChildren.get(0);
        ICodeNode exprNode = assignChildren.get(1);

        SymTabEntry targetId = (SymTabEntry) targetNode.getAttribute(ID);
        TypeSpec targetType = targetNode.getTypeSpec();
        TypeSpec exprType = exprNode.getTypeSpec();
        ExpressionGenerator exprGenerator = new ExpressionGenerator(this);

        int slot;          // local variables array slot number of the target
        int nestingLevel;  // nesting level of the target
        SymTab symTab;     // symbol table that contains the target id

        // Assign a function value. Use the slot number of the function value.
        if (targetId.getDefinition() == DefinitionImpl.FUNCTION) {
            slot = (Integer) targetId.getAttribute(SLOT);
            nestingLevel = 2;
        }

        // Standard assignment.
        else {
            symTab = targetId.getSymTab();
            slot = (Integer) targetId.getAttribute(SLOT);
            nestingLevel = symTab.getNestingLevel();
        }

        // Generate code to do the assignment.
        generateScalarAssignment(targetType, targetId,
                                 slot, nestingLevel, exprNode, exprType,
                                 exprGenerator);
    }

    /**
     * Generate code to assign a scalar value.
     * @param targetType the data type of the target.
     * @param targetId the symbol table entry of the target variable.
     * @param index the index of the target variable.
     * @param nestingLevel the nesting level of the target variable.
     * @param exprNode the expression tree node.
     * @param exprType the expression data type.
     * @param exprGenerator the expression generator.
     */
    private void generateScalarAssignment(TypeSpec targetType,
                                          SymTabEntry targetId,
                                          int index, int nestingLevel,
                                          ICodeNode exprNode,
                                          TypeSpec exprType,
                                          ExpressionGenerator exprGenerator)
    {
        // Generate code to evaluate the expression.
        // Special cases: float variable := integer constant
        //                float variable := integer expression
        //                char variable  := single-character string constant
        if (targetType == Predefined.realType) {
            if (exprNode.getType() == INTEGER_CONSTANT) {
                int value = (Integer) exprNode.getAttribute(VALUE);
                emitLoadConstant((float) value);
                localStack.increase(1);
            }
            else {
                exprGenerator.generate(exprNode);

                if (exprType.baseType() == Predefined.integerType) {
                    emit(I2F);
                }
            }
        }
        else if ((targetType == Predefined.charType) &&
                 (exprNode.getType() == STRING_CONSTANT)) {
            int value = ((String) exprNode.getAttribute(VALUE)).charAt(0);
            emitLoadConstant(value);
            localStack.increase(1);
        }
        else {
            exprGenerator.generate(exprNode);
        }

        // Generate code to store the expression value into the target variable.
        emitStoreVariable(targetId, nestingLevel, index);
        localStack.decrease(isWrapped(targetId) ? 2 : 1);
    }
}
