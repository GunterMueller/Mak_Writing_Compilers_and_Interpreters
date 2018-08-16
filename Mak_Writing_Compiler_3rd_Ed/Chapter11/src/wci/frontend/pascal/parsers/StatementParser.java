package wci.frontend.pascal.parsers;

import java.util.EnumSet;

import wci.frontend.*;
import wci.frontend.pascal.*;
import wci.intermediate.*;
import wci.intermediate.symtabimpl.*;

import static wci.frontend.pascal.PascalTokenType.*;
import static wci.frontend.pascal.PascalErrorCode.*;
import static wci.intermediate.symtabimpl.DefinitionImpl.*;
import static wci.intermediate.icodeimpl.ICodeNodeTypeImpl.*;
import static wci.intermediate.icodeimpl.ICodeKeyImpl.*;

/**
 * <h1>StatementParser</h1>
 *
 * <p>Parse a Pascal statement.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class StatementParser extends PascalParserTD
{
    /**
     * Constructor.
     * @param parent the parent parser.
     */
    public StatementParser(PascalParserTD parent)
    {
        super(parent);
    }

    // Synchronization set for starting a statement.
    protected static final EnumSet<PascalTokenType> STMT_START_SET =
        EnumSet.of(BEGIN, CASE, FOR, PascalTokenType.IF, REPEAT, WHILE,
                   IDENTIFIER, SEMICOLON);

    // Synchronization set for following a statement.
    protected static final EnumSet<PascalTokenType> STMT_FOLLOW_SET =
        EnumSet.of(SEMICOLON, END, ELSE, UNTIL, DOT);

    /**
     * Parse a statement.
     * To be overridden by the specialized statement parser subclasses.
     * @param token the initial token.
     * @return the root node of the generated parse tree.
     * @throws Exception if an error occurred.
     */
    public ICodeNode parse(Token token)
        throws Exception
    {
        ICodeNode statementNode = null;

        switch ((PascalTokenType) token.getType()) {

            case BEGIN: {
                CompoundStatementParser compoundParser =
                    new CompoundStatementParser(this);
                statementNode = compoundParser.parse(token);
                break;
            }

            case IDENTIFIER: {
                String name = token.getText().toLowerCase();
                SymTabEntry id = symTabStack.lookup(name);
                Definition idDefn = id != null ? id.getDefinition()
                                               : UNDEFINED;

                // Assignment statement or procedure call.
                switch ((DefinitionImpl) idDefn) {

                    case VARIABLE:
                    case VALUE_PARM:
                    case VAR_PARM:
                    case UNDEFINED: {
                        AssignmentStatementParser assignmentParser =
                            new AssignmentStatementParser(this);
                        statementNode = assignmentParser.parse(token);
                        break;
                    }

                    case FUNCTION: {
                        AssignmentStatementParser assignmentParser =
                            new AssignmentStatementParser(this);
                        statementNode =
                            assignmentParser.parseFunctionNameAssignment(token);
                        break;
                    }

                    case PROCEDURE: {
                        CallParser callParser = new CallParser(this);
                        statementNode = callParser.parse(token);
                        break;
                    }

                    default: {
                        errorHandler.flag(token, UNEXPECTED_TOKEN, this);
                        token = nextToken();  // consume identifier
                    }
                }

                break;
            }

            case REPEAT: {
                RepeatStatementParser repeatParser =
                    new RepeatStatementParser(this);
                statementNode = repeatParser.parse(token);
                break;
            }

            case WHILE: {
                WhileStatementParser whileParser =
                    new WhileStatementParser(this);
                statementNode = whileParser.parse(token);
                break;
            }

            case FOR: {
                ForStatementParser forParser = new ForStatementParser(this);
                statementNode = forParser.parse(token);
                break;
            }

            case IF: {
                IfStatementParser ifParser = new IfStatementParser(this);
                statementNode = ifParser.parse(token);
                break;
            }

            case CASE: {
                CaseStatementParser caseParser = new CaseStatementParser(this);
                statementNode = caseParser.parse(token);
                break;
            }

            default: {
                statementNode = ICodeFactory.createICodeNode(NO_OP);
                break;
            }
        }

        // Set the current line number as an attribute.
        setLineNumber(statementNode, token);

        return statementNode;
    }

    /**
     * Set the current line number as a statement node attribute.
     * @param node ICodeNode
     * @param token Token
     */
    protected void setLineNumber(ICodeNode node, Token token)
    {
        if (node != null) {
            node.setAttribute(LINE, token.getLineNumber());
        }
    }

    /**
     * Parse a statement list.
     * @param token the curent token.
     * @param parentNode the parent node of the statement list.
     * @param terminator the token type of the node that terminates the list.
     * @param errorCode the error code if the terminator token is missing.
     * @throws Exception if an error occurred.
     */
    protected void parseList(Token token, ICodeNode parentNode,
                             PascalTokenType terminator,
                             PascalErrorCode errorCode)
        throws Exception
    {
        // Synchronization set for the terminator.
        EnumSet<PascalTokenType> terminatorSet = STMT_START_SET.clone();
        terminatorSet.add(terminator);

        // Loop to parse each statement until the END token
        // or the end of the source file.
        while (!(token instanceof EofToken) &&
               (token.getType() != terminator)) {

            // Parse a statement.  The parent node adopts the statement node.
            ICodeNode statementNode = parse(token);
            parentNode.addChild(statementNode);

            token = currentToken();
            TokenType tokenType = token.getType();

            // Look for the semicolon between statements.
            if (tokenType == SEMICOLON) {
                token = nextToken();  // consume the ;
            }

            // If at the start of the next statement, then missing a semicolon.
            else if (STMT_START_SET.contains(tokenType)) {
                errorHandler.flag(token, MISSING_SEMICOLON, this);
            }

            // Synchronize at the start of the next statement
            // or at the terminator.
            token = synchronize(terminatorSet);
        }

        // Look for the terminator token.
        if (token.getType() == terminator) {
            token = nextToken();  // consume the terminator token
        }
        else {
            errorHandler.flag(token, errorCode, this);
        }
    }
}
