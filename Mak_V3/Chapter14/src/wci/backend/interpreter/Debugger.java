package wci.backend.interpreter;

import java.io.*;
import java.util.ArrayList;
import java.util.HashSet;

import wci.frontend.*;
import wci.frontend.pascal.*;
import wci.backend.*;
import wci.message.*;

import static wci.frontend.pascal.PascalTokenType.*;

/**
 * <h1>Debugger</h1>
 *
 * <p>Interface for the interactive source-level debugger.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public abstract class Debugger
{
    private RuntimeStack runtimeStack;     // runtime stack
    private HashSet<Integer> breakpoints;  // set of breakpoints
    private HashSet<String> watchpoints;   // set of watchpoints

    private Scanner commandInput;          // input source for commands

    /**
     * Constructor.
     * @param backend the back end.
     * @param runtimeStack the runtime stack.
     */
    public Debugger(Backend backend, RuntimeStack runtimeStack)
    {
        this.runtimeStack = runtimeStack;
        backend.addMessageListener(new BackendMessageListener());

        breakpoints = new HashSet<Integer>();
        watchpoints = new HashSet<String>();

        // Create the command input from the standard input.
        try {
            commandInput = new PascalScanner(
                               new Source(
                                   new BufferedReader(
                                       new InputStreamReader(System.in))));
        }
        catch(IOException ignore) {};
    }

    /**
     * Getter.
     * @return the runtime stack.
     */
    public RuntimeStack getRuntimeStack()
    {
        return runtimeStack;
    }

    /**
     * Listener for back end messages.
     */
    private class BackendMessageListener implements MessageListener
    {
        /**
         * Called by the back end whenever it produces a message.
         * @param message the message.
         */
        public void messageReceived(Message message)
        {
            processMessage(message);
        }
    }

    /**
     * Read the debugger commands.
     */
    public void readCommands()
    {
        do {
            promptForCommand();
        } while (parseCommand());
    }

    /**
     * Return the current token from the command input.
     * @return the token.
     * @throws Exception if an error occurred.
     */
    public Token currentToken()
        throws Exception
    {
        return commandInput.currentToken();
    }

    /**
     * Return the next token from the command input.
     * @return the token.
     * @throws Exception if an error occurred.
     */
    public Token nextToken()
        throws Exception
    {
        return commandInput.nextToken();
    }

    /**
     * Get the next word token from the command input.
     * @param errorMessage the error message if an exception is thrown.
     * @return the text of the word token.
     * @throws Exception if an error occurred.
     */
    public String getWord(String errorMessage)
        throws Exception
    {
        Token token = currentToken();
        TokenType type = token.getType();

        if (type == IDENTIFIER) {
            String word = token.getText().toLowerCase();
            nextToken();
            return word;
        }
        else {
            throw new Exception(errorMessage);
        }
    }

    /**
     * Get the next integer constant token from the command input.
     * @param errorMessage the error message if an exception is thrown.
     * @return the constant integer value.
     * @throws Exception if an error occurred.
     */
    public Integer getInteger(String errorMessage)
        throws Exception
    {
        Token token = currentToken();
        TokenType type = token.getType();

        if (type == INTEGER) {
            Integer value = (Integer) token.getValue();
            nextToken();
            return value;
        }
        else {
            throw new Exception(errorMessage);
        }
    }

    /**
     * Get the next constant value token from the command input.
     * @param errorMessage the error message if an exception is thrown.
     * @return the constant value.
     * @throws Exception if an error occurred.
     */
    public Object getValue(String errorMessage)
        throws Exception
    {
        Token token = currentToken();
        TokenType tokenType = token.getType();
        boolean sign = false;
        boolean minus = false;

        // Unary plus or minus sign.
        if ((tokenType == MINUS) | (tokenType == PLUS)) {
            sign = true;
            minus = tokenType == MINUS;
            token = nextToken();
            tokenType = token.getType();
        }

        switch ((PascalTokenType) tokenType) {

            case INTEGER: {
                Integer value = (Integer) token.getValue();
                nextToken();
                return minus ? -value : value;
            }

            case REAL: {
                Float value = (Float) token.getValue();
                nextToken();
                return minus ? -value : value;
            }

            case STRING: {
                if (sign) {
                    throw new Exception(errorMessage);
                }
                else {
                    String value = (String) token.getValue();
                    nextToken();
                    return value.charAt(0);
                }
            }

            case IDENTIFIER: {
                if (sign) {
                    throw new Exception(errorMessage);
                }
                else {
                    String name = token.getText();
                    nextToken();

                    if (name.equalsIgnoreCase("true")) {
                        return (Boolean) true;
                    }
                    else if (name.equalsIgnoreCase("false")) {
                        return (Boolean) false;
                    }
                    else {
                        throw new Exception(errorMessage);
                    }
                }
            }

            default: {
                throw new Exception(errorMessage);
            }
        }
    }

    /**
     * Skip the rest of this command input line.
     * @throws Exception if an error occurred.
     */
    public void skipToNextCommand()
        throws Exception
    {
        commandInput.skipToNextLine();
    }

    /**
     * Set a breakpoint at a source line.
     * @param lineNumber the source line number.
     */
    public void setBreakpoint(Integer lineNumber)
    {
        breakpoints.add(lineNumber);
    }

    /**
     * Remove a breakpoint at a source line.
     * @param lineNumber the source line number.
     */
    public void unsetBreakpoint(Integer lineNumber)
    {
        breakpoints.remove(lineNumber);
    }

    /**
     * Check if a source line is at a breakpoint.
     * @param lineNumber the source line number
     * @return true if at a breakpoint, else false.
     */
    public boolean isBreakpoint(Integer lineNumber)
    {
        return breakpoints.contains(lineNumber);
    }

    /**
     * Set a watchpoint on a variable.
     * @param name the variable name.
     */
    public void setWatchpoint(String name)
    {
        watchpoints.add(name);
    }

    /**
     * Remove a watchpoint on a variable.
     * @param name the variable name.
     */
    public void unsetWatchpoint(String name)
    {
        watchpoints.remove(name);
    }

    /**
     * Check if a variable is a watchpoint.
     * @param name the variable name.
     * @return true if a watchpoint, else false.
     */
    public boolean isWatchpoint(String name)
    {
        return watchpoints.contains(name);
    }

    /**
     * Process a message from the back end.
     * @param message the message.
     */
    public abstract void processMessage(Message message);

    /**
     * Display a prompt for a debugger command.
     */
    public abstract void promptForCommand();

    /**
     * Parse a debugger command.
     * @return true to parse another command immediately, else false.
     */
    public abstract boolean parseCommand();

    /**
     * Process a source statement.
     * @param lineNumber the statement line number.
     */
    public abstract void atStatement(Integer lineNumber);

    /**
     * Process a breakpoint at a statement.
     * @param lineNumber the statement line number.
     */
    public abstract void atBreakpoint(Integer lineNumber);

    /**
     * Process the current value of a watchpoint variable.
     * @param lineNumber the current statement line number.
     * @param name the variable name.
     * @param value the variable's value.
     */
    public abstract void atWatchpointValue(Integer lineNumber,
                                           String name, Object value);

    /**
     * Process the assigning a new value to a watchpoint variable.
     * @param lineNumber the current statement line number.
     * @param name the variable name.
     * @param value the new value.
     */
    public abstract void atWatchpointAssignment(Integer lineNumber,
                                                String name, Object value);

    /**
     * Process calling a declared procedure or function.
     * @param lineNumber the current statement line number.
     * @param name the routine name.
     */
    public abstract void callRoutine(Integer lineNumber, String routineName);

    /**
     * Process returning from a declared procedure or function.
     * @param lineNumber the current statement line number.
     * @param name the routine name.
     */
    public abstract void returnRoutine(Integer lineNumber, String routineName);

    /**
     * Display a value.
     * @param valueString the value string.
     */
    public abstract void displayValue(String valueString);

    /**
     * Display the call stack.
     * @param stack the list of elements of the call stack.
     */
    public abstract void displayCallStack(ArrayList stack);

    /**
     * Terminate execution of the source program.
     */
    public abstract void quit();

    /**
     * Handle a debugger command error.
     * @param errorMessage the error message.
     */
    public abstract void commandError(String errorMessage);

    /**
     * Handle a source program runtime error.
     * @param errorMessage the error message.
     * @param lineNumber the source line number where the error occurred.
     */
    public abstract void runtimeError(String errorMessage, Integer lineNumber);
}
