package wci.frontend.pascal.parsers;

import java.util.EnumSet;
import java.util.HashSet;

import wci.frontend.*;
import wci.frontend.pascal.*;
import wci.intermediate.*;
import wci.intermediate.symtabimpl.*;
import wci.intermediate.typeimpl.*;

import static wci.frontend.pascal.PascalTokenType.*;
import static wci.frontend.pascal.PascalErrorCode.*;
import static wci.intermediate.symtabimpl.SymTabKeyImpl.*;
import static wci.intermediate.symtabimpl.DefinitionImpl.*;
import static wci.intermediate.icodeimpl.ICodeNodeTypeImpl.*;
import static wci.intermediate.icodeimpl.ICodeKeyImpl.*;
import static wci.intermediate.typeimpl.TypeFormImpl.ENUMERATION;

/**
 * <h1>CaseStatementParser</h1>
 *
 * <p>Parse a Pascal CASE statement.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class CaseStatementParser extends StatementParser
{
    /**
     * Constructor.
     * @param parent the parent parser.
     */
    public CaseStatementParser(PascalParserTD parent)
    {
        super(parent);
    }

    // Synchronization set for starting a CASE option constant.
    private static final EnumSet<PascalTokenType> CONSTANT_START_SET =
        EnumSet.of(IDENTIFIER, INTEGER, PLUS, MINUS, STRING);

    // Synchronization set for OF.
    private static final EnumSet<PascalTokenType> OF_SET =
        CONSTANT_START_SET.clone();
    static {
        OF_SET.add(OF);
        OF_SET.addAll(StatementParser.STMT_FOLLOW_SET);
    }

    /**
     * Parse a CASE statement.
     * @param token the initial token.
     * @return the root node of the generated parse tree.
     * @throws Exception if an error occurred.
     */
    public ICodeNode parse(Token token)
        throws Exception
    {
        token = nextToken();  // consume the CASE

        // Create a SELECT node.
        ICodeNode selectNode = ICodeFactory.createICodeNode(SELECT);

        // Parse the CASE expression.
        // The SELECT node adopts the expression subtree as its first child.
        ExpressionParser expressionParser = new ExpressionParser(this);
        ICodeNode exprNode = expressionParser.parse(token);
        selectNode.addChild(exprNode);

        // Type check: The CASE expression's type must be integer, character,
        //             or enumeration.
        TypeSpec exprType = exprNode != null ? exprNode.getTypeSpec()
                                             : Predefined.undefinedType;
        if (!TypeChecker.isInteger(exprType) &&
            !TypeChecker.isChar(exprType) &&
            (exprType.getForm() != ENUMERATION))
        {
            errorHandler.flag(token, INCOMPATIBLE_TYPES, this);
        }

        // Synchronize at the OF.
        token = synchronize(OF_SET);
        if (token.getType() == OF) {
            token = nextToken();  // consume the OF
        }
        else {
            errorHandler.flag(token, MISSING_OF, this);
        }

        // Set of CASE branch constants.
        HashSet<Object> constantSet = new HashSet<Object>();

        // Loop to parse each CASE branch until the END token
        // or the end of the source file.
        while (!(token instanceof EofToken) && (token.getType() != END)) {

            // The SELECT node adopts the CASE branch subtree.
            selectNode.addChild(parseBranch(token, exprType, constantSet));

            token = currentToken();
            TokenType tokenType = token.getType();

            // Look for the semicolon between CASE branches.
            if (tokenType == SEMICOLON) {
                token = nextToken();  // consume the ;
            }

            // If at the start of the next constant, then missing a semicolon.
            else if (CONSTANT_START_SET.contains(tokenType)) {
                errorHandler.flag(token, MISSING_SEMICOLON, this);
            }
        }

        // Look for the END token.
        if (token.getType() == END) {
            token = nextToken();  // consume END
        }
        else {
            errorHandler.flag(token, MISSING_END, this);
        }

        return selectNode;
    }

    /**
     * Parse a CASE branch.
     * @param token the current token.
     * @param expressionType the CASE expression type.
     * @param constantSet the set of CASE branch constants.
     * @return the root SELECT_BRANCH node of the subtree.
     * @throws Exception if an error occurred.
     */
    private ICodeNode parseBranch(Token token, TypeSpec expressionType,
                                  HashSet<Object> constantSet)
        throws Exception
    {
        // Create an SELECT_BRANCH node and a SELECT_CONSTANTS node.
        // The SELECT_BRANCH node adopts the SELECT_CONSTANTS node as its
        // first child.
        ICodeNode branchNode = ICodeFactory.createICodeNode(SELECT_BRANCH);
        ICodeNode constantsNode =
                               ICodeFactory.createICodeNode(SELECT_CONSTANTS);
        branchNode.addChild(constantsNode);

        // Parse the list of CASE branch constants.
        // The SELECT_CONSTANTS node adopts each constant.
        parseConstantList(token, expressionType, constantsNode, constantSet);

        // Look for the : token.
        token = currentToken();
        if (token.getType() == COLON) {
            token = nextToken();  // consume the :
        }
        else {
            errorHandler.flag(token, MISSING_COLON, this);
        }

        // Parse the CASE branch statement. The SELECT_BRANCH node adopts
        // the statement subtree as its second child.
        StatementParser statementParser = new StatementParser(this);
        branchNode.addChild(statementParser.parse(token));

        return branchNode;
    }

    // Synchronization set for COMMA.
    private static final EnumSet<PascalTokenType> COMMA_SET =
        CONSTANT_START_SET.clone();
    static {
        COMMA_SET.add(COMMA);
        COMMA_SET.add(COLON);
        COMMA_SET.addAll(StatementParser.STMT_START_SET);
        COMMA_SET.addAll(StatementParser.STMT_FOLLOW_SET);
    }

    /**
     * Parse a list of CASE branch constants.
     * @param token the current token.
     * @param expressionType the CASE expression type.
     * @param constantsNode the parent SELECT_CONSTANTS node.
     * @param constantSet the set of CASE branch constants.
     * @throws Exception if an error occurred.
     */
    private void parseConstantList(Token token,
                                   TypeSpec expressionType,
                                   ICodeNode constantsNode,
                                   HashSet<Object> constantSet)
        throws Exception
    {
        // Loop to parse each constant.
        while (CONSTANT_START_SET.contains(token.getType())) {

            // The constants list node adopts the constant node.
            constantsNode.addChild(parseConstant(token, expressionType,
                                                 constantSet));

            // Synchronize at the comma between constants.
            token = synchronize(COMMA_SET);

            // Look for the comma.
            if (token.getType() == COMMA) {
                token = nextToken();  // consume the ,
            }

            // If at the start of the next constant, then missing a comma.
            else if (CONSTANT_START_SET.contains(token.getType())) {
                errorHandler.flag(token, MISSING_COMMA, this);
            }
        }
    }

    /**
     * Parse CASE branch constant.
     * @param token the current token.
     * @param expressionType the CASE expression type.
     * @param constantSet the set of CASE branch constants.
     * @return the constant node.
     * @throws Exception if an error occurred.
     */
    private ICodeNode parseConstant(Token token, TypeSpec expressionType,
                                    HashSet<Object> constantSet)
        throws Exception
    {
        TokenType sign = null;
        ICodeNode constantNode = null;
        TypeSpec constantType = null;

        // Synchronize at the start of a constant.
        token = synchronize(CONSTANT_START_SET);
        TokenType tokenType = token.getType();

        // Plus or minus sign?
        if ((tokenType == PLUS) || (tokenType == MINUS)) {
            sign = tokenType;
            token = nextToken();  // consume sign
        }

        // Parse the constant.
        switch ((PascalTokenType) token.getType()) {

            case IDENTIFIER: {
                constantNode = parseIdentifierConstant(token, sign);
                if (constantNode != null) {
                    constantType = constantNode.getTypeSpec();
                }

                break;
            }

            case INTEGER: {
                constantNode = parseIntegerConstant(token.getText(), sign);
                constantType = Predefined.integerType;
                break;
            }

            case STRING: {
                constantNode =
                    parseCharacterConstant(token, (String) token.getValue(),
                                           sign);
                constantType = Predefined.charType;
                break;
            }

            default: {
                errorHandler.flag(token, INVALID_CONSTANT, this);
                break;
            }
        }

        // Check for reused constants.
        if (constantNode != null) {
            Object value = constantNode.getAttribute(VALUE);

            if (constantSet.contains(value)) {
                errorHandler.flag(token, CASE_CONSTANT_REUSED, this);
            }
            else {
                constantSet.add(value);
            }
        }

        // Type check: The constant type must be comparison compatible
        //             with the CASE expression type.
        if (!TypeChecker.areComparisonCompatible(expressionType,
                                                 constantType)) {
            errorHandler.flag(token, INCOMPATIBLE_TYPES, this);
        }

        token = nextToken();  // consume the constant

        constantNode.setTypeSpec(constantType);
        return constantNode;
    }

    /**
     * Parse an identifier CASE constant.
     * @param value the current token value string.
     * @param sign the sign, if any.
     * @return the constant node.
     */
    private ICodeNode parseIdentifierConstant(Token token, TokenType sign)
        throws Exception
    {
        ICodeNode constantNode = null;
        TypeSpec constantType = null;

        // Look up the identifier in the symbol table stack.
        String name = token.getText().toLowerCase();
        SymTabEntry id = symTabStack.lookup(name);

        // Undefined.
        if (id == null) {
            id = symTabStack.enterLocal(name);
            id.setDefinition(UNDEFINED);
            id.setTypeSpec(Predefined.undefinedType);
            errorHandler.flag(token, IDENTIFIER_UNDEFINED, this);
            return null;
        }

        Definition defnCode = id.getDefinition();

        // Constant identifier.
        if ((defnCode == CONSTANT) || (defnCode == ENUMERATION_CONSTANT)) {
            Object constantValue = id.getAttribute(CONSTANT_VALUE);
            constantType = id.getTypeSpec();

            // Type check: Leading sign permitted only for integer constants.
            if ((sign != null) && !TypeChecker.isInteger(constantType)) {
                errorHandler.flag(token, INVALID_CONSTANT, this);
            }

            constantNode = ICodeFactory.createICodeNode(INTEGER_CONSTANT);
            constantNode.setAttribute(VALUE, constantValue);
        }

        id.appendLineNumber(token.getLineNumber());

        if (constantNode != null) {
            constantNode.setTypeSpec(constantType);
        }

        return constantNode;
    }

    /**
     * Parse an integer CASE constant.
     * @param value the current token value string.
     * @param sign the sign, if any.
     * @return the constant node.
     */
    private ICodeNode parseIntegerConstant(String value, TokenType sign)
    {
        ICodeNode constantNode = ICodeFactory.createICodeNode(INTEGER_CONSTANT);
        int intValue = Integer.parseInt(value);

        if (sign == MINUS) {
            intValue = -intValue;
        }

        constantNode.setAttribute(VALUE, intValue);
        return constantNode;
    }

    /**
     * Parse a character CASE constant.
     * @param token the current token.
     * @param value the token value string.
     * @param sign the sign, if any.
     * @return the constant node.
     */
    private ICodeNode parseCharacterConstant(Token token, String value,
                                             TokenType sign)
    {
        ICodeNode constantNode = null;

        if (sign != null) {
            errorHandler.flag(token, INVALID_CONSTANT, this);
        }
        else {
            if (value.length() == 1) {
                constantNode = ICodeFactory.createICodeNode(STRING_CONSTANT);
                constantNode.setAttribute(VALUE, value);
            }
            else {
                errorHandler.flag(token, INVALID_CONSTANT, this);
            }
        }

        return constantNode;
    }
}
