package wci.ide;

import wci.ide.*;
import wci.ide.ideimpl.*;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/**
 * <h1>IDEFrame</h1>
 *
 * <p>The main window of the Pascal IDE.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class IDEFrame
    extends JFrame
    implements IDEControl
{
    private static final String TITLE = "Pascal IDE";

    private static final int WINDOW_SPACING = 10;
    private static final int HEADER_HEIGHT = 32;
    private static final int BORDER_SIZE = 5;

    private static final int EDITOR_WIDTH = 600;
    private static final int EDITOR_HEIGHT = 600;

    private static final int STACK_WIDTH = 400;
    private static final int STACK_HEIGHT = 600;

    private static final int CONSOLE_WIDTH = EDITOR_WIDTH + WINDOW_SPACING +
                                             STACK_WIDTH;
    private static final int CONSOLE_HEIGHT = 350;

    private static final int DEBUGGER_WIDTH = 600;
    private static final int DEBUGGER_HEIGHT = EDITOR_HEIGHT + WINDOW_SPACING +
                                               CONSOLE_HEIGHT;

    private static final int IDE_WIDTH = EDITOR_WIDTH + WINDOW_SPACING +
                                         STACK_WIDTH + WINDOW_SPACING +
                                         DEBUGGER_WIDTH + 2*BORDER_SIZE;
    private static final int IDE_HEIGHT = HEADER_HEIGHT + DEBUGGER_HEIGHT +
                                          2*BORDER_SIZE;

    private JDesktopPane desktop = new JDesktopPane();
    private EditFrame editFrame;
    private DebugFrame debugFrame;
    private ConsoleFrame consoleFrame;
    private CallStackFrame stackFrame;

    private DebuggerProcess debuggerProcess;

    private String sourcePath;
    private String inputPath;

    /**
     * Constructor.
     */
    public IDEFrame()
    {
        Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
        Dimension frameSize = new Dimension(IDE_WIDTH, IDE_HEIGHT);

        setSize(frameSize);
        setLocation((screenSize.width - frameSize.width)/2, 10);
        setTitle(TITLE);
        setVisible(true);
        validate();

        JPanel contentPanel = (JPanel) this.getContentPane();
        contentPanel.add(desktop);
        desktop.setBackground(new Color(215, 215, 255));

        // Editor window.
        editFrame = new EditFrame(this);
        editFrame.setSize(EDITOR_WIDTH, EDITOR_HEIGHT);
        editFrame.setLocation(0, 0);
        desktop.add(editFrame);
        editFrame.setVisible(true);

        // Debugger window.
        debugFrame = new DebugFrame(this);
        debugFrame.setSize(DEBUGGER_WIDTH, DEBUGGER_HEIGHT);
        debugFrame.setLocation(IDE_WIDTH - BORDER_SIZE - DEBUGGER_WIDTH, 0);
        desktop.add(debugFrame);
        debugFrame.setVisible(false);

        // Console window.
        consoleFrame = new ConsoleFrame(this);
        consoleFrame.setSize(CONSOLE_WIDTH, CONSOLE_HEIGHT);
        consoleFrame.setLocation(0, EDITOR_HEIGHT + WINDOW_SPACING);
        desktop.add(consoleFrame);
        consoleFrame.setVisible(false);

        // Call stack window.
        stackFrame = new CallStackFrame(this);
        stackFrame.setSize(STACK_WIDTH, STACK_HEIGHT);
        stackFrame.setLocation(EDITOR_WIDTH + WINDOW_SPACING, 0);
        desktop.add(stackFrame);
        stackFrame.setVisible(false);
    }

    /**
     * Process a window event.
     * @param event the event.
     */
    protected void processWindowEvent(WindowEvent event)
    {
        super.processWindowEvent(event);

        if (event.getID() == WindowEvent.WINDOW_CLOSING) {
            if (debuggerProcess != null) {
                debuggerProcess.kill();
            }

            debugFrame.stop();
            System.exit(0);
        }
    }

    /**
     * Set the path of the source file.
     * @param sourcePath the path.
     */
    public void setSourcePath(String sourcePath)
    {
        this.sourcePath = sourcePath;
    }

    /**
     * @return the path of the source file.
     */
    public String getSourcePath()
    {
        return sourcePath != null ? sourcePath : "";
    }

    /**
     * Set the path of the runtime input data file.
     * @param inputPath the path.
     */
    public void setInputPath(String inputPath)
    {
        this.inputPath = inputPath;
    }

    /**
     * @return the path of the runtime input data file.
     */
    public String getInputPath()
    {
        return inputPath != null ? inputPath : "";
    }

    /**
     * Start the debugger process.
     * @param sourceName the source file name.
     */
    public void startDebuggerProcess(String sourceName)
    {
        debuggerProcess = new DebuggerProcess(this, sourceName);
        debuggerProcess.start();
    }

    /**
     * Stop the debugger process.
     */
    public void stopDebuggerProcess()
    {
        if (debuggerProcess != null) {
            debuggerProcess.kill();
            debuggerProcess = null;
        }
    }

    /**
     * Send a command or runtime input text to the debugger process.
     * @param text the command string or input text.
     */
    public void sendToDebuggerProcess(String text)
    {
        debuggerProcess.writeToDebuggerStandardInput(text);
    }

    /**
     * Set the editor window's message.
     * @param message the message.
     * @param color the message color.
     */
    public void setEditWindowMessage(String message, Color color)
    {
        editFrame.setMessage(message, color);
    }

    /**
     * Clear the editor window's syntax errors.
     */
    public void clearEditWindowErrors()
    {
        editFrame.clearEditWindowErrors();
    }

    /**
     * Add a syntax error message to the editor window's syntax errors.
     * @param line the error message.
     */
    public void addToEditWindowErrors(String line)
    {
        editFrame.addError(line);
    }

    /**
     * Show the debugger window.
     * @param sourceName the source file name.
     */
    public void showDebugWindow(String sourceName)
    {
        desktop.getDesktopManager().deiconifyFrame(debugFrame);
        debugFrame.setTitle("DEBUG: " + sourceName);
        debugFrame.setVisible(true);
        debugFrame.initialize();
    }

    /**
     * Clear the debugger window's listing.
     */
    public void clearDebugWindowListing()
    {
        debugFrame.clearListing();
    }

    /**
     * Add a line to the debugger window's listing.
     * @param line the listing line.
     */
    public void addToDebugWindowListing(String line)
    {
        debugFrame.addListingLine(line);
    }

    /**
     * Select a listing line in the debugger window.
     * @param lineNumber the line number.
     */
    public void selectDebugWindowListingLine(int lineNumber)
    {
        debugFrame.selectListingLine(lineNumber);
    }

    /**
     * Set the debugger to a listing line.
     * @param lineNumber the line number.
     */
    public void setDebugWindowAtListingLine(int lineNumber)
    {
        debugFrame.atListingLine(lineNumber);
    }

    /**
     * Set the debugger to break at a listing line.
     * @param lineNumber the line number.
     */
    public void breakDebugWindowAtListingLine(int lineNumber)
    {
        debugFrame.breakAtListingLine(lineNumber);
    }

    /**
     * Set the debugger window's message.
     * @param message the message.
     * @param color the message color.
     */
    public void setDebugWindowMessage(String message, Color color)
    {
        debugFrame.setMessage(message, color);
    }

    /**
     * Stop the debugger.
     */
    public void stopDebugWindow()
    {
        debugFrame.stop();
    }

    /**
     * Show the call stack window.
     * @param sourceName the source file name.
     */
    public void showCallStackWindow(String sourceName)
    {
        desktop.getDesktopManager().deiconifyFrame(stackFrame);
        stackFrame.setTitle("CALL STACK: " + sourceName);
        stackFrame.setVisible(true);
        stackFrame.initialize();
        stackFrame.toBack();

        try {
            debugFrame.setSelected(true);
        }
        catch(Exception ignore) {}
    }

    /**
     * Initialize the call stack display.
     */
    public void initializeCallStackWindow()
    {
        stackFrame.initialize();
    }

    /**
     * Add an invoked routine to the call stack display.
     * @param level the routine's nesting level.
     * @param header the routine's header.
     */
    public void addRoutineToCallStackWindow(String level, String header)
    {
        stackFrame.addRoutine(level, header);
    }

    /**
     * Add a local variable to the call stack display.
     * @param name the variable's name.
     * @param value the variable's value.
     */
    public void addVariableToCallStackWindow(String name, String value)
    {
        stackFrame.addVariable(name, value);
    }

    /**
     * Complete the call stack display.
     */
    public void completeCallStackWindow()
    {
        stackFrame.complete();
    }

    /**
     * Show the console window.
     * @param sourceName the source file name.
     */
    public void showConsoleWindow(String sourceName)
    {
        desktop.getDesktopManager().deiconifyFrame(consoleFrame);
        consoleFrame.setTitle("CONSOLE: " + sourceName);
        consoleFrame.setVisible(true);
        consoleFrame.initialize();
        consoleFrame.toBack();

        try {
            debugFrame.setSelected(true);
        }
        catch(Exception ignore) {}
    }

    /**
     * Clear the console window's output.
     */
    public void clearConsoleWindowOutput()
    {
        consoleFrame.clearOutput();
    }

    /**
     * Add output text to the console window.
     * @param line the output text.
     */
    public void addToConsoleWindowOutput(String text)
    {
       consoleFrame.addToOutput(text);
    }

    /**
     * Enable runtime input from the console window.
     */
    public void enableConsoleWindowInput()
    {
        consoleFrame.enableInput();
    }

    /**
     * Disable runtime input from the console window.
     */
    public void disableConsoleWindowInput()
    {
        consoleFrame.disableInput();
    }
}
