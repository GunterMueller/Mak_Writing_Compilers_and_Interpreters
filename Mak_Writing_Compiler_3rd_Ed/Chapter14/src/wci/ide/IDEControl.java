package wci.ide;

import java.awt.Color;

/**
 * <h1>IDEControl</h1>
 *
 * <p>The master interface of the Pascal IDE.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public interface IDEControl
{
    // Debugger output line tags.
    public static final String LISTING_TAG = "!LISTING:";
    public static final String PARSER_TAG = "!PARSER:";
    public static final String SYNTAX_TAG = "!SYNTAX:";
    public static final String INTERPRETER_TAG = "!INTERPRETER:";

    public static final String DEBUGGER_AT_TAG = "!DEBUGGER.AT:";
    public static final String DEBUGGER_BREAK_TAG = "!DEBUGGER.BREAK:";
    public static final String DEBUGGER_ROUTINE_TAG = "!DEBUGGER.ROUTINE:";
    public static final String DEBUGGER_VARIABLE_TAG = "!DEBUGGER.VARIABLE:";

    /**
     * Set the path of the source file.
     * @param sourcePath the path.
     */
    public void setSourcePath(String sourcePath);

    /**
     * @return the path of the source file.
     */
    public String getSourcePath();

    /**
     * Set the path of the runtime input data file.
     * @param inputPath the path.
     */
    public void setInputPath(String inputPath);

    /**
     * @return the path of the runtime input data file.
     */
    public String getInputPath();

    /**
     * Start the debugger process.
     * @param sourceName the source file name.
     */
    public void startDebuggerProcess(String sourceName);

    /**
     * Stop the debugger process.
     */
    public void stopDebuggerProcess();

    /**
     * Send a command or runtime input text to the debugger process.
     * @param text the command string or input text.
     */
    public void sendToDebuggerProcess(String text);

    /**
     * Set the editor window's message.
     * @param message the message.
     * @param color the message color.
     */
    public void setEditWindowMessage(String message, Color color);

    /**
     * Clear the editor window's syntax errors.
     */
    public void clearEditWindowErrors();

    /**
     * Add a syntax error message to the editor window's syntax errors.
     * @param line the error message.
     */
    public void addToEditWindowErrors(String line);

    /**
     * Show the debugger window.
     * @param sourceName the source file name.
     */
    public void showDebugWindow(String sourceName);

    /**
     * Clear the debugger window's listing.
     */
    public void clearDebugWindowListing();

    /**
     * Add a line to the debugger window's listing.
     * @param line the listing line.
     */
    public void addToDebugWindowListing(String line);

    /**
     * Select a listing line in the debugger window.
     * @param lineNumber the line number.
     */
    public void selectDebugWindowListingLine(int lineNumber);

    /**
     * Set the debugger to a listing line.
     * @param lineNumber the line number.
     */
    public void setDebugWindowAtListingLine(int lineNumber);

    /**
     * Set the debugger to break at a listing line.
     * @param lineNumber the line number.
     */
    public void breakDebugWindowAtListingLine(int lineNumber);

    /**
     * Set the debugger window's message.
     * @param message the message.
     * @param color the message color.
     */
    public void setDebugWindowMessage(String message, Color color);

    /**
     * Stop the debugger.
     */
    public void stopDebugWindow();

    /**
     * Show the call stack window.
     * @param sourceName the source file name.
     */
    public void showCallStackWindow(String sourceName);

    /**
     * Initialize the call stack display.
     */
    public void initializeCallStackWindow();

    /**
     * Add an invoked routine to the call stack display.
     * @param level the routine's nesting level.
     * @param header the routine's header.
     */
    public void addRoutineToCallStackWindow(String level, String header);

    /**
     * Add a local variable to the call stack display.
     * @param name the variable's name.
     * @param value the variable's value.
     */
    public void addVariableToCallStackWindow(String name, String value);

    /**
     * Complete the call stack display.
     */
    public void completeCallStackWindow();

    /**
     * Show the console window.
     * @param sourceName the source file name.
     */
    public void showConsoleWindow(String sourceName);

    /**
     * Clear the console window's output.
     */
    public void clearConsoleWindowOutput();

    /**
     * Add output text to the console window.
     * @param line the output text.
     */
    public void addToConsoleWindowOutput(String text);

    /**
     * Enable runtime input from the console window.
     */
    public void enableConsoleWindowInput();

    /**
     * Disable runtime input from the console window.
     */
    public void disableConsoleWindowInput();
}
