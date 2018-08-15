package wci.frontend.pascal.parsers;

import java.util.ArrayList;
import java.util.EnumSet;

import wci.frontend.*;
import wci.frontend.pascal.*;
import wci.intermediate.*;
import wci.intermediate.symtabimpl.*;
import wci.intermediate.typeimpl.*;

import static wci.frontend.pascal.PascalTokenType.*;
import static wci.frontend.pascal.PascalErrorCode.*;
import static wci.intermediate.symtabimpl.SymTabKeyImpl.*;
import static wci.intermediate.symtabimpl.DefinitionImpl.*;
import static wci.intermediate.symtabimpl.RoutineCodeImpl.*;

/**
 * <h1>DeclaredRoutineParser</h1>
 *
 * <p>Parse a main program routine or a declared procedure or function.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class DeclaredRoutineParser extends DeclarationsParser
{
    /**
     * Constructor.
     * @param parent the parent parser.
     */
    public DeclaredRoutineParser(PascalParserTD parent)
    {
        super(parent);
    }

    private static int dummyCounter = 0;  // counter for dummy routine names

    /**
     * Parse a standard subroutine declaration.
     * @param token the initial token.
     * @param parentId the symbol table entry of the parent routine's name.
     * @return the symbol table entry of the declared routine's name.
     * @throws Exception if an error occurred.
     */
    public SymTabEntry parse(Token token, SymTabEntry parentId)
        throws Exception
    {
        Definition routineDefn = null;
        String dummyName = null;
        SymTabEntry routineId = null;
        TokenType routineType = token.getType();

        // Initialize.
        switch ((PascalTokenType) routineType) {

            case PROGRAM: {
                token = nextToken();  // consume PROGRAM
                routineDefn = DefinitionImpl.PROGRAM;
                dummyName = "DummyProgramName".toLowerCase();
                break;
            }

            case PROCEDURE: {
                token = nextToken();  // consume PROCEDURE
                routineDefn = DefinitionImpl.PROCEDURE;
                dummyName = "DummyProcedureName_".toLowerCase() +
                            String.format("%03d", ++dummyCounter);
                break;
            }

            case FUNCTION: {
                token = nextToken();  // consume FUNCTION
                routineDefn = DefinitionImpl.FUNCTION;
                dummyName = "DummyFunctionName_".toLowerCase() +
                            String.format("%03d", ++dummyCounter);
                break;
            }

            default: {
                routineDefn = DefinitionImpl.PROGRAM;
                dummyName = "DummyProgramName".toLowerCase();
                break;
            }
        }

        // Parse the routine name.
        routineId = parseRoutineName(token, dummyName);
        routineId.setDefinition(routineDefn);

        token = currentToken();

        // Create new intermediate code for the routine.
        ICode iCode = ICodeFactory.createICode();
        routineId.setAttribute(ROUTINE_ICODE, iCode);
        routineId.setAttribute(ROUTINE_ROUTINES, new ArrayList<SymTabEntry>());

        // Push the routine's new symbol table onto the stack.
        // If it was forwarded, push its existing symbol table.
        if (routineId.getAttribute(ROUTINE_CODE) == FORWARD) {
            SymTab symTab = (SymTab) routineId.getAttribute(ROUTINE_SYMTAB);
            symTabStack.push(symTab);
        }
        else {
            routineId.setAttribute(ROUTINE_SYMTAB, symTabStack.push());
        }

        // Program: Set the program identifier in the symbol table stack.
        // Set the initial local variables array slot number to 1.
        if (routineDefn == DefinitionImpl.PROGRAM) {
            symTabStack.setProgramId(routineId);
            symTabStack.getLocalSymTab().nextSlotNumber();  // bump slot number
        }

        // Non-forwarded procedure or function: Append to the parent's list
        //                                      of routines.
        else if (routineId.getAttribute(ROUTINE_CODE) != FORWARD) {
            ArrayList<SymTabEntry> subroutines = (ArrayList<SymTabEntry>)
                                       parentId.getAttribute(ROUTINE_ROUTINES);
            subroutines.add(routineId);
        }

        // If the routine was forwarded, there should not be
        // any formal parameters or a function return type.
        // But parse them anyway if they're there.
        if (routineId.getAttribute(ROUTINE_CODE) == FORWARD) {
            if (token.getType() != SEMICOLON) {
                errorHandler.flag(token, ALREADY_FORWARDED, this);
                parseHeader(token, routineId);
            }
        }

        // Parse the routine's formal parameters and function return type.
        else {
            parseHeader(token, routineId);
        }

        // Look for the semicolon.
        token = currentToken();
        if (token.getType() == SEMICOLON) {
            do {
                token = nextToken();  // consume ;
            } while (token.getType() == SEMICOLON);
        }
        else {
            errorHandler.flag(token, MISSING_SEMICOLON, this);
        }

        // Parse the routine's block or forward declaration.
        if ((token.getType() == IDENTIFIER) &&
            (token.getText().equalsIgnoreCase("forward")))
        {
            token = nextToken();  // consume forward
            routineId.setAttribute(ROUTINE_CODE, FORWARD);
        }
        else {
            routineId.setAttribute(ROUTINE_CODE, DECLARED);

            BlockParser blockParser = new BlockParser(this);
            ICodeNode rootNode = blockParser.parse(token, routineId);
            iCode.setRoot(rootNode);
        }

        // Pop the routine's symbol table off the stack.
        symTabStack.pop();

        return routineId;
    }

    /**
     * Parse a routine's name.
     * @param token the current token.
     * @param routineDefn how the routine is defined.
     * @param dummyName a dummy name in case of parsing problem.
     * @return the symbol table entry of the declared routine's name.
     * @throws Exception if an error occurred.
     */
    private SymTabEntry parseRoutineName(Token token, String dummyName)
        throws Exception
    {
        SymTabEntry routineId = null;

        // Parse the routine name identifier.
        if (token.getType() == IDENTIFIER) {
            String routineName = token.getText().toLowerCase();
            routineId = symTabStack.lookupLocal(routineName);

            // Not already defined locally: Enter into the local symbol table.
            if (routineId == null) {
                routineId = symTabStack.enterLocal(routineName);
            }

            // If already defined, it should be a forward definition.
            else if (routineId.getAttribute(ROUTINE_CODE) != FORWARD) {
                routineId = null;
                errorHandler.flag(token, IDENTIFIER_REDEFINED, this);
            }

            token = nextToken();  // consume routine name identifier
        }
        else {
            errorHandler.flag(token, MISSING_IDENTIFIER, this);
        }

        // If necessary, create a dummy routine name symbol table entry.
        if (routineId == null) {
            routineId = symTabStack.enterLocal(dummyName);
        }

        return routineId;
    }

    /**
     * Parse a routine's formal parameter list and the function return type.
     * @param token the current token.
     * @param routineId the symbol table entry of the declared routine's name.
     * @throws Exception if an error occurred.
     */
    private void parseHeader(Token token, SymTabEntry routineId)
        throws Exception
    {
        // Parse the routine's formal parameters.
        parseFormalParameters(token, routineId);
        token = currentToken();

        // If this is a function, parse and set its return type.
        if (routineId.getDefinition() == DefinitionImpl.FUNCTION) {
            VariableDeclarationsParser variableDeclarationsParser =
                new VariableDeclarationsParser(this);
            variableDeclarationsParser.setDefinition(DefinitionImpl.FUNCTION);
            TypeSpec type = variableDeclarationsParser.parseTypeSpec(token);

            token = currentToken();

            // The return type cannot be an array or record.
            if (type != null) {
                TypeForm form = type.getForm();
                if ((form == TypeFormImpl.ARRAY) ||
                    (form == TypeFormImpl.RECORD))
                {
                    errorHandler.flag(token, INVALID_TYPE, this);
                }
            }

            // Missing return type.
            else {
                type = Predefined.undefinedType;
            }

            routineId.setTypeSpec(type);
            token = currentToken();
        }
    }

    // Synchronization set for a formal parameter sublist.
    private static final EnumSet<PascalTokenType> PARAMETER_SET =
        DeclarationsParser.DECLARATION_START_SET.clone();
    static {
        PARAMETER_SET.add(VAR);
        PARAMETER_SET.add(IDENTIFIER);
        PARAMETER_SET.add(RIGHT_PAREN);
    }

    // Synchronization set for the opening left parenthesis.
    private static final EnumSet<PascalTokenType> LEFT_PAREN_SET =
        DeclarationsParser.DECLARATION_START_SET.clone();
    static {
        LEFT_PAREN_SET.add(LEFT_PAREN);
        LEFT_PAREN_SET.add(SEMICOLON);
        LEFT_PAREN_SET.add(COLON);
    }

    // Synchronization set for the closing right parenthesis.
    private static final EnumSet<PascalTokenType> RIGHT_PAREN_SET =
        LEFT_PAREN_SET.clone();
    static {
        RIGHT_PAREN_SET.remove(LEFT_PAREN);
        RIGHT_PAREN_SET.add(RIGHT_PAREN);
    }

    /**
     * Parse a routine's formal parameter list.
     * @param token the current token.
     * @param routineId the symbol table entry of the declared routine's name.
     * @throws Exception if an error occurred.
     */
    protected void parseFormalParameters(Token token, SymTabEntry routineId)
        throws Exception
    {
        // Parse the formal parameters if there is an opening left parenthesis.
        token = synchronize(LEFT_PAREN_SET);
        if (token.getType() == LEFT_PAREN) {
            token = nextToken();  // consume (

            ArrayList<SymTabEntry> parms = new ArrayList<SymTabEntry>();

            token = synchronize(PARAMETER_SET);
            TokenType tokenType = token.getType();

            // Loop to parse sublists of formal parameter declarations.
            while ((tokenType == IDENTIFIER) || (tokenType == VAR)) {
                parms.addAll(parseParmSublist(token, routineId));
                token = currentToken();
                tokenType = token.getType();
            }

            // Closing right parenthesis.
            if (token.getType() == RIGHT_PAREN) {
                token = nextToken();  // consume )
            }
            else {
                errorHandler.flag(token, MISSING_RIGHT_PAREN, this);
            }

            routineId.setAttribute(ROUTINE_PARMS, parms);
        }
    }

    // Synchronization set to follow a formal parameter identifier.
    private static final EnumSet<PascalTokenType> PARAMETER_FOLLOW_SET =
        EnumSet.of(COLON, RIGHT_PAREN, SEMICOLON);
    static {
        PARAMETER_FOLLOW_SET.addAll(DeclarationsParser.DECLARATION_START_SET);
    }

    // Synchronization set for the , token.
    private static final EnumSet<PascalTokenType> COMMA_SET =
        EnumSet.of(COMMA, COLON, IDENTIFIER, RIGHT_PAREN, SEMICOLON);
    static {
        COMMA_SET.addAll(DeclarationsParser.DECLARATION_START_SET);
    }

    /**
     * Parse a sublist of formal parameter declarations.
     * @param token the current token.
     * @param routineId the symbol table entry of the declared routine's name.
     * @return the sublist of symbol table entries for the parm identifiers.
     * @throws Exception if an error occurred.
     */
    private ArrayList<SymTabEntry> parseParmSublist(Token token,
                                                    SymTabEntry routineId)
        throws Exception
    {
        boolean isProgram = routineId.getDefinition() == DefinitionImpl.PROGRAM;
        Definition parmDefn = isProgram ? PROGRAM_PARM : null;
        TokenType tokenType = token.getType();

        // VAR or value parameter?
        if (tokenType == VAR) {
            if (!isProgram) {
                parmDefn = VAR_PARM;
            }
            else {
                errorHandler.flag(token, INVALID_VAR_PARM, this);
            }

            token = nextToken();  // consume VAR
        }
        else if (!isProgram) {
            parmDefn = VALUE_PARM;
        }

        // Parse the parameter sublist and its type specification.
        VariableDeclarationsParser variableDeclarationsParser =
            new VariableDeclarationsParser(this);
        variableDeclarationsParser.setDefinition(parmDefn);
        ArrayList<SymTabEntry> sublist =
            variableDeclarationsParser.parseIdentifierSublist(
                                           token, PARAMETER_FOLLOW_SET,
                                           COMMA_SET);
        token = currentToken();
        tokenType = token.getType();

        if (!isProgram) {

            // Look for one or more semicolons after a sublist.
            if (tokenType == SEMICOLON) {
                while (token.getType() == SEMICOLON) {
                    token = nextToken();  // consume the ;
                }
            }

            // If at the start of the next sublist, then missing a semicolon.
            else if (VariableDeclarationsParser.
                         NEXT_START_SET.contains(tokenType)) {
                errorHandler.flag(token, MISSING_SEMICOLON, this);
            }

            token = synchronize(PARAMETER_SET);
        }

        return sublist;
    }
}
