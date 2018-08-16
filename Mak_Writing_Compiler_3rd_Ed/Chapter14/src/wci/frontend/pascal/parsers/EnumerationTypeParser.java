package wci.frontend.pascal.parsers;

import java.util.EnumSet;
import java.util.ArrayList;

import wci.frontend.*;
import wci.frontend.pascal.*;
import wci.intermediate.*;
import wci.intermediate.symtabimpl.*;
import wci.intermediate.typeimpl.*;

import static wci.frontend.pascal.PascalTokenType.*;
import static wci.frontend.pascal.PascalErrorCode.*;
import static wci.intermediate.symtabimpl.SymTabKeyImpl.*;
import static wci.intermediate.symtabimpl.DefinitionImpl.*;
import static wci.intermediate.typeimpl.TypeFormImpl.*;
import static wci.intermediate.typeimpl.TypeKeyImpl.*;

/**
 * <h1>EnumerationTypeParser</h1>
 *
 * <p>Parse a Pascal enumeration type specification.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class EnumerationTypeParser extends TypeSpecificationParser
{
    /**
     * Constructor.
     * @param parent the parent parser.
     */
    protected EnumerationTypeParser(PascalParserTD parent)
    {
        super(parent);
    }

    // Synchronization set to start an enumeration constant.
    private static final EnumSet<PascalTokenType> ENUM_CONSTANT_START_SET =
        EnumSet.of(IDENTIFIER, COMMA);

    // Synchronization set to follow an enumeration definition.
    private static final EnumSet<PascalTokenType> ENUM_DEFINITION_FOLLOW_SET =
        EnumSet.of(RIGHT_PAREN, SEMICOLON);
    static {
        ENUM_DEFINITION_FOLLOW_SET.addAll(DeclarationsParser.VAR_START_SET);
    }

    /**
     * Parse a Pascal enumeration type specification.
     * @param token the current token.
     * @return the enumeration type specification.
     * @throws Exception if an error occurred.
     */
    public TypeSpec parse(Token token)
        throws Exception
    {
        TypeSpec enumerationType = TypeFactory.createType(ENUMERATION);
        int value = -1;
        ArrayList<SymTabEntry> constants = new ArrayList<SymTabEntry>();

        token = nextToken();  // consume the opening (

        do {
            token = synchronize(ENUM_CONSTANT_START_SET);
            parseEnumerationIdentifier(token, ++value, enumerationType,
                                       constants);

            token = currentToken();
            TokenType tokenType = token.getType();

            // Look for the comma.
            if (tokenType == COMMA) {
                token = nextToken();  // consume the comma

                if (ENUM_DEFINITION_FOLLOW_SET.contains(token.getType())) {
                    errorHandler.flag(token, MISSING_IDENTIFIER, this);
                }
            }
            else if (ENUM_CONSTANT_START_SET.contains(tokenType)) {
                errorHandler.flag(token, MISSING_COMMA, this);
            }
        } while (!ENUM_DEFINITION_FOLLOW_SET.contains(token.getType()));

        // Look for the closing ).
        if (token.getType() == RIGHT_PAREN) {
            token = nextToken();  // consume the )
        }
        else {
            errorHandler.flag(token, MISSING_RIGHT_PAREN, this);
        }

        enumerationType.setAttribute(ENUMERATION_CONSTANTS, constants);
        return enumerationType;
    }

    /**
     * Parse an enumeration identifier.
     * @param token the current token.
     * @param value the identifier's integer value (sequence number).
     * @param enumerationType the enumeration type specification.
     * @param constants the array of symbol table entries for the
     * enumeration constants.
     * @throws Exception if an error occurred.
     */
    private void parseEnumerationIdentifier(Token token, int value,
                                            TypeSpec enumerationType,
                                            ArrayList<SymTabEntry> constants)
        throws Exception
    {
        TokenType tokenType = token.getType();

        if (tokenType == IDENTIFIER) {
            String name = token.getText().toLowerCase();
            SymTabEntry constantId = symTabStack.lookupLocal(name);

            if (constantId != null) {
                errorHandler.flag(token, IDENTIFIER_REDEFINED, this);
            }
            else {
                constantId = symTabStack.enterLocal(name);
                constantId.setDefinition(ENUMERATION_CONSTANT);
                constantId.setTypeSpec(enumerationType);
                constantId.setAttribute(CONSTANT_VALUE, value);
                constantId.appendLineNumber(token.getLineNumber());
                constants.add(constantId);
            }

            token = nextToken();  // consume the identifier
        }
        else {
            errorHandler.flag(token, MISSING_IDENTIFIER, this);
        }
    }
}
