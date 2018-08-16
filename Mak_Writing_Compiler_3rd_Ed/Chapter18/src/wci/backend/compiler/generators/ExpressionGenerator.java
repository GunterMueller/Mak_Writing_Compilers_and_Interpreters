package wci.backend.compiler.generators;

import java.util.ArrayList;
import java.util.EnumSet;

import wci.intermediate.*;
import wci.intermediate.symtabimpl.*;
import wci.intermediate.icodeimpl.*;
import wci.intermediate.typeimpl.*;
import wci.backend.compiler.*;

import static wci.intermediate.symtabimpl.DefinitionImpl.*;
import static wci.intermediate.symtabimpl.SymTabKeyImpl.*;
import static wci.intermediate.symtabimpl.RoutineCodeImpl.*;
import static wci.intermediate.typeimpl.TypeKeyImpl.*;
import static wci.intermediate.typeimpl.TypeFormImpl.*;
import static wci.intermediate.icodeimpl.ICodeNodeTypeImpl.*;
import static wci.intermediate.icodeimpl.ICodeKeyImpl.*;
import static wci.backend.compiler.Instruction.*;

/**
 * <h1>ExpressionGenerator</h1>
 *
 * <p>Generate code for an expression.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class ExpressionGenerator extends StatementGenerator
{
    /**
     * Constructor.
     * @param the parent executor.
     */
    public ExpressionGenerator(CodeGenerator parent)
    {
        super(parent);
    }

    /**
     * Generate code to evaluate an expression.
     * @param node the root intermediate code node of the compound statement.
     */
    public void generate(ICodeNode node)
    {
        ICodeNodeTypeImpl nodeType = (ICodeNodeTypeImpl) node.getType();

        switch (nodeType) {

            case VARIABLE: {

                // Generate code to load a variable's value.
                generateLoadValue(node);
                break;
            }

            case INTEGER_CONSTANT: {
                TypeSpec type = node.getTypeSpec();
                Integer value = (Integer) node.getAttribute(VALUE);

                // Generate code to load a boolean constant
                // 0 (false) or 1 (true).
                if (type == Predefined.booleanType) {
                    emitLoadConstant(value == 1 ? 1 : 0);
                }

                // Generate code to load an integer constant.
                else {
                    emitLoadConstant(value);
                }

                localStack.increase(1);
                break;
            }

            case REAL_CONSTANT: {
                float value = (Float) node.getAttribute(VALUE);

                // Generate code to load a float constant.
                emitLoadConstant(value);

                localStack.increase(1);
                break;
            }

            case STRING_CONSTANT: {
                String value = (String) node.getAttribute(VALUE);

                // Generate code to load a string constant.
                if (node.getTypeSpec() == Predefined.charType) {
                    emitLoadConstant(value.charAt(0));
                }
                else {
                    emitLoadConstant(value);
                }

                localStack.increase(1);
                break;
            }

            case NEGATE: {

                // Get the NEGATE node's expression node child.
                ArrayList<ICodeNode> children = node.getChildren();
                ICodeNode expressionNode = children.get(0);

                // Generate code to evaluate the expression and
                // negate its value.
                generate(expressionNode);
                emit(expressionNode.getTypeSpec() == Predefined.integerType
                         ? INEG : FNEG);

                break;
            }

            case NOT: {

                // Get the NOT node's expression node child.
                ArrayList<ICodeNode> children = node.getChildren();
                ICodeNode expressionNode = children.get(0);

                // Generate code to evaluate the expression and NOT its value.
                generate(expressionNode);
                emit(ICONST_1);
                emit(IXOR);

                localStack.use(1);
                break;
            }

            case CALL: {

                // Generate code to call a function.
                CallGenerator callGenerator = new CallGenerator(this);
                callGenerator.generate(node);

                break;
            }

            // Must be a binary operator.
            default: generateBinaryOperator(node, nodeType);
        }
    }

    /**
     * Generate code to load a variable's value.
     * @param variableNode the variable node.
     */
    protected void generateLoadValue(ICodeNode variableNode)
    {
        ArrayList<ICodeNode> variableChildren = variableNode.getChildren();
        int childrenCount = variableChildren.size();

        // First load the variable's address (structured) or value (scalar).
        TypeSpec variableType = generateLoadVariable(variableNode);

        // Were there any subscript or field modifiers?
        if (childrenCount > 0) {
            ICodeNodeType lastModifierType =
                              variableChildren.get(childrenCount-1).getType();

            // Array subscript.
            if (lastModifierType == SUBSCRIPTS) {
                emitLoadArrayElement(variableType);
                localStack.decrease(1);
            }

            // Record field.
            else {
                emit(INVOKEVIRTUAL,
                     "java/util/HashMap.get(Ljava/lang/Object;)" +
                     "Ljava/lang/Object;");
                emitCheckCastClass(variableType);

                if (!isStructured(variableType)) {
                    emit(INVOKEVIRTUAL, valueSignature(variableType));
                }

                localStack.decrease(1);
            }
        }
    }

    /**
     * Generate code to load a variable's address (structured) or
     * value (scalar).
     * @param variableNode the variable node.
     */
    protected TypeSpec generateLoadVariable(ICodeNode variableNode)
    {
        SymTabEntry variableId = (SymTabEntry) variableNode.getAttribute(ID);
        TypeSpec variableType = variableId.getTypeSpec();
        ArrayList<ICodeNode> variableChildren = variableNode.getChildren();
        int childrenCount = variableChildren.size();

        emitLoadVariable(variableId);
        localStack.increase(1);

        // Process subscripts and/or fields.
        for (int i = 0; i < childrenCount; ++i) {
            ICodeNode modifier = variableChildren.get(i);
            ICodeNodeType modifierType = modifier.getType();
            boolean last = i == childrenCount-1;

            if (modifierType == SUBSCRIPTS) {
                variableType = generateArrayElement(modifier, variableType,
                                                    last);
            }
            else if (modifierType == ICodeNodeTypeImpl.FIELD) {
                variableType = generateRecordField(modifier, last);
            }
        }

        return variableType;
    }

    /**
     * Generate code for a subscripted variable.
     * @param subscriptsNode the SUBSCRIPTS node.
     * @param variableType the array variable type.
     * @param last true if this is the variable's last subscript, else false.
     * @return the type of the element.
     */
    private TypeSpec generateArrayElement(ICodeNode subscriptsNode,
                                          TypeSpec variableType, boolean last)
    {
        ArrayList<ICodeNode> subscripts = subscriptsNode.getChildren();
        ICodeNode lastSubscript = subscripts.get(subscripts.size()-1);
        TypeSpec elmtType = variableType;

        for (ICodeNode subscript : subscripts) {
            generate(subscript);

            TypeSpec indexType =
                         (TypeSpec) elmtType.getAttribute(ARRAY_INDEX_TYPE);

            if (indexType.getForm() == SUBRANGE) {
                int min = (Integer) indexType.getAttribute(SUBRANGE_MIN_VALUE);
                if (min != 0) {
                    emitLoadConstant(min);
                    emit(ISUB);
                    localStack.use(1);
                }
            }

            if (!last || (subscript != lastSubscript)) {
                emit(AALOAD);
                localStack.decrease(1);
            }

            elmtType = (TypeSpec) elmtType.getAttribute(ARRAY_ELEMENT_TYPE);
        }

        return elmtType;
    }

    /**
     * Generate code to access the value of a record field.
     * @param fieldNode the FIELD node.
     * @param last true if this is the variable's last field, else false.
     * @return the type of the field.
     */
    private TypeSpec generateRecordField(ICodeNode fieldNode, boolean last)
    {
        SymTabEntry fieldId = (SymTabEntry) fieldNode.getAttribute(ID);
        String fieldName = fieldId.getName();
        TypeSpec fieldType = fieldNode.getTypeSpec();

        emitLoadConstant(fieldName);
        localStack.increase(1);

        if (!last) {
            emit(INVOKEVIRTUAL,
                 "java/util/HashMap.get(Ljava/lang/Object;)" +
                 "Ljava/lang/Object;");
            emitCheckCast(fieldType);
            localStack.decrease(1);
        }

        return fieldType;
    }

    // Set of arithmetic operator node types.
    private static final EnumSet<ICodeNodeTypeImpl> ARITH_OPS =
        EnumSet.of(ADD, SUBTRACT, MULTIPLY, FLOAT_DIVIDE, INTEGER_DIVIDE, MOD);

    /**
     * Generate code to evaluate a binary operator.
     * @param node the root node of the expression.
     * @param nodeType the node type.
     */
    private void generateBinaryOperator(ICodeNode node,
                                        ICodeNodeTypeImpl nodeType)
    {
        // Get the two operand children of the operator node.
        ArrayList<ICodeNode> children = node.getChildren();
        ICodeNode operandNode1 = children.get(0);
        ICodeNode operandNode2 = children.get(1);
        TypeSpec type1 = operandNode1.getTypeSpec();
        TypeSpec type2 = operandNode2.getTypeSpec();

        boolean integerMode = TypeChecker.areBothInteger(type1, type2) ||
                              (type1.getForm() == ENUMERATION) |
                              (type2.getForm() == ENUMERATION);
        boolean realMode = TypeChecker.isAtLeastOneReal(type1, type2) ||
                           (nodeType == FLOAT_DIVIDE);
        boolean characterMode = TypeChecker.isChar(type1) &&
                                TypeChecker.isChar(type2);
        boolean stringMode = type1.isPascalString() &&
                             type2.isPascalString();

        if (!stringMode) {
            // Emit code to evaluate the first operand.
            generate(operandNode1);
            if (realMode && TypeChecker.isInteger(type1)) {
                emit(I2F);
            }

            // Emit code to evaluate the second operand.
            generate(operandNode2);
            if (realMode && TypeChecker.isInteger(type2)) {
                emit(I2F);
            }
        }

        // ====================
        // Arithmetic operators
        // ====================

        if (ARITH_OPS.contains(nodeType)) {
            if (integerMode) {

                // Integer operations.
                switch (nodeType) {
                    case ADD:            emit(IADD); break;
                    case SUBTRACT:       emit(ISUB); break;
                    case MULTIPLY:       emit(IMUL); break;
                    case FLOAT_DIVIDE:   emit(FDIV); break;
                    case INTEGER_DIVIDE: emit(IDIV); break;
                    case MOD:            emit(IREM); break;
                }
            }
            else {

                // Float operations.
                switch (nodeType) {
                    case ADD:          emit(FADD); break;
                    case SUBTRACT:     emit(FSUB); break;
                    case MULTIPLY:     emit(FMUL); break;
                    case FLOAT_DIVIDE: emit(FDIV); break;
                }
            }

            localStack.decrease(1);
        }

        // ==========
        // AND and OR
        // ==========

        else if (nodeType == AND) {
            emit(IAND);
            localStack.decrease(1);
        }
        else if (nodeType == OR) {
            emit(IOR);
            localStack.decrease(1);
        }

        // ====================
        // Relational operators
        // ====================

        else {
            Label trueLabel = Label.newLabel();
            Label nextLabel = Label.newLabel();

            if (integerMode || characterMode) {
                switch (nodeType) {
                    case EQ: emit(IF_ICMPEQ, trueLabel); break;
                    case NE: emit(IF_ICMPNE, trueLabel); break;
                    case LT: emit(IF_ICMPLT, trueLabel); break;
                    case LE: emit(IF_ICMPLE, trueLabel); break;
                    case GT: emit(IF_ICMPGT, trueLabel); break;
                    case GE: emit(IF_ICMPGE, trueLabel); break;
                }

                localStack.decrease(2);
            }

            else if (realMode) {
                emit(FCMPG);

                switch (nodeType) {
                    case EQ: emit(IFEQ, trueLabel); break;
                    case NE: emit(IFNE, trueLabel); break;
                    case LT: emit(IFLT, trueLabel); break;
                    case LE: emit(IFLE, trueLabel); break;
                    case GT: emit(IFGT, trueLabel); break;
                    case GE: emit(IFGE, trueLabel); break;
                }

                localStack.decrease(2);
            }

            else if (stringMode) {

                // Load the value of the first string operand.
                generate(operandNode1);
                if (operandNode1.getType() != STRING_CONSTANT) {
                    emit(INVOKEVIRTUAL,
                         "java/lang/StringBuilder.toString()" +
                         "Ljava/lang/String;");
                }

                // Load the value of the second string operand.
                generate(operandNode2);
                if (operandNode2.getType() != STRING_CONSTANT) {
                    emit(INVOKEVIRTUAL,
                         "java/lang/StringBuilder.toString()" +
                         "Ljava/lang/String;");
                }

                emit(INVOKEVIRTUAL,
                     "java/lang/String.compareTo(Ljava/lang/String;)I");

                switch (nodeType) {
                    case EQ: emit(IFEQ, trueLabel); break;
                    case NE: emit(IFNE, trueLabel); break;
                    case LT: emit(IFLT, trueLabel); break;
                    case LE: emit(IFLE, trueLabel); break;
                    case GT: emit(IFGT, trueLabel); break;
                    case GE: emit(IFGE, trueLabel); break;
                }

                localStack.decrease(2);
            }

            emit(ICONST_0); // false
            emit(GOTO, nextLabel);
            emitLabel(trueLabel);
            emit(ICONST_1); // true
            emitLabel(nextLabel);

            localStack.increase(1);
        }
    }
}
