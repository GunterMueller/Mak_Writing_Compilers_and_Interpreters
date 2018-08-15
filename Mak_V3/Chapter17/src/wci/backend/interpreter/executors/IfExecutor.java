package wci.backend.interpreter.executors;

import java.util.ArrayList;

import wci.intermediate.*;
import wci.backend.interpreter.*;

import static wci.intermediate.ICodeNodeType.*;
import static wci.backend.interpreter.RuntimeErrorCode.*;

/**
 * <h1>IfExecutor</h1>
 *
 * <p>Execute an IF statement.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class IfExecutor extends StatementExecutor
{
    /**
     * Constructor.
     * @param the parent executor.
     */
    public IfExecutor(Executor parent)
    {
        super(parent);
    }

    /**
     * Execute an IF statement.
     * @param node the root node of the statement.
     * @return null.
     */
    public Object execute(ICodeNode node)
    {
        // Get the IF node's children.
        ArrayList<ICodeNode> children = node.getChildren();
        ICodeNode exprNode = children.get(0);
        ICodeNode thenStmtNode = children.get(1);
        ICodeNode elseStmtNode = children.size() > 2 ? children.get(2) : null;

        ExpressionExecutor expressionExecutor = new ExpressionExecutor(this);
        StatementExecutor statementExecutor = new StatementExecutor(this);

        // Evaluate the expression to determine which statement to execute.
        boolean b = (Boolean) expressionExecutor.execute(exprNode);
        if (b) {
            statementExecutor.execute(thenStmtNode);
        }
        else if (elseStmtNode != null) {
            statementExecutor.execute(elseStmtNode);
        }

        ++executionCount;  // count the IF statement itself
        return null;
    }
}
