package wci.backend.interpreter.executors;

import java.util.ArrayList;

import wci.intermediate.*;
import wci.backend.interpreter.*;

import static wci.intermediate.symtabimpl.DefinitionImpl.*;
import static wci.intermediate.symtabimpl.SymTabKeyImpl.*;
import static wci.intermediate.icodeimpl.ICodeKeyImpl.*;

/**
 * <h1>CallDeclaredExecutor</h1>
 *
 * <p>Execute a call a declared procedure or function.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class CallDeclaredExecutor extends CallExecutor
{
    /**
     * Constructor.
     * @param the parent executor.
     */
    public CallDeclaredExecutor(Executor parent)
    {
        super(parent);
    }

    /**
     * Execute a call to a declared procedure or function.
     * @param node the CALL node.
     * @return null.
     */
    public Object execute(ICodeNode node)
    {
        SymTabEntry routineId = (SymTabEntry) node.getAttribute(ID);
        ActivationRecord newAr =
            MemoryFactory.createActivationRecord(routineId);

        // Execute any actual parameters and initialize
        // the formal parameters in the new activation record.
        if (node.getChildren().size() > 0) {
            ICodeNode parmsNode = node.getChildren().get(0);
            ArrayList<ICodeNode> actualNodes = parmsNode.getChildren();
            ArrayList<SymTabEntry> formalIds =
                (ArrayList<SymTabEntry>) routineId.getAttribute(ROUTINE_PARMS);
            executeActualParms(actualNodes, formalIds, newAr);
        }

        // Push the new activation record.
        runtimeStack.push(newAr);

        sendCallMessage(node, routineId.getName());

        // Get the root node of the routine's intermediate code.
        ICode iCode = (ICode) routineId.getAttribute(ROUTINE_ICODE);
        ICodeNode rootNode = iCode.getRoot();

        // Execute the routine.
        StatementExecutor statementExecutor = new StatementExecutor(this);
        Object value = statementExecutor.execute(rootNode);

        // Pop off the activation record.
        runtimeStack.pop();

        sendReturnMessage(node, routineId.getName());
        return value;
    }

    /**
     * Execute the actual parameters of a call.
     * @param actualNodes the list of nodes of the actual parms.
     * @param formalIds the list of symbol table entries of the formal parms.
     * @param newAr the new activation record.
     */
    private void executeActualParms(ArrayList<ICodeNode> actualNodes,
                                    ArrayList<SymTabEntry> formalIds,
                                    ActivationRecord newAr)
    {
        ExpressionExecutor expressionExecutor = new ExpressionExecutor(this);
        AssignmentExecutor assignmentExecutor = new AssignmentExecutor(this);

        for (int i = 0; i < formalIds.size(); ++i) {
            SymTabEntry formalId = formalIds.get(i);
            Definition formalDefn = formalId.getDefinition();
            Cell formalCell = newAr.getCell(formalId.getName());
            ICodeNode actualNode = actualNodes.get(i);

            // Value parameter.
            if (formalDefn == VALUE_PARM) {
                TypeSpec formalType = formalId.getTypeSpec();
                TypeSpec valueType  = actualNode.getTypeSpec().baseType();
                Object value = expressionExecutor.execute(actualNode);

                assignmentExecutor.assignValue(actualNode, formalId,
                                               formalCell, formalType,
                                               value, valueType);
            }

            // VAR parameter.
            else {
                Cell actualCell =
                    (Cell) expressionExecutor.executeVariable(actualNode);
                formalCell.setValue(actualCell);
            }
        }
    }
}
