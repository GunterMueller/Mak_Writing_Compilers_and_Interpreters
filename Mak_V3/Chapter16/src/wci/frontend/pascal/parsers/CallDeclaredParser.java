package wci.frontend.pascal.parsers;

import wci.frontend.*;
import wci.frontend.pascal.*;
import wci.intermediate.*;

import static wci.frontend.pascal.PascalTokenType.*;
import static wci.intermediate.icodeimpl.ICodeNodeTypeImpl.*;
import static wci.intermediate.icodeimpl.ICodeKeyImpl.*;

/**
 * <h1>DeclaredCallParser</h1>
 *
 * <p>Parse a called to a declared procedure or function.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class CallDeclaredParser extends CallParser
{
    /**
     * Constructor.
     * @param parent the parent parser.
     */
    public CallDeclaredParser(PascalParserTD parent)
    {
        super(parent);
    }

    /**
     * Parse a call to a declared procedure or function.
     * @param token the initial token.
     * @return the root node of the generated parse tree.
     * @throws Exception if an error occurred.
     */
    public ICodeNode parse(Token token)
        throws Exception
    {
        // Create the CALL node.
        ICodeNode callNode = ICodeFactory.createICodeNode(CALL);
        SymTabEntry pfId = symTabStack.lookup(token.getText().toLowerCase());
        callNode.setAttribute(ID, pfId);
        callNode.setTypeSpec(pfId.getTypeSpec());

        token = nextToken();  // consume procedure or function identifier

        ICodeNode parmsNode = parseActualParameters(token, pfId,
                                                    true, false, false);

        callNode.addChild(parmsNode);
        return callNode;
    }
}
