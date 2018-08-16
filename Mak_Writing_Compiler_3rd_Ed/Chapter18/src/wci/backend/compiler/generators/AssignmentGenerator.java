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

        // Assign to a VAR parameter: Load the address of the wrapper.
        if (isWrapped(targetId)) {
            emitLoadLocal(null, slot);
            localStack.increase(1);
        }

        ArrayList<ICodeNode> targetChildren = targetNode.getChildren();
        int childrenCount = targetChildren.size();

        // The last modifier, if any, is the variable's last subscript or field.
        ICodeNode lastModifier = null;

        // The target variable has subscripts and/or fields.
        if (childrenCount > 0) {
            lastModifier = targetChildren.get(childrenCount-1);

            if (assignmentType.isPascalString()) {
                exprGenerator.generateLoadValue(targetNode);
            }
            else {
                assignmentType = exprGenerator.generateLoadVariable(targetNode);
            }
        }

        // Assign to a Pascal string.
        else if (assignmentType.isPascalString()) {
            emitLoadVariable(targetId);
            localStack.increase(1);
        }

        // Generate code to do the assignment.
        if (targetType.isPascalString()) {
            generateStringAssignment(assignmentType, exprNode,
                                     exprType, exprGenerator);
        }
        else {
            generateScalarAssignment(targetType, targetId, lastModifier,
                                     slot, nestingLevel, exprNode, exprType,
                                     exprGenerator);
        }
    }

    /**
     * Generate code to assign a scalar value.
     * @param targetType the data type of the target.
     * @param targetId the symbol table entry of the target variable.
     * @param lastModifier the tree node of the last field or subscript.
     * @param index the index of the target variable.
     * @param nestingLevel the nesting level of the target variable.
     * @param exprNode the expression tree node.
     * @param exprType the expression data type.
     * @param exprGenerator the expression generator.
     */
    private void generateScalarAssignment(TypeSpec targetType,
                                          SymTabEntry targetId,
                                          ICodeNode lastModifier,
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
        // The target variable has no subscripts or fields.
        if (lastModifier == null) {
            emitStoreVariable(targetId, nestingLevel, index);
            localStack.decrease(isWrapped(targetId) ? 2 : 1);
        }

        // The target variable is a field.
        else if (lastModifier.getType() == ICodeNodeTypeImpl.FIELD) {
            TypeSpec dataType = lastModifier.getTypeSpec().baseType();
            TypeForm typeForm = dataType.getForm();

            if ((typeForm == SCALAR) || (typeForm == ENUMERATION)) {
                emit(INVOKESTATIC, valueOfSignature(dataType));
            }

            emit(INVOKEVIRTUAL,
                 "java/util/HashMap.put(Ljava/lang/Object;" +
                 "Ljava/lang/Object;)Ljava/lang/Object;");
            emit(POP);
            localStack.decrease(3);
        }

        // The target variable is an array element.
        else {
            emitStoreArrayElement(targetType);
            localStack.decrease(3);
        }
    }

    private static final String SETLENGTH =
        "java/lang/StringBuilder.setLength(I)V";
    private static final String PAD_BLANKS =
        "PaddedString.blanks(II)Ljava/lang/StringBuilder;";

    private static final String APPEND_STRING =
        "java/lang/StringBuilder.append(Ljava/lang/String;)" +
        "Ljava/lang/StringBuilder;";
    private static final String APPEND_CHARSEQUENCE =
        "java/lang/StringBuilder.append(Ljava/lang/CharSequence;)" +
        "Ljava/lang/StringBuilder;";

    private static final String STRINGBUILDER_SUBSTRING =
        "java/lang/StringBuilder.substring(II)Ljava/lang/String;";
    private static final String STRING_SUBSTRING =
        "java/lang/String.substring(II)Ljava/lang/String;";

    /**
     * Generate code to assign a Pascal string value.
     * @param targetType the data type of the target variable.
     * @param exprNode the expression tree node.
     * @param exprType the expression data type.
     * @param exprGenerator the expression code generator.
     */
    private void generateStringAssignment(TypeSpec targetType,
                                          ICodeNode exprNode, TypeSpec exprType,
                                          ExpressionGenerator exprGenerator)
    {
        int targetLength =
                (Integer) targetType.getAttribute(ARRAY_ELEMENT_COUNT);
        int sourceLength;
        String appender;

        emit(DUP);
        emitLoadConstant(0);
        emit(INVOKEVIRTUAL, SETLENGTH);

        localStack.use(2, 2);

        // Generate code to load the source string.
        if (exprNode.getType() == STRING_CONSTANT) {
            String value = (String) exprNode.getAttribute(VALUE);
            sourceLength = value.length();
            appender = APPEND_STRING;
            emitLoadConstant(value);

            localStack.increase(1);
        }
        else {
            sourceLength = (Integer) exprType.getAttribute(ARRAY_ELEMENT_COUNT);
            appender = APPEND_CHARSEQUENCE;
            exprGenerator.generate(exprNode);
        }

        // Same lengths.
        if (targetLength == sourceLength) {
            emit(INVOKEVIRTUAL, appender);
            localStack.decrease(1);
        }

        // Truncate if necessary.
        else if (targetLength < sourceLength) {
            emitLoadConstant(0);
            emitLoadConstant(targetLength);

            String substringRoutine = exprNode.getType() == STRING_CONSTANT
                                          ? STRING_SUBSTRING
                                          : STRINGBUILDER_SUBSTRING;

            emit(INVOKEVIRTUAL, substringRoutine);
            emit(INVOKEVIRTUAL, appender);

            localStack.use(2, 3);
        }

        // Blank-pad if necessary.
        else {
            emit(INVOKEVIRTUAL, appender);
            emitLoadConstant(targetLength);
            emitLoadConstant(sourceLength);
            emit(INVOKESTATIC,PAD_BLANKS);
            emit(INVOKEVIRTUAL, APPEND_CHARSEQUENCE);

            localStack.use(2, 3);
        }

        emit(POP);
        localStack.decrease(1);
    }
}
