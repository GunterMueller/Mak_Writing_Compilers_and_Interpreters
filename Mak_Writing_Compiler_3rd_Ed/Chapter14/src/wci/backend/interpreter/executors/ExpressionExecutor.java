package wci.backend.interpreter.executors;

import java.util.ArrayList;
import java.util.EnumSet;
import java.util.HashMap;

import wci.intermediate.*;
import wci.intermediate.symtabimpl.*;
import wci.intermediate.icodeimpl.*;
import wci.backend.*;
import wci.backend.interpreter.*;

import static wci.intermediate.symtabimpl.SymTabKeyImpl.*;
import static wci.intermediate.symtabimpl.RoutineCodeImpl.*;
import static wci.intermediate.typeimpl.TypeKeyImpl.*;
import static wci.intermediate.typeimpl.TypeFormImpl.*;
import static wci.intermediate.icodeimpl.ICodeNodeTypeImpl.*;
import static wci.intermediate.icodeimpl.ICodeKeyImpl.*;
import static wci.backend.interpreter.RuntimeErrorCode.*;

/**
 * <h1>ExpressionExecutor</h1>
 *
 * <p>Execute an expression.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class ExpressionExecutor extends StatementExecutor
{
    /**
     * Constructor.
     * @param the parent executor.
     */
    public ExpressionExecutor(Executor parent)
    {
        super(parent);
    }

    /**
     * Execute an expression.
     * @param node the root intermediate code node of the compound statement.
     * @return the computed value of the expression.
     */
    public Object execute(ICodeNode node)
    {
        ICodeNodeTypeImpl nodeType = (ICodeNodeTypeImpl) node.getType();

        switch (nodeType) {

            case VARIABLE: {

                // Return the variable's value.
                return executeValue(node);
            }

            case INTEGER_CONSTANT: {
                TypeSpec type = node.getTypeSpec();
                Integer value = (Integer) node.getAttribute(VALUE);

                // If boolean, return true or false.
                // Else return the integer value.
                return type == Predefined.booleanType
                           ? value == 1  // true or false
                           : value;      // integer value
            }

            case REAL_CONSTANT: {

                // Return the float value.
                return (Float) node.getAttribute(VALUE);
            }

            case STRING_CONSTANT: {

                // Return the string value.
                return (String) node.getAttribute(VALUE);
            }

            case NEGATE: {

                // Get the NEGATE node's expression node child.
                ArrayList<ICodeNode> children = node.getChildren();
                ICodeNode expressionNode = children.get(0);

                // Execute the expression and return the negative of its value.
                Object value = execute(expressionNode);
                if (value instanceof Integer) {
                    return -((Integer) value);
                }
                else {
                    return -((Float) value);
                }
            }

            case NOT: {

                // Get the NOT node's expression node child.
                ArrayList<ICodeNode> children = node.getChildren();
                ICodeNode expressionNode = children.get(0);

                // Execute the expression and return the "not" of its value.
                boolean value = (Boolean) execute(expressionNode);
                return !value;
            }

            case CALL: {

                // Execute a function call.
                SymTabEntry functionId = (SymTabEntry) node.getAttribute(ID);
                RoutineCode routineCode =
                    (RoutineCode) functionId.getAttribute(ROUTINE_CODE);
                CallExecutor callExecutor = new CallExecutor(this);
                Object value = callExecutor.execute(node);

                // If it was a declared function, obtain the function value
                // from its name.
                if (routineCode == DECLARED) {
                    String functionName = functionId.getName();
                    int nestingLevel = functionId.getSymTab().getNestingLevel();
                    ActivationRecord ar = runtimeStack.getTopmost(nestingLevel);
                    Cell functionValueCell = ar.getCell(functionName);
                    value = functionValueCell.getValue();

                    sendFetchMessage(node, functionId.getName(), value);
                }

                // Return the function value.
                return value;
            }

            // Must be a binary operator.
            default: return executeBinaryOperator(node, nodeType);
        }
    }

    /**
     * Return a variable's value.
     * @param node ICodeNode
     * @return Object
     */
    private Object executeValue(ICodeNode node)
    {
        SymTabEntry variableId = (SymTabEntry) node.getAttribute(ID);
        String variableName = variableId.getName();
        TypeSpec variableType = variableId.getTypeSpec();

        // Get the variable's value.
        Cell variableCell = executeVariable(node);
        Object value = variableCell.getValue();

        if (value != null) {
            value = toJava(variableType, value);
        }

        // Uninitialized value error: Use a default value.
        else {
            errorHandler.flag(node, UNINITIALIZED_VALUE, this);

            value = BackendFactory.defaultValue(variableType);
            variableCell.setValue(value);
        }

        sendFetchMessage(node, variableName, value);
        return value;
    }

    /**
     * Execute a variable and return the reference to its cell.
     * @param node the variable node.
     * @return the reference to the variable's cell.
     */
    public Cell executeVariable(ICodeNode node)
    {
        SymTabEntry variableId = (SymTabEntry) node.getAttribute(ID);
        String variableName = variableId.getName();
        TypeSpec variableType = variableId.getTypeSpec();
        int nestingLevel = variableId.getSymTab().getNestingLevel();

        // Get the variable reference from the appropriate activation record.
        ActivationRecord ar = runtimeStack.getTopmost(nestingLevel);
        Cell variableCell = ar.getCell(variableName);

        ArrayList<ICodeNode> modifiers = node.getChildren();

        // Reference to a reference: Use the original reference.
        if (variableCell.getValue() instanceof Cell) {
            variableCell = (Cell) variableCell.getValue();
        }

        // Execute any array subscripts or record fields.
        for (ICodeNode modifier : modifiers) {
            ICodeNodeType nodeType = modifier.getType();

            // Subscripts.
            if (nodeType == SUBSCRIPTS) {
                ArrayList<ICodeNode> subscripts = modifier.getChildren();

                // Compute a new reference for each subscript.
                for (ICodeNode subscript : subscripts) {
                    TypeSpec indexType =
                        (TypeSpec) variableType.getAttribute(ARRAY_INDEX_TYPE);
                    int minIndex = indexType.getForm() == SUBRANGE
                        ? (Integer) indexType.getAttribute(SUBRANGE_MIN_VALUE)
                        : 0;

                    int value = (Integer) execute(subscript);
                    value = (Integer) checkRange(node, indexType, value);

                    int index = value - minIndex;
                    variableCell = ((Cell[]) variableCell.getValue())[index];
                    variableType = (TypeSpec)
                        variableType.getAttribute(ARRAY_ELEMENT_TYPE);
                }
            }

            // Field.
            else if (nodeType == FIELD) {
                SymTabEntry fieldId = (SymTabEntry) modifier.getAttribute(ID);
                String fieldName = fieldId.getName();

                // Compute a new reference for the field.
                HashMap<String, Cell> map =
                    (HashMap<String, Cell>) variableCell.getValue();
                variableCell = map.get(fieldName);
                variableType = fieldId.getTypeSpec();
            }
        }

        return variableCell;
    }

    // Set of arithmetic operator node types.
    private static final EnumSet<ICodeNodeTypeImpl> ARITH_OPS =
        EnumSet.of(ADD, SUBTRACT, MULTIPLY, FLOAT_DIVIDE, INTEGER_DIVIDE, MOD);

    /**
     * Execute a binary operator.
     * @param node the root node of the expression.
     * @param nodeType the node type.
     * @return the computed value of the expression.
     */
    private Object executeBinaryOperator(ICodeNode node,
                                         ICodeNodeTypeImpl nodeType)
    {
        // Get the two operand children of the operator node.
        ArrayList<ICodeNode> children = node.getChildren();
        ICodeNode operandNode1 = children.get(0);
        ICodeNode operandNode2 = children.get(1);

        // Operands.
        Object operand1 = execute(operandNode1);
        Object operand2 = execute(operandNode2);

        boolean integerMode = false;
        boolean characterMode = false;
        boolean stringMode = false;

        if ((operand1 instanceof Integer) && (operand2 instanceof Integer)) {
            integerMode = true;
        }
        else if ( ( (operand1 instanceof Character) ||
                    ( (operand1 instanceof String) &&
                      (((String) operand1).length() == 1) )
                  ) &&
                  ( (operand2 instanceof Character) ||
                    ( (operand2 instanceof String) &&
                      (((String) operand2).length() == 1) )
                  )
                ) {
            characterMode = true;
        }
        else if ((operand1 instanceof String) && (operand2 instanceof String)) {
            stringMode = true;
        }

        // ====================
        // Arithmetic operators
        // ====================

        if (ARITH_OPS.contains(nodeType)) {
            if (integerMode) {
                int value1 = (Integer) operand1;
                int value2 = (Integer) operand2;

                // Integer operations.
                switch (nodeType) {
                    case ADD:      return value1 + value2;
                    case SUBTRACT: return value1 - value2;
                    case MULTIPLY: return value1 * value2;

                    case FLOAT_DIVIDE: {

                        // Check for division by zero.
                        if (value2 != 0) {
                            return ((float) value1)/((float) value2);
                        }
                        else {
                            errorHandler.flag(node, DIVISION_BY_ZERO, this);
                            return 0;
                        }
                    }

                    case INTEGER_DIVIDE: {

                        // Check for division by zero.
                        if (value2 != 0) {
                            return value1/value2;
                        }
                        else {
                            errorHandler.flag(node, DIVISION_BY_ZERO, this);
                            return 0;
                        }
                    }

                    case MOD:  {

                        // Check for division by zero.
                        if (value2 != 0) {
                            return value1%value2;
                        }
                        else {
                            errorHandler.flag(node, DIVISION_BY_ZERO, this);
                            return 0;
                        }
                    }
                }
            }
            else {
                float value1 = operand1 instanceof Integer
                                   ? (Integer) operand1 : (Float) operand1;
                float value2 = operand2 instanceof Integer
                                   ? (Integer) operand2 : (Float) operand2;

                // Float operations.
                switch (nodeType) {
                    case ADD:      return value1 + value2;
                    case SUBTRACT: return value1 - value2;
                    case MULTIPLY: return value1 * value2;

                    case FLOAT_DIVIDE: {

                        // Check for division by zero.
                        if (value2 != 0.0f) {
                            return value1/value2;
                        }
                        else {
                            errorHandler.flag(node, DIVISION_BY_ZERO, this);
                            return 0.0f;
                        }
                    }
                }
            }
        }

        // ==========
        // AND and OR
        // ==========

        else if ((nodeType == AND) || (nodeType == OR)) {
            boolean value1 = (Boolean) operand1;
            boolean value2 = (Boolean) operand2;

            switch (nodeType) {
                case AND: return value1 && value2;
                case OR:  return value1 || value2;
            }
        }

        // ====================
        // Relational operators
        // ====================

        else if (integerMode) {
            int value1 = (Integer) operand1;
            int value2 = (Integer) operand2;

            // Integer operands.
            switch (nodeType) {
                case EQ: return value1 == value2;
                case NE: return value1 != value2;
                case LT: return value1 <  value2;
                case LE: return value1 <= value2;
                case GT: return value1 >  value2;
                case GE: return value1 >= value2;
            }
        }
        else if (characterMode) {
            int value1 = operand1 instanceof Character
                             ? (Character) operand1
                             : ((String) operand1).charAt(0);
            int value2 = operand2 instanceof Character
                             ? (Character) operand2
                             : ((String) operand2).charAt(0);

            // Character operands.
            switch (nodeType) {
                case EQ: return value1 == value2;
                case NE: return value1 != value2;
                case LT: return value1 <  value2;
                case LE: return value1 <= value2;
                case GT: return value1 >  value2;
                case GE: return value1 >= value2;
            }
        }
        else if (stringMode) {
            String value1 = (String) operand1;
            String value2 = (String) operand2;

            // String operands.
            int comp = value1.compareTo(value2);
            switch (nodeType) {
                case EQ: return comp == 0;
                case NE: return comp != 0;
                case LT: return comp <  0;
                case LE: return comp <= 0;
                case GT: return comp >  0;
                case GE: return comp >= 0;
            }
        }
        else {
            float value1 = operand1 instanceof Integer
                               ? (Integer) operand1 : (Float) operand1;
            float value2 = operand2 instanceof Integer
                               ? (Integer) operand2 : (Float) operand2;

            // Float operands.
            switch (nodeType) {
                case EQ: return value1 == value2;
                case NE: return value1 != value2;
                case LT: return value1 <  value2;
                case LE: return value1 <= value2;
                case GT: return value1 >  value2;
                case GE: return value1 >= value2;
            }
        }

        return 0;  // should never get here
    }
}
