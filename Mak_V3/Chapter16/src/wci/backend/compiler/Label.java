package wci.backend.compiler;

import wci.intermediate.*;

/**
 * <h1>Label</h1>
 *
 * <p>Jasmin instruction label.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class Label
{
    private static int index = 0;  // index for generating label strings
    private String label;          // the label string

    /**
     * Constructor.
     */
    private Label()
    {
        this.label = "L" + String.format("%03d", ++index);
    }

    /**
     * @return a new instruction label.
     */
    public static Label newLabel()
    {
        return new Label();
    }

    /**
     * @return the label string.
     */
    public String toString()
    {
        return this.label;
    }
}
