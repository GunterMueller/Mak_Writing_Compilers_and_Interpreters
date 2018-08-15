package wci.backend.interpreter.executors;

import java.util.ArrayList;
import java.util.HashMap;

import wci.intermediate.*;
import wci.backend.interpreter.*;

import static wci.intermediate.icodeimpl.ICodeNodeTypeImpl.*;
import static wci.intermediate.icodeimpl.ICodeKeyImpl.*;
import static wci.backend.interpreter.RuntimeErrorCode.*;

/**
 * <h1>SelectExecutor</h1>
 *
 * <p>Execute a SELECT statement. Optimized.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class SelectExecutor extends StatementExecutor
{
    /**
     * Constructor.
     * @param the parent executor.
     */
    public SelectExecutor(Executor parent)
    {
        super(parent);
    }

    // Jump table cache: entry key is a SELECT node,
    //                   entry value is the jump table.
    // Jump table: entry key is a selection value,
    //             entry value is the branch statement.
    private static HashMap<ICodeNode, HashMap<Object, ICodeNode>> jumpCache =
        new HashMap<ICodeNode, HashMap<Object, ICodeNode>>();

    /**
     * Execute SELECT statement.
     * @param node the root node of the statement.
     * @return null.
     */
    public Object execute(ICodeNode node)
    {
        // Is there already an entry for this SELECT node in the
        // jump table cache? If not, create a jump table entry.
        HashMap<Object, ICodeNode> jumpTable = jumpCache.get(node);
        if (jumpTable == null) {
            jumpTable = createJumpTable(node);
            jumpCache.put(node, jumpTable);
        }

        // Get the SELECT node's children.
        ArrayList<ICodeNode> selectChildren = node.getChildren();
        ICodeNode exprNode = selectChildren.get(0);

        // Evaluate the SELECT expression.
        ExpressionExecutor expressionExecutor = new ExpressionExecutor(this);
        Object selectValue = expressionExecutor.execute(exprNode);

        // If there is a selection, execute the SELECT_BRANCH's statement.
        ICodeNode statementNode = jumpTable.get(selectValue);
        if (statementNode != null) {
            StatementExecutor statementExecutor = new StatementExecutor(this);
            statementExecutor.execute(statementNode);
        }

        ++executionCount;  // count the SELECT statement itself
        return null;
    }

    /**
     * Create a jump table for a SELECT node.
     * @param node the SELECT node.
     * @return the jump table.
     */
    private HashMap<Object, ICodeNode> createJumpTable(ICodeNode node)
    {
        HashMap<Object, ICodeNode> jumpTable = new HashMap<Object, ICodeNode>();

        // Loop over children that are SELECT_BRANCH nodes.
        ArrayList<ICodeNode> selectChildren = node.getChildren();
        for (int i = 1; i < selectChildren.size(); ++i) {
            ICodeNode branchNode = selectChildren.get(i);
            ICodeNode constantsNode = branchNode.getChildren().get(0);
            ICodeNode statementNode = branchNode.getChildren().get(1);

            // Loop over the constants children of the branch's CONSTANTS_NODE.
            ArrayList<ICodeNode> constantsList = constantsNode.getChildren();
            for (ICodeNode constantNode : constantsList) {

                // Create a jump table entry.
                Object value = constantNode.getAttribute(VALUE);
                jumpTable.put(value, statementNode);
            }
        }

        return jumpTable;
    }
}
