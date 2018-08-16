package wci.ide.ideimpl;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

import wci.ide.IDEControl;

/**
 * <h1>ConsoleFrame</h1>
 *
 * <p>The console window of the Pascal IDE.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class ConsoleFrame
    extends JInternalFrame
    implements ActionListener
{
    private BorderLayout contentBorderLayout = new BorderLayout();
    private GridBagLayout inputGridBagLayout = new GridBagLayout();
    private JScrollPane scrollPane = new JScrollPane();
    private JTextArea outputArea = new JTextArea();
    private JPanel inputPanel = new JPanel();
    private JLabel inputLabel = new JLabel();
    private JTextField inputText = new JTextField();
    private JButton enterButton = new JButton();

    private IDEControl control;

    /**
     * Constructor.
     */
    public ConsoleFrame()
    {
        try {
            initGuiComponents();
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    /**
     * Constructor.
     * @param control the IDE control.
     */
    public ConsoleFrame(IDEControl control)
    {
        this();
        this.control = control;

        enterButton.addActionListener(this);
    }

    /**
     * Initialize the GUI components.
     * @throws Exception if an error occurred.
     */
    private void initGuiComponents()
        throws Exception
    {
        this.setIconifiable(true);
        this.setMaximizable(true);
        this.setResizable(true);
        this.getContentPane().setLayout(contentBorderLayout);
        this.getContentPane().add(scrollPane, BorderLayout.CENTER);
        this.getContentPane().add(inputPanel, java.awt.BorderLayout.SOUTH);
        inputPanel.setLayout(inputGridBagLayout);
        inputLabel.setText("Runtime input text:");
        enterButton.setText("Enter");
        scrollPane.getViewport().add(outputArea, null);
        outputArea.setFont(new java.awt.Font("Courier", Font.PLAIN, 14));
        outputArea.setDoubleBuffered(true);
        outputArea.setEditable(false);
        inputPanel.add(inputLabel,
                       new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0,
                                              GridBagConstraints.WEST,
                                              GridBagConstraints.NONE,
                                              new Insets(5, 5, 5, 0), 0, 0));
        inputPanel.add(inputText,
                       new GridBagConstraints(1, 0, 1, 1, 1.0, 0.0,
                                              GridBagConstraints.CENTER,
                                              GridBagConstraints.HORIZONTAL,
                                              new Insets(5, 5, 5, 0), 0, 0));
        inputPanel.add(enterButton,
                       new GridBagConstraints(2, 0, 1, 1, 0.0, 0.0,
                                              GridBagConstraints.EAST,
                                              GridBagConstraints.NONE,
                                              new Insets(5, 5, 5, 5), 0, 0));
    }

    /**
     * Initialize the console window.
     */
    public void initialize()
    {
        inputText.setEnabled(true);
        enterButton.setEnabled(true);
    }

    /**
     * Clear the output.
     */
    public void clearOutput()
    {
        outputArea.setText("");
    }

    /**
     * Add output text.
     * @param line the output text.
     */
    public void addToOutput(String text)
    {
        outputArea.append(text);
        outputArea.setEnabled(true);
        outputArea.setCaretPosition(outputArea.getText().length());
    }

    /**
     * Enable runtime input.
     */
    public void enableInput()
    {
        inputText.setEnabled(true);
        enterButton.setEnabled(true);
    }

    /**
     * Disable runtime input.
     */
    public void disableInput()
    {
        inputText.setEnabled(false);
        enterButton.setEnabled(false);
    }

    /**
     * Button event dispatcher.
     * @param event the button event.
     */
    public void actionPerformed(ActionEvent event)
    {
        Object button = event.getSource();

        if (button == enterButton) {
            enterAction();
        }
    }

    /**
     * Enter button event handler.
     */
    private void enterAction()
    {
        String text = inputText.getText() + "\n";

        addToOutput(text);
        inputText.setText("");
        control.sendToDebuggerProcess(text);
    }
}
