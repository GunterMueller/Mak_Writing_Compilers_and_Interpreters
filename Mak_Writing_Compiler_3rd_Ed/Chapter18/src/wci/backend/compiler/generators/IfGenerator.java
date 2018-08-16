package wci.backend.compiler.generators;

import java.util.ArrayList;

import wci.intermediate.*;
import wci.backend.compiler.*;

import static wci.backend.compiler.Instruction.*;

/**
 * <h1>IfGenerator</h1>
 *
 * <p>Generate code for an IF statement.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class IfGenerator extends StatementGenerator
{
    /**
     * Constructor.
     * @param the parent executor.
     */
    public IfGenerator(CodeGenerator parent)
    {
        super(parent);
    }

    /**
     * Generate code for an IF statement.
     * @param node the root node of the statement.
     */
    public void generate(ICodeNode node)
        throws PascalCompilerException
    {
        ArrayList<ICodeNode> children = node.getChildren();
        ICodeNode expressionNode = children.get(0);
        ICodeNode thenNode = children.get(1);
        ICodeNode elseNode = children.size() > 2 ? children.get(2) : null;
        ExpressionGenerator expressionGenerator = new ExpressionGenerator(this);
        StatementGenerator  statementGenerator  = new StatementGenerator(this);
        Label nextLabel = Label.newLabel();

        // Generate code for the boolean expression.
        expressionGenerator.generate(expressionNode);

        // Generate code for a THEN statement only.
        if (elseNode == null) {
            emit(IFEQ, nextLabel);
            localStack.decrease(1);

            statementGenerator.generate(thenNode);
        }

        // Generate code for a THEN statement and an ELSE statement.
        else {
            Label falseLabel = Label.newLabel();

            emit(IFEQ, falseLabel);
            localStack.decrease(1);

            statementGenerator.generate(thenNode);
            emit(GOTO, nextLabel);

            emitLabel(falseLabel);
            statementGenerator.generate(elseNode);
        }

        emitLabel(nextLabel);
    }
}
