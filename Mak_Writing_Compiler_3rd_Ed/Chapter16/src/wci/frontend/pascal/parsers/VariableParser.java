package wci.frontend.pascal.parsers;

import java.util.EnumSet;

import wci.frontend.*;
import wci.frontend.pascal.*;
import wci.intermediate.*;
import wci.intermediate.symtabimpl.*;
import wci.intermediate.icodeimpl.*;
import wci.intermediate.typeimpl.*;

import static wci.frontend.pascal.PascalTokenType.*;
import static wci.frontend.pascal.PascalErrorCode.*;
import static wci.intermediate.symtabimpl.SymTabKeyImpl.*;
import static wci.intermediate.symtabimpl.DefinitionImpl.FUNCTION;
import static wci.intermediate.symtabimpl.DefinitionImpl.UNDEFINED;
import static wci.intermediate.symtabimpl.DefinitionImpl.VARIABLE;
import static wci.intermediate.symtabimpl.DefinitionImpl.VALUE_PARM;
import static wci.intermediate.symtabimpl.DefinitionImpl.VAR_PARM;
import static wci.intermediate.typeimpl.TypeFormImpl.ARRAY;
import static wci.intermediate.typeimpl.TypeFormImpl.RECORD;
import static wci.intermediate.typeimpl.TypeKeyImpl.*;
import static wci.intermediate.icodeimpl.ICodeNodeTypeImpl.*;
import static wci.intermediate.icodeimpl.ICodeKeyImpl.*;

/**
 * <h1>VariableParser</h1>
 *
 * <p>Parse a Pascal variable.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class VariableParser extends StatementParser
{
    // Set to true to parse a function name
    // as the target of an assignment.
    private boolean isFunctionTarget = false;

    /**
     * Constructor.
     * @param parent the parent parser.
     */
    public VariableParser(PascalParserTD parent)
    {
        super(parent);
    }

    private static final EnumSet<PascalTokenType> SUBSCRIPT_FIELD_START_SET =
        EnumSet.of(LEFT_BRACKET, DOT);

    /**
     * Parse a variable.
     * @param token the initial token.
     * @return the root node of the generated parse tree.
     * @throws Exception if an error occurred.
     */
    public ICodeNode parse(Token token)
        throws Exception
    {
        // Look up the identifier in the symbol table stack.
        String name = token.getText().toLowerCase();
        SymTabEntry variableId = symTabStack.lookup(name);

        // If not found, flag the error and enter the identifier
        // as an undefined identifier with an undefined type.
        if (variableId == null) {
            errorHandler.flag(token, IDENTIFIER_UNDEFINED, this);
            variableId = symTabStack.enterLocal(name);
            variableId.setDefinition(UNDEFINED);
            variableId.setTypeSpec(Predefined.undefinedType);
        }

        return parse(token, variableId);
    }

    /**
     * Parse a function name as the target of an assignment statement.
     * @param token the initial token.
     * @return the root node of the generated parse tree.
     * @throws Exception if an error occurred.
     */
    public ICodeNode parseFunctionNameTarget(Token token)
        throws Exception
    {
        isFunctionTarget = true;
        return parse(token);
    }

    /**
     * Parse a variable.
     * @param token the initial token.
     * @param variableId the symbol table entry of the variable identifier.
     * @return the root node of the generated parse tree.
     * @throws Exception if an error occurred.
     */
    public ICodeNode parse(Token token, SymTabEntry variableId)
        throws Exception
    {
        // Check how the variable is defined.
        Definition defnCode = variableId.getDefinition();
        if (! ( (defnCode == VARIABLE) || (defnCode == VALUE_PARM) ||
                (defnCode == VAR_PARM) ||
                (isFunctionTarget && (defnCode == FUNCTION) )
              )
           )
        {
            errorHandler.flag(token, INVALID_IDENTIFIER_USAGE, this);
        }

        variableId.appendLineNumber(token.getLineNumber());

        ICodeNode variableNode =
            ICodeFactory.createICodeNode(ICodeNodeTypeImpl.VARIABLE);
        variableNode.setAttribute(ID, variableId);

        token = nextToken();  // consume the identifier
        TypeSpec variableType = variableId.getTypeSpec();

        if (!isFunctionTarget) {

            // Parse array subscripts or record fields.
            while (SUBSCRIPT_FIELD_START_SET.contains(token.getType())) {
                ICodeNode subFldNode = token.getType() == LEFT_BRACKET
                                       ? parseSubscripts(variableType)
                                       : parseField(variableType);
                token = currentToken();

                // Update the variable's type.
                // The variable node adopts the SUBSCRIPTS or FIELD node.
                variableType = subFldNode.getTypeSpec();
                variableNode.addChild(subFldNode);
            }
        }

        variableNode.setTypeSpec(variableType);
        return variableNode;
    }

    // Synchronization set for the ] token.
    private static final EnumSet<PascalTokenType> RIGHT_BRACKET_SET =
        EnumSet.of(RIGHT_BRACKET, EQUALS, SEMICOLON);

    /**
     * Parse a set of comma-separated subscript expressions.
     * @param variableType the type of the array variable.
     * @return the root node of the generated parse tree.
     * @throws Exception if an error occurred.
     */
    private ICodeNode parseSubscripts(TypeSpec variableType)
        throws Exception
    {
        Token token;
        ExpressionParser expressionParser = new ExpressionParser(this);

        // Create a SUBSCRIPTS node.
        ICodeNode subscriptsNode = ICodeFactory.createICodeNode(SUBSCRIPTS);

        do {
            token = nextToken();  // consume the [ or , token

            // The current variable is an array.
            if (variableType.getForm() == ARRAY) {

                // Parse the subscript expression.
                ICodeNode exprNode = expressionParser.parse(token);
                TypeSpec exprType = exprNode != null ? exprNode.getTypeSpec()
                                                     : Predefined.undefinedType;

                // The subscript expression type must be assignment
                // compatible with the array index type.
                TypeSpec indexType =
                    (TypeSpec) variableType.getAttribute(ARRAY_INDEX_TYPE);
                if (!TypeChecker.areAssignmentCompatible(indexType, exprType)) {
                    errorHandler.flag(token, INCOMPATIBLE_TYPES, this);
                }

                // The SUBSCRIPTS node adopts the subscript expression tree.
                subscriptsNode.addChild(exprNode);

                // Update the variable's type.
                variableType =
                    (TypeSpec) variableType.getAttribute(ARRAY_ELEMENT_TYPE);
            }

            // Not an array type, so too many subscripts.
            else {
                errorHandler.flag(token, TOO_MANY_SUBSCRIPTS, this);
                expressionParser.parse(token);
            }

            token = currentToken();
        } while (token.getType() == COMMA);

        // Synchronize at the ] token.
        token = synchronize(RIGHT_BRACKET_SET);
        if (token.getType() == RIGHT_BRACKET) {
            token = nextToken();  // consume the ] token
        }
        else {
            errorHandler.flag(token, MISSING_RIGHT_BRACKET, this);
        }

        subscriptsNode.setTypeSpec(variableType);
        return subscriptsNode;
    }

    /**
     * Parse a record field.
     * @param variableType the type of the record variable.
     * @return the root node of the generated parse tree.
     * @throws Exception if an error occurred.
     */
    private ICodeNode parseField(TypeSpec variableType)
        throws Exception
    {
        // Create a FIELD node.
        ICodeNode fieldNode = ICodeFactory.createICodeNode(FIELD);

        Token token = nextToken();  // consume the . token
        TokenType tokenType = token.getType();
        TypeForm variableForm = variableType.getForm();

        if ((tokenType == IDENTIFIER) && (variableForm == RECORD)) {
            SymTab symTab = (SymTab) variableType.getAttribute(RECORD_SYMTAB);
            String fieldName = token.getText().toLowerCase();
            SymTabEntry fieldId = symTab.lookup(fieldName);

            if (fieldId != null) {
                variableType = fieldId.getTypeSpec();
                fieldId.appendLineNumber(token.getLineNumber());

                // Set the field identifier's name.
                fieldNode.setAttribute(ID, fieldId);
            }
            else {
                errorHandler.flag(token, INVALID_FIELD, this);
            }
        }
        else {
            errorHandler.flag(token, INVALID_FIELD, this);
        }

        token = nextToken();  // consume the field identifier

        fieldNode.setTypeSpec(variableType);
        return fieldNode;
    }
}
