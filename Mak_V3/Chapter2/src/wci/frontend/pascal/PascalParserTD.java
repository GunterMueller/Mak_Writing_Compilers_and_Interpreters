package wci.frontend.pascal;

import wci.frontend.*;
import wci.message.Message;

import static wci.message.MessageType.PARSER_SUMMARY;

/**
 * <h1>PascalParserTD</h1>
 *
 * <p>The top-down Pascal parser.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class PascalParserTD extends Parser
{
    /**
     * Constructor.
     * @param scanner the scanner to be used with this parser.
     */
    public PascalParserTD(Scanner scanner)
    {
        super(scanner);
    }

    /**
     * Parse a Pascal source program and generate the symbol table
     * and the intermediate code.
     */
    public void parse()
        throws Exception
    {
        Token token;
        long startTime = System.currentTimeMillis();

        while (!((token = nextToken()) instanceof EofToken)) {}

        // Send the parser summary message.
        float elapsedTime = (System.currentTimeMillis() - startTime)/1000f;
        sendMessage(new Message(PARSER_SUMMARY,
                                new Number[] {token.getLineNumber(),
                                              getErrorCount(),
                                              elapsedTime}));
    }

    /**
     * Return the number of syntax errors found by the parser.
     * @return the error count.
     */
    public int getErrorCount()
    {
        return 0;
    }
}
