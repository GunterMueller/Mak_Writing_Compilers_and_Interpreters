package wci.backend.interpreter.executors;

import java.util.Iterator;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

import wci.intermediate.*;
import wci.intermediate.symtabimpl.*;
import wci.intermediate.icodeimpl.*;
import wci.backend.interpreter.*;
import wci.message.*;

import static wci.intermediate.ICodeNodeType.*;
import static wci.intermediate.icodeimpl.ICodeKeyImpl.*;
import static wci.intermediate.typeimpl.TypeKeyImpl.*;
import static wci.intermediate.typeimpl.TypeFormImpl.*;
import static wci.backend.interpreter.RuntimeErrorCode.*;
import static wci.message.MessageType.*;

/**
 * <h1>StatementExecutor</h1>
 *
 * <p>Execute a statement.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class StatementExecutor extends Executor
{
    /**
     * Constructor.
     * @param the parent executor.
     */
    public StatementExecutor(Executor parent)
    {
        super(parent);
    }

    /**
     * Execute a statement.
     * To be overridden by the specialized statement executor subclasses.
     * @param node the root node of the statement.
     * @return null.
     */
    public Object execute(ICodeNode node)
    {
        ICodeNodeTypeImpl nodeType = (ICodeNodeTypeImpl) node.getType();

        // Send a message about the current source line.
        sendSourceLineMessage(node);

        switch (nodeType) {

            case COMPOUND: {
                CompoundExecutor compoundExecutor = new CompoundExecutor(this);
                return compoundExecutor.execute(node);
            }

            case ASSIGN: {
                AssignmentExecutor assignmentExecutor =
                    new AssignmentExecutor(this);
                return assignmentExecutor.execute(node);
            }

            case LOOP: {
                LoopExecutor loopExecutor = new LoopExecutor(this);
                return loopExecutor.execute(node);
            }

            case IF: {
                IfExecutor ifExecutor = new IfExecutor(this);
                return ifExecutor.execute(node);
            }

            case SELECT: {
                SelectExecutor selectExecutor = new SelectExecutor(this);
                return selectExecutor.execute(node);
            }

            case CALL: {
                CallExecutor callExecutor = new CallExecutor(this);
                return callExecutor.execute(node);
            }

            case NO_OP: return null;

            default: {
                errorHandler.flag(node, UNIMPLEMENTED_FEATURE, this);
                return null;
            }
        }
    }

    /**
     * Convert a Java string to a Pascal string or character.
     * @param targetType the target type specification.
     * @param javaValue the Java string.
     * @return the Pascal string or character.
     */
    protected Object toPascal(TypeSpec targetType, Object javaValue)
    {
        if (javaValue instanceof String) {
            String string = (String) javaValue;

            if (targetType == Predefined.charType) {
                return string.charAt(0);  // Pascal character
            }
            else if (targetType.isPascalString()) {
                Cell charCells[] = new Cell[string.length()];

                // Build an array of characters.
                for (int i = 0; i < string.length(); ++i) {
                    charCells[i] = MemoryFactory.createCell(string.charAt(i));
                }

                return charCells;  // Pascal string (array of characters)
            }
            else {
                return javaValue;
            }
        }
        else {
            return javaValue;
        }
    }

    /**
     * Convert a Pascal string to a Java string.
     * @param targetType the target type specification
     * @param pascalValue the Pascal string.
     * @return the Java string.
     */
    protected Object toJava(TypeSpec targetType, Object pascalValue)
    {
        if ( (pascalValue instanceof Cell[]) &&
             (((Cell[]) pascalValue)[0].getValue() instanceof Character) )
        {
            Cell charCells[] = (Cell[]) pascalValue;
            StringBuilder string = new StringBuilder(charCells.length);

            // Build a Java string.
            for (Cell ref : charCells) {
                string.append((Character) ref.getValue());
            }

            return string.toString();  // Java string
        }
        else {
            return pascalValue;
        }
    }

    /**
     * Return a copy of a Pascal value.
     * @param value the value.
     * @param node the statement node.
     * @return the copy.
     */
    protected Object copyOf(Object value, ICodeNode node)
    {
        Object copy = null;

        if (value instanceof Integer) {
            copy = new Integer((Integer) value);
        }
        else if (value instanceof Float) {
            copy = new Float((Float) value);
        }
        else if (value instanceof Character) {
            copy = new Character((Character) value);
        }
        else if (value instanceof Boolean) {
            copy = new Boolean((Boolean) value);
        }
        else if (value instanceof String) {
            copy = new String((String) value);
        }
        else if (value instanceof HashMap) {
            copy = copyRecord((HashMap<String, Object>) value, node);
        }
        else {
            copy = copyArray((Cell[]) value, node);
        }

        return copy;
    }

    /**
     * Return a copy of a Pascal record.
     * @param value the record value hashmap.
     * @param node the statement node.
     * @return the copy of the hashmap.
     */
    private Object copyRecord(HashMap<String, Object> value, ICodeNode node)
    {
        HashMap<String, Object> copy = new HashMap<String,Object>();

        if (value != null) {
            Set<Map.Entry<String, Object>> entries = value.entrySet();
            Iterator<Map.Entry<String, Object>> it = entries.iterator();

            while (it.hasNext()) {
                Map.Entry<String, Object> entry = it.next();
                String newKey = new String(entry.getKey());
                Cell valueCell = (Cell) entry.getValue();
                Object newValue = copyOf(valueCell.getValue(), node);

                copy.put(newKey, MemoryFactory.createCell(newValue));
            }
        }
        else {
            errorHandler.flag(node, UNINITIALIZED_VALUE, this);
        }

        return copy;
    }

    /**
     * Return a copy of a Pascal array.
     * @param valueCells the array cells.
     * @param node the statement node.
     * @return the copy of the array cells.
     */
    private Cell[] copyArray(Cell valueCells[], ICodeNode node)
    {
        int length;
        Cell copy[];

        if (valueCells != null) {
            length = valueCells.length;
            copy = new Cell[length];

            for (int i = 0; i < length; ++i) {
                Cell valueCell = (Cell) valueCells[i];
                Object newValue = copyOf(valueCell.getValue(), node);
                copy[i] = MemoryFactory.createCell(newValue);
            }
        }
        else {
            errorHandler.flag(node, UNINITIALIZED_VALUE, this);
            copy = new Cell[1];
        }

        return copy;
    }

    /**
     * Runtime range check.
     * @param node the root node of the expression subtree to check.
     * @param type the target type specification.
     * @param value the value.
     * @return the value to use.
     */
    protected Object checkRange(ICodeNode node, TypeSpec type, Object value)
    {
        if (type.getForm() == SUBRANGE) {
            int minValue = (Integer) type.getAttribute(SUBRANGE_MIN_VALUE);
            int maxValue = (Integer) type.getAttribute(SUBRANGE_MAX_VALUE);

            if (((Integer) value) < minValue) {
                errorHandler.flag(node, VALUE_RANGE, this);
                return minValue;
            }
            else if (((Integer) value) > maxValue) {
                errorHandler.flag(node, VALUE_RANGE, this);
                return maxValue;
            }
            else {
                return value;
            }
        }
        else {
            return value;
        }
    }

    /**
     * Send a message about the current source line.
     * @param node the statement node.
     */
    private void sendSourceLineMessage(ICodeNode node)
    {
        Object lineNumber = node.getAttribute(LINE);

        // Send the SOURCE_LINE message.
        if (lineNumber != null) {
            sendMessage(new Message(SOURCE_LINE, lineNumber));
        }
    }

    /**
     * Send a message about an assignment operation.
     * @param node the parse tree node.
     * @param variableName the name of the target variable.
     * @param value the value of the expression.
     */
    protected void sendAssignMessage(ICodeNode node, String variableName,
                                     Object value)
    {
        Object lineNumber = getLineNumber(node);

        // Send an ASSIGN message.
        if (lineNumber != null) {
            sendMessage(new Message(ASSIGN, new Object[] {lineNumber,
                                                          variableName,
                                                          value}));
        }
    }

    /**
     * Send a message about a value fetch operation.
     * @param node the parse tree node.
     * @param variableName the name of the variable.
     * @param value the value of the expression.
     */
    protected void sendFetchMessage(ICodeNode node, String variableName,
                                    Object value)
    {
        Object lineNumber = getLineNumber(node);

        // Send a FETCH message.
        if (lineNumber != null) {
            sendMessage(new Message(FETCH, new Object[] {lineNumber,
                                                         variableName,
                                                         value}));
        }
    }

    /**
     * Send a message about a call to a declared procedure or function.
     * @param node the parse tree node.
     * @param variableName the name of the variable.
     * @param value the value of the expression.
     */
    protected void sendCallMessage(ICodeNode node, String routineName)
    {
        Object lineNumber = getLineNumber(node);

        // Send a CALL message.
        if (lineNumber != null) {
            sendMessage(new Message(CALL, new Object[] {lineNumber,
                                                        routineName}));
        }
    }

    /**
     * Send a message about a return from a declared procedure or function.
     * @param node the parse tree node.
     * @param variableName the name of the variable.
     * @param value the value of the expression.
     */
    protected void sendReturnMessage(ICodeNode node, String routineName)
    {
        Object lineNumber = getLineNumber(node);

        // Send a RETURN message.
        if (lineNumber != null) {
            sendMessage(new Message(RETURN, new Object[] {lineNumber,
                                                          routineName}));
        }
    }

    /**
     * Get the source line number of a parse tree node.
     * @param node the parse tree node.
     * @return the line number.
     */
    private Object getLineNumber(ICodeNode node)
    {
        Object lineNumber = null;

        // Go up the parent links to look for a line number.
        while ((node != null) &&
               ((lineNumber = node.getAttribute(LINE)) == null)) {
            node = node.getParent();
        }

        return lineNumber;
    }
}
