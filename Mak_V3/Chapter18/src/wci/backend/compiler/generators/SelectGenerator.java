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
 * <h1>SelectGenerator</h1>
 *
 * <p>Generate code for a select statement.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class SelectGenerator extends StatementGenerator
{
    /**
     * Constructor.
     * @param the parent executor.
     */
    public SelectGenerator(CodeGenerator parent)
    {
        super(parent);
    }

    /**
     * A value-label pair for the LOOKUPSWITCH: A SELECT branch value
     *                                          and its statement label.
     */
    private class ValueLabelPair
    {
        private int value;
        private Label label;

        /**
         * Constructor.
         * @param value the SELECT branch value.
         * @param label the corresponding statement label.
         */
        private ValueLabelPair(int value, Label label)
        {
            this.value = value;
            this.label = label;
        }
    }

    /**
     * Generate code for a SELECT statement.
     * @param node the root node of the statement.
     */
    public void generate(ICodeNode node)
        throws PascalCompilerException
    {
        ArrayList<ICodeNode> selectChildren = node.getChildren();
        ICodeNode exprNode = selectChildren.get(0);
        ArrayList<Label> branchLabels = new ArrayList<Label>();

        branchLabels.add(Label.newLabel());  // "next" label

        // Generate code to evaluate the SELECT expression.
        ExpressionGenerator exprGenerator = new ExpressionGenerator(this);
        exprGenerator.generate(exprNode);

        // Process the select branches.
        ArrayList<ValueLabelPair> pairs = processSelectBranches(selectChildren,
                                                                branchLabels);

        // Generate code for the LOOKUPSWITCH and the branch statements.
        generateLookupSwitch(pairs, branchLabels);
        generateBranchStatements(selectChildren, branchLabels);
    }

    /**
     * Process the SELECT branches.
     * @param selectChildren the child nodes of the SELECT node.
     * @param branchLabels the branch labels.
     * @return the array list of table entries.
     */
    private ArrayList<ValueLabelPair> processSelectBranches
                                          (ArrayList<ICodeNode> selectChildren,
                                           ArrayList<Label> branchLabels)
    {
        ArrayList<ValueLabelPair> pairs = new ArrayList<ValueLabelPair>();

        // Loop over the SELECT branches.
        for (int i = 1; i < selectChildren.size(); ++i) {
            ICodeNode branchNode = selectChildren.get(i);
            ICodeNode constantsNode = branchNode.getChildren().get(0);
            Label branchLabel = Label.newLabel();

            branchLabels.add(branchLabel);

            // Loop over the constants children of the branch's
            // SELECT_CONSTANTS node and create the label-value pairs.
            ArrayList<ICodeNode> constantsList = constantsNode.getChildren();
            for (ICodeNode constantNode : constantsList) {
                Object value = constantNode.getAttribute(VALUE);
                int intValue = constantNode.getType() == STRING_CONSTANT
                                   ? (int) (((String) value).charAt(0))
                                   : ((Integer) value).intValue();

                pairs.add(new ValueLabelPair(intValue, branchLabel));
            }
        }

        // Sort and return the list of value-label pairs.
        sortPairs(pairs);
        return pairs;
    }

    /**
     * Sort the label-value pairs into ascending order by value.
     * @param pairs the list of pairs.
     */
    private void sortPairs(ArrayList<ValueLabelPair> pairs)
    {
        // Simple insertion sort on the SELECT branch values.
        for (int i = 0; i < pairs.size()-1; ++i) {
            for (int j = i+1; j < pairs.size(); ++j) {
                if (pairs.get(i).value > pairs.get(j).value) {
                    ValueLabelPair temp = pairs.get(i);
                    pairs.set(i, pairs.get(j));
                    pairs.set(j, temp);
                }
            }
        }
    }

    /**
     * Generate code for the LOOKUPSWITCH instruction.
     * @param entries the table entries.
     * @param branchLabels the branch labels.
     */
    private void generateLookupSwitch(ArrayList<ValueLabelPair> entries,
                                      ArrayList<Label> branchLabels)
    {
        emitBlankLine();
        emit(LOOKUPSWITCH);

        // For each entry, emit the value and its label.
        for (ValueLabelPair entry : entries) {
            emitLabel(entry.value, entry.label);
        }

        // Emit the default label;
        emitLabel("default", branchLabels.get(0));

        localStack.decrease(1);
    }

    /**
     * Generate code for the branch statements.
     * @param selectChildren the child nodes of the SELECT node.
     * @param branchLabels the branch labels
     * @throws PascalCompilerException if an error occurred.
     */
    private void generateBranchStatements(ArrayList<ICodeNode> selectChildren,
                                          ArrayList<Label> branchLabels)
        throws PascalCompilerException
    {
        StatementGenerator stmtGenerator = new StatementGenerator(this);
        emitBlankLine();

        // Loop to emit each branch label and then generate code for
        // the corresponding branch statement.
        for (int i = 1; i < selectChildren.size(); ++i) {
            ICodeNode branchNode = selectChildren.get(i);
            ICodeNode statementNode = branchNode.getChildren().get(1);

            emitLabel(branchLabels.get(i));
            stmtGenerator.generate(statementNode);

            // Branch to the "next" label.
            emit(GOTO, branchLabels.get(0));
        }

        // Emit the "next" label.
        emitLabel(branchLabels.get(0));

        emitBlankLine();
    }
}
