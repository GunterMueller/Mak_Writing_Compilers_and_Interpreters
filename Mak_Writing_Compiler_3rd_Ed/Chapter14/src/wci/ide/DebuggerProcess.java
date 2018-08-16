package wci.ide;

import java.io.*;
import java.awt.*;

import wci.ide.IDEControl;

import static wci.backend.interpreter.Debugger.*;
import static wci.ide.IDEControl.*;

/**
 * <h1>DebuggerProcess</h1>
 *
 * <p>The debugger process of the Pascal IDE.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class DebuggerProcess extends Thread
{
    private IDEControl control;                // the IDE control interface
    private Process process;                   // the debugger process
    private String sourceName;                 // source file name
    private PrintWriter toDebuggerStream;      // IDE to debugger I/O stream
    private DebuggerOutput debuggerOutput;     // debugger process output
    private boolean haveSyntaxErrors = false;  // true if have syntax errors
    private boolean debugging = false;         // true if debugging process I/O

    /**
     * Constructor.
     * @param control the IDE control.
     * @param sourceName the source file name.
     */
    public DebuggerProcess(IDEControl control, String sourceName)
    {
        this.control = control;
        this.sourceName = sourceName;
    }

    // The command that starts the debugger process.
    private static final String COMMAND =
                                "java -classpath classes Pascal execute %s %s";

    /**
     * Run the procecess.
     */
    public void run()
    {
        try {
            // Start the Pascal debugger process.
            String command = String.format(COMMAND, control.getSourcePath(),
                                                    control.getInputPath());
            process = Runtime.getRuntime().exec(command);

            // Capture the process's input stream.
            toDebuggerStream = new PrintWriter(process.getOutputStream());

            // Read and dispatch output text from the
            // debugger process for processing.
            debuggerOutput = new DebuggerOutput(process);
            dispatchDebuggerOutput();
        }
        catch(Exception ex) {
            ex.printStackTrace();
        }
    }

    /**
     * Kill the debugger process.
     */
    public void kill()
    {
        if (process != null) {
            process.destroy();
            process = null;
        }
    }

    /**
     * Write a command or runtime input text
     * to the debugger process's standard input.
     * @param text the command string or input text.
     */
    public void writeToDebuggerStandardInput(String text)
    {
        synchronized(DebuggerProcess.class) {
            toDebuggerStream.println(text);
            toDebuggerStream.flush();

            if (debugging) {
                System.err.println("Sent: '" + text + "'");
            }
        }
    }

    /**
     * Read and dispatch output text from the debugger process for processing.
     * @throws Exception if an error occurred.
     */
    private void dispatchDebuggerOutput()
        throws Exception
    {
        String text;

        // Loop to process debugger output text
        // which may contain embedded output tags.
        do {
            text = debuggerOutput.next();
            if (debugging) {
                System.out.println("Read: '" + text + "'");
            }

            int index;
            do {
                Thread.sleep(1);
                index = text.indexOf('!');

                // The debugger output text contains the ! character.
                // It may be the start of an output tag.
                if (index >= 0) {

                    // Add any preceding text to the console window.
                    if (index > 0) {
                        String precedingText = text.substring(0, index);
                        control.addToConsoleWindowOutput(precedingText);
                        text = text.substring(index);
                    }

                    // Yes, it was an output tag. Don't loop again.
                    if (processTag(text)) {
                        index = -1;
                    }

                    // No, it wasn't.
                    // Loop again to process the rest of the output text.
                    else {
                        control.addToConsoleWindowOutput("!");
                        text = text.substring(1);
                    }
                }

                // Send all the debugger output text to the console window.
                else {
                    control.addToConsoleWindowOutput(text);
                }
            } while (index >= 0);
        }
        while (!text.startsWith(INTERPRETER_TAG));
    }

    /**
     * Process a tag in the output text.
     * @param text the output text
     * @return true if processed a tag, else false.
     */
    private boolean processTag(String text)
    {
        // Listing line.
        if (text.startsWith(LISTING_TAG)) {
            control.addToDebugWindowListing(
                        text.substring(LISTING_TAG.length()));
            return true;
        }

        // Syntax error message.
        else if (text.startsWith(SYNTAX_TAG)) {
            String errorMessage = text.substring(SYNTAX_TAG.length());
            control.addToEditWindowErrors(errorMessage);
            haveSyntaxErrors = true;
            return true;
        }

        // Parser message.
        else if (text.startsWith(PARSER_TAG)) {
            control.setEditWindowMessage(text.substring(PARSER_TAG.length()),
                                         haveSyntaxErrors ? Color.RED
                                                          : Color.BLUE);

            if (!haveSyntaxErrors) {
                control.clearEditWindowErrors();
                control.setDebugWindowMessage("", Color.BLUE);
                control.showDebugWindow(sourceName);
                control.showCallStackWindow(sourceName);
                control.showConsoleWindow(sourceName);
            }
            else {
                control.setDebugWindowMessage("Fix syntax errors.",
                                             Color.RED);
                control.stopDebugWindow();
                control.disableConsoleWindowInput();
            }

            return true;
        }

        // Debugger at a source statement.
        else if (text.startsWith(DEBUGGER_AT_TAG)) {
            String lineNumber = text.substring(DEBUGGER_AT_TAG.length());
            control.setDebugWindowAtListingLine(
                        Integer.parseInt(lineNumber.trim()));
            control.setDebugWindowMessage(" ", Color.BLUE);
            return true;
        }

        // Debugger break at a source statement.
        else if (text.startsWith(DEBUGGER_BREAK_TAG)) {
            String lineNumber = text.substring(DEBUGGER_BREAK_TAG.length());
            control.breakDebugWindowAtListingLine(
                        Integer.parseInt(lineNumber.trim()));
            control.setDebugWindowMessage("Break at text " + lineNumber,
                                          Color.BLUE);
            return true;
        }

        // Debugger add a routine to the call stack.
        else if (text.startsWith(DEBUGGER_ROUTINE_TAG)) {
            String components[] = text.split(":");
            String level = components[1].trim();

            // Header.
            if (level.equals("-1")) {
                control.initializeCallStackWindow();
            }

            // Footer.
            else if (level.equals("-2")) {
                control.completeCallStackWindow();
            }

            // Routine name.
            else {
                String header = components[2].trim();
                control.addRoutineToCallStackWindow(level, header);
            }

            return true;
        }

        // Debugger add a local variable to the call stack.
        else if (text.startsWith(DEBUGGER_VARIABLE_TAG)) {
            text = text.substring(DEBUGGER_VARIABLE_TAG.length());

            int index = text.indexOf(":");
            String name = text.substring(0, index);
            String value = text.substring(index + 1);

            control.addVariableToCallStackWindow(name, value);
            return true;
        }

        // Interpreter message.
        else if (text.startsWith(INTERPRETER_TAG)) {
            control.setDebugWindowMessage(
                text.substring(INTERPRETER_TAG.length()), Color.BLUE);
            control.stopDebugWindow();
            control.disableConsoleWindowInput();
            return true;
        }
        else {
            return false;  // it wasn't an output tag
        }
    }

    /**
     * Output from the debugger.
     */
    private class DebuggerOutput
    {
        private static final int BUFFER_SIZE = 1024;

        private BufferedReader fromDebuggerStream; // debugger to IDE I/O stream
        private char buffer[];                     // output buffer
        private int start;                         // start of output line
        private int index;                         // index of \n or end of line
        private int prevIndex;                     // previous index
        private int length;                        // output text length

        /**
         * Constructor.
         * @param process the interpreter process.
         */
        private DebuggerOutput(Process process)
        {
            fromDebuggerStream = new BufferedReader(
                                     new InputStreamReader(
                                         process.getInputStream()));
            buffer = new char[BUFFER_SIZE];
            start = 0;
            length = 0;
        }

        /**
         * Get the next complete or partial output line.
         * @return the output line.
         * @throws Exception if an error occurred.
         */
        private String next()
            throws Exception
        {
            String output = "";

            // Loop to process output from the interpreter.
            for (;;) {
                Thread.sleep(1);
                index = findEol(prevIndex);

                // Found end of line: Return the line.
                if (index < length) {
                    output += new String(buffer, start, index - start + 1);
                    start = index + 1;
                    prevIndex = start;

                    if (debugging) {
                        System.err.println("Output: '" + output + "'");
                    }

                    return output;
                }

                // No end of line: Append to the current output.
                if (index > start) {
                    output += new String(buffer, start, index - start);
                }

                // Prepare to read again into the buffer.
                start = 0;
                length = 0;
                prevIndex = 0;

                // Read more output if available.
                if (fromDebuggerStream.ready()) {
                    length = readFromDebuggerStandardOutput();
                }

                // No more output: Return the current output.
                else {
                    if (debugging) {
                        System.err.println("Output: '" + output + "'");
                    }

                    return output;
                }
            }
        }

        /**
         * Read debugger status or runtime output
         * from the debugger's standard output.
         * @return the number of characters read.
         * @throws Exception if an error occurred.
         */
        private int readFromDebuggerStandardOutput()
            throws Exception
        {
            return fromDebuggerStream.read(buffer);
        }

        /**
         * Look for \n in the output.
         * @param index where to start looking.
         * @return the index of \n or the end of output.
         */
        private int findEol(int index)
        {
            while ((index < length) && (buffer[index] != '\n')) {
                ++index;
            }

            return index;
        }
    }
}
