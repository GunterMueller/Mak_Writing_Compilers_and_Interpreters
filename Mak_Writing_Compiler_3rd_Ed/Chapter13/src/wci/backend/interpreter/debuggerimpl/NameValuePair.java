package wci.backend.interpreter.debuggerimpl;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;

import wci.backend.interpreter.Cell;

/**
 * <h1>NameValuePair</h1>
 *
 * <p>Variable name and its value string pair used by the debugger.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class NameValuePair
{
    private String variableName;  // variable's name
    private String valueString;   // variable's value string

    /**
     * Constructor.
     * @param variableName the variable's name
     * @param value the variable's current value
     */
    protected NameValuePair(String variableName, Object value)
    {
        this.variableName = variableName;
        this.valueString = valueString(value);
    }

    /**
     * Getter.
     * @return the variable's name.
     */
    protected String getVariableName()
    {
        return variableName;
    }

    /**
     * Getter.
     * @return return the variable's value string.
     */
    protected String getValueString()
    {
        return valueString;
    }

    private static final int MAX_DISPLAYED_ELEMENTS = 10;

    /**
     * Convert a value into a value string.
     * @param value the value.
     * @return the value string.
     */
    protected static String valueString(Object value)
    {
        StringBuilder buffer = new StringBuilder();

        // Undefined value.
        if (value == null) {
            buffer.append("?");
        }

        // Dereference a VAR parameter.
        else if (value instanceof Cell) {
            buffer.append(valueString(((Cell) value).getValue()));
        }

        // Array value.
        else if (value instanceof Cell[]) {
            arrayValueString((Cell[]) value, buffer);
        }

        // Record value.
        else if (value instanceof HashMap) {
            recordValueString((HashMap) value, buffer);
        }

        // Character value.
        else if (value instanceof Character) {
            buffer.append("'").append((Character) value).append("'");
        }

        // Numeric or boolean value.
        else {
            buffer.append(value.toString());
        }

        return buffer.toString();
    }

    /**
     * Convert an array value into a value string.
     * @param array the array.
     * @param buffer the StringBuilder to use.
     */
    private static void arrayValueString(Cell array[], StringBuilder buffer)
    {
        int elementCount = 0;
        boolean first = true;
        buffer.append("[");

        // Loop over each array element up to MAX_DISPLAYED_ELEMENTS times.
        for (Cell cell : array) {
            if (first) {
                first = false;
            }
            else {
                buffer.append(", ");
            }

            if (++elementCount <= MAX_DISPLAYED_ELEMENTS) {
                buffer.append(valueString(cell.getValue()));
            }
            else {
                buffer.append("...");
                break;
            }
        }

        buffer.append("]");
    }

    /**
     * Convert a record value into a value string.
     * @param array the record.
     * @param buffer the StringBuilder to use.
     */
    private static void recordValueString(HashMap<String, Cell> record,
                                          StringBuilder buffer)
    {
        boolean first = true;
        buffer.append("{");

        Set<Map.Entry<String, Cell>> entries = record.entrySet();
        Iterator<Map.Entry<String, Cell>> it = entries.iterator();

        // Loop over each record field.
        while (it.hasNext()) {
            Map.Entry<String, Cell> entry = it.next();

            if (first) {
                first = false;
            }
            else {
                buffer.append(", ");
            }

            buffer.append(entry.getKey()).append(": ")
                  .append(valueString(entry.getValue().getValue()));
        }

        buffer.append("}");
    }
}
