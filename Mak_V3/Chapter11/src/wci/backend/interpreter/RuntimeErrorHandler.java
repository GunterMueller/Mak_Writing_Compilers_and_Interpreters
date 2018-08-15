package wci.backend.interpreter;

import wci.intermediate.*;
import wci.backend.Backend;
import wci.message.*;

import static wci.intermediate.icodeimpl.ICodeKeyImpl.*;
import static wci.message.MessageType.RUNTIME_ERROR;

/**
 * <h1>RuntimeErrorHandler</h1>
 *
 * <p>Runtime error handler for the backend interpreter.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class RuntimeErrorHandler
{
    private static final int MAX_ERRORS = 5;

    private static int errorCount = 0;    // count of runtime errors

    /**
     * Getter
     * @return the count of runtime errors.
     */
    public static int getErrorCount()
    {
        return errorCount;
    }

    /**
     * Flag a runtime error.
     * @param node the root node of the offending statement or expression.
     * @param errorCode the runtime error code.
     * @param backend the backend processor.
     */
    public void flag(ICodeNode node, RuntimeErrorCode errorCode,
                     Backend backend)
    {
        String lineNumber = null;

        // Look for the ancestor statement node with a line number attribute.
        while ((node != null) && (node.getAttribute(LINE) == null)) {
            node = node.getParent();
        }

        // Notify the interpreter's listeners.
        backend.sendMessage(
            new Message(RUNTIME_ERROR,
                        new Object[] {errorCode.toString(),
                                      (Integer) node.getAttribute(LINE)}));

        if (++errorCount > MAX_ERRORS) {
            System.out.println("*** ABORTED AFTER TOO MANY RUNTIME ERRORS.");
            System.exit(-1);
        }
    }
}
