package wci.backend.interpreter.executors;

import wci.intermediate.*;
import wci.intermediate.icodeimpl.*;
import wci.backend.interpreter.*;
import wci.message.*;

import static wci.intermediate.ICodeNodeType.*;
import static wci.intermediate.icodeimpl.ICodeKeyImpl.*;
import static wci.backend.interpreter.RuntimeErrorCode.*;
import static wci.message.MessageType.SOURCE_LINE;

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

            case NO_OP: return null;

            default: {
                errorHandler.flag(node, UNIMPLEMENTED_FEATURE, this);
                return null;
            }
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
}
