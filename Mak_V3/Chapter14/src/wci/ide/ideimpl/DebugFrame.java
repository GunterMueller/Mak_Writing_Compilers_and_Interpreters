package wci.ide.ideimpl;

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;
import java.awt.BorderLayout;

import wci.ide.IDEControl;

/**
 * <h1>DebugFrame</h1>
 *
 * <p>The debug window of the Pascal IDE.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class DebugFrame
    extends JInternalFrame
    implements ActionListener
{
    private BorderLayout contentBorderLayout = new BorderLayout();
    private BorderLayout messageBorderLayout = new BorderLayout();
    private TitledBorder messageTitledBorder;
    private GridBagLayout controlGridBagLayout = new GridBagLayout();
    private GridLayout buttonGridLayout = new GridLayout();
    private JScrollPane listingScrollPane = new JScrollPane();
    private JList listing = new JList();
    private JPanel controlPanel = new JPanel();
    private JPanel messagePanel = new JPanel();
    private JTextArea messageArea = new JTextArea();
    private JPanel buttonPanel = new JPanel();
    private JPanel filler1Panel = new JPanel();
    private JButton goButton = new JButton();
    private JButton quitButton = new JButton();
    private JButton singleStepButton = new JButton();
    private JButton autoStepButton = new JButton();
    private JButton slowerButton = new JButton();
    private JButton fasterButton = new JButton();

    private static int INIT_AUTO_STEP_WAIT_TIME = 200;

    private IDEControl control;
    private Thread autoStepper;

    private int currentLineNumber;
    private int autoStepWaitTime;

    private boolean programRunning = false;
    private boolean settingBreakpoint = false;
    private boolean autoStepping = false;
    private boolean atCommandPrompt = false;
    private boolean breakpointFlippable = false;

    private boolean savedSingleStepState;

    private Vector lineBuffer = new Vector();  // listing lines buffer

    /**
     * Constructor.
     */
    public DebugFrame()
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
    public DebugFrame(IDEControl control)
    {
        this();
        this.control = control;

        goButton.addActionListener(this);
        quitButton.addActionListener(this);
        singleStepButton.addActionListener(this);
        autoStepButton.addActionListener(this);
        slowerButton.addActionListener(this);
        fasterButton.addActionListener(this);
        listing.setCellRenderer(new ListingLineRenderer());

        // Mouse click listener for the listing.
        listing.addMouseListener(new MouseAdapter()
        {
            public void mouseClicked(MouseEvent event)
            {
                if (programRunning) {
                    settingBreakpoint = true;
                    toggleBreakpoint(listing.locationToIndex(event.getPoint()));
                    settingBreakpoint = false;
                }

                listing.clearSelection();
                selectListingLine(currentLineNumber);
            }
        });
    }

    /**
     * Initialize the GUI components.
     * @throws Exception if an error occurred.
     */
    private void initGuiComponents()
        throws Exception
    {
        messageTitledBorder = new TitledBorder(BorderFactory.createLineBorder(
            new Color(153, 153, 153), 2), "Interpreter messages");
        this.getContentPane().setLayout(contentBorderLayout);
        this.setIconifiable(true);
        this.setMaximizable(true);
        this.setResizable(true);
        this.getContentPane().add(listingScrollPane, BorderLayout.CENTER);
        this.getContentPane().add(controlPanel, BorderLayout.SOUTH);
        listing.setFont(new java.awt.Font("Courier", 0, 12));
        listing.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        controlPanel.setLayout(controlGridBagLayout);
        messagePanel.setLayout(messageBorderLayout);
        messagePanel.setBorder(messageTitledBorder);
        messagePanel.setMinimumSize(new Dimension(12, 50));
        messagePanel.setPreferredSize(new Dimension(12, 50));
        buttonGridLayout.setRows(2);
        buttonGridLayout.setColumns(3);
        buttonGridLayout.setHgap(5);
        buttonGridLayout.setVgap(5);
        messagePanel.add(messageArea, BorderLayout.CENTER);
        messageArea.setFont(new java.awt.Font("Dialog", 1, 12));
        messageArea.setForeground(Color.blue);
        messageArea.setEditable(false);
        messageArea.setText("");
        buttonPanel.setLayout(buttonGridLayout);
        buttonPanel.setBorder(BorderFactory.createLoweredBevelBorder());
        filler1Panel.setLayout(null);
        goButton.setEnabled(false);
        goButton.setText("Go");
        quitButton.setEnabled(false);
        quitButton.setText("Quit");
        singleStepButton.setEnabled(false);
        singleStepButton.setText("Single step");
        autoStepButton.setEnabled(false);
        autoStepButton.setText("Auto step");
        fasterButton.setEnabled(false);
        fasterButton.setText("Faster stepping");
        slowerButton.setEnabled(false);
        slowerButton.setText("Slower stepping");
        listingScrollPane.getViewport().add(listing, null);
        controlPanel.add(messagePanel,
                         new GridBagConstraints(0, 0, 4, 1, 1.0, 0.0,
                                                GridBagConstraints.CENTER,
                                                GridBagConstraints.HORIZONTAL,
                                                new Insets(5, 5, 0, 5), 0, 0));
        controlPanel.add(buttonPanel,
                         new GridBagConstraints(0, 1, 1, 1, 1.0, 0.0
                                                , GridBagConstraints.CENTER,
                                                GridBagConstraints.HORIZONTAL,
                                                new Insets(5, 5, 5, 5), 0, 0));
        buttonPanel.add(goButton, null);
        buttonPanel.add(singleStepButton, null);
        buttonPanel.add(slowerButton, null);
        buttonPanel.add(quitButton, null);
        buttonPanel.add(autoStepButton, null);
        buttonPanel.add(fasterButton, null);
    }

    /**
     * Initialize the control panel buttons.
     */
    private void initializeButtons()
    {
        goButton.setEnabled(true);
        quitButton.setEnabled(true);
        singleStepButton.setEnabled(savedSingleStepState = true);
        autoStepButton.setEnabled(true);
        slowerButton.setEnabled(false);
        fasterButton.setEnabled(false);
    }

    /**
     * Disable the control panel buttons.
     */
    private void disableButtons()
    {
        savedSingleStepState = singleStepButton.isEnabled();

        goButton.setEnabled(false);
        singleStepButton.setEnabled(false);
    }

    /**
     * Restore the control panel buttons.
     */
    private void restoreButtons()
    {
        goButton.setEnabled(true);
        singleStepButton.setEnabled(savedSingleStepState);
    }

    /**
     * Initialize the debugger window.
     */
    public void initialize()
    {
        programRunning = true;
        atCommandPrompt = false;
        autoStepping = false;
        autoStepWaitTime = INIT_AUTO_STEP_WAIT_TIME;

        initializeButtons();
    }

    /**
     * Clear the listing lines.
     */
    public void clearListing()
    {
        lineBuffer.clear();
        loadListing();
    }

    /**
     * Add a listing line.
     * @param line the line.
     */
    public void addListingLine(String line)
    {
        lineBuffer.addElement(new ListingLine(line, true));
        loadListing();
    }

    /**
     * Select a listing line.
     * @param lineNumber the line number.
     */
    public void selectListingLine(int lineNumber)
    {
        synchronized (DebugFrame.class) {
            currentLineNumber = lineNumber;
            int index = lineNumber - 1;

            listing.setSelectedIndex(index);
            listing.ensureIndexIsVisible(index);
        }
    }

    /**
     * Set the debugger to a listing line.
     * @param lineNumber the line number.
     */
    public void atListingLine(int lineNumber)
    {
        atCommandPrompt = true;
        breakpointFlippable = true;
        selectListingLine(lineNumber);
        restoreButtons();

        if (autoStepping) {
            goButton.setEnabled(false);
            autoStepButton.setEnabled(true);
            slowerButton.setEnabled(true);
            fasterButton.setEnabled(autoStepWaitTime > 0);
        }
    }

    /**
     * Set the debugger to break at a listing line.
     * @param lineNumber the line number.
     */
    public void breakAtListingLine(int lineNumber)
    {
        autoStepping = false;
        breakpointFlippable = true;
        selectListingLine(lineNumber);
        initializeButtons();
    }

    /**
     * Set the message.
     * @param message the message.
     * @param color the message color.
     */
    public void setMessage(String message, Color color)
    {
        messageArea.setForeground(color);
        messageArea.setText(message);
    }

    /**
     * Stop the debugger.
     */
    public void stop()
    {
        programRunning = false;
        autoStepping = false;

        goButton.setEnabled(false);
        singleStepButton.setEnabled(false);
        autoStepButton.setEnabled(false);
        slowerButton.setEnabled(false);
        fasterButton.setEnabled(false);
        quitButton.setEnabled(false);

        listing.repaint();
    }

    /**
     * Load the listing lines.
     */
    private void loadListing()
    {
        synchronized (DebugFrame.class) {
            listing.clearSelection();
            listing.setListData(lineBuffer);
        }
    }

    /**
     * Clear all the breakpoints.
     */
    private void clearAllBreakpoints()
    {
        ListModel model = listing.getModel();
        int size = model.getSize();

        for (int i = 0; i < size; ++i) {
            ListingLine line = (ListingLine) model.getElementAt(i);

            if (line.breakpoint) {
                line.breakpoint = false;

                int lineNumber = i + 1;
                control.sendToDebuggerProcess("unbreak " + lineNumber + ";");
            }
        }

        listing.repaint();
    }

    /**
     * Toggle the breakpoint on the listing line the mouse clicked on.
     * @param index the source line index.
     */
    private void toggleBreakpoint(int index)
    {
        ListingLine line = (ListingLine) listing.getModel().getElementAt(index);

        if (breakpointFlippable && line.isBreakable()) {
            line.toggleBreakpoint();

            String command = line.isBreakpoint() ? "break " : "unbreak ";
            int lineNumber = index + 1;

            // Execute the break or unbreak command.
            control.sendToDebuggerProcess(command + lineNumber + ";");
        }
    }

    /**
     * Button event dispatcher.
     * @param event the button event.
     */
    public void actionPerformed(ActionEvent event)
    {
        Object button = event.getSource();

        if (button == goButton) {
            goAction();
        }
        else if (button == quitButton) {
            quitAction();
        }
        else if (button == singleStepButton) {
            singleStepAction();
        }
        else if (button == autoStepButton) {
            autoStepAction();
        }
        else if (button == slowerButton) {
            slowerAction();
        }
        else if (button == fasterButton) {
            fasterAction();
        }
    }

    /**
     * Go button event handler.
     */
    private void goAction()
    {
        programRunning = true;
        disableButtons();
        breakpointFlippable = false;

        control.sendToDebuggerProcess("go;");
        control.enableConsoleWindowInput();
    }

    /**
     * Quit button event handler.
     */
    private void quitAction()
    {
        programRunning = false;
        clearAllBreakpoints();

        goButton.setEnabled(false);
        quitButton.setEnabled(false);
        singleStepButton.setEnabled(false);
        autoStepButton.setEnabled(false);
        slowerButton.setEnabled(false);
        fasterButton.setEnabled(false);

        control.sendToDebuggerProcess("quit;");
        control.stopDebuggerProcess();
    }

    /**
     * Single step button event handler.
     */
    private void singleStepAction()
    {
        disableButtons();
        breakpointFlippable = false;

        control.sendToDebuggerProcess("step;");
        control.sendToDebuggerProcess("stack;");
        control.enableConsoleWindowInput();
    }

    /**
     * Autostep button event handler.
     */
    private void autoStepAction()
    {
        // Stop autostepping.
        if (autoStepping) {
            autoStepping = false;
        }

        // Start autostepping.
        else {
            autoStepper = new AutoStepper();

            goButton.setEnabled(false);
            singleStepButton.setEnabled(savedSingleStepState = false);
            autoStepButton.setText("Stop stepping");
            autoStepButton.setEnabled(true);
            slowerButton.setEnabled(true);
            fasterButton.setEnabled(autoStepWaitTime > 0);

            autoStepping = true;
            autoStepper.start();
        }
    }

    /**
     * Slower stepping button event handler.
     */
    private void slowerAction()
    {
        autoStepWaitTime += 50;
        fasterButton.setEnabled(true);
    }

    /**
     * Faster stepping button event handler.
     */
    private void fasterAction()
    {
        if ((autoStepWaitTime -= 50) <= 0) {
            autoStepWaitTime = 0;
        }

        fasterButton.setEnabled(autoStepWaitTime > 0);
    }

    /**
     * Autostepper thread.
     */
    private class AutoStepper extends Thread
    {
        public void run()
        {
            // Loop to perform a single-step action for each command prompt
            // until autoStepping is false. Sleep between each single step.
            do {
                try {
                    if (atCommandPrompt) {
                        atCommandPrompt = false;

                        singleStepAction();
                        Thread.sleep(autoStepWaitTime);
                    }
                    else {
                        Thread.yield();
                    }
                }
                catch (Exception ignore) {}
            }
            while (autoStepping);

            goButton.setEnabled(programRunning);
            quitButton.setEnabled(programRunning);
            singleStepButton.setEnabled(savedSingleStepState = programRunning);
            autoStepButton.setText("Auto step");
            autoStepButton.setEnabled(programRunning);
            slowerButton.setEnabled(false);
            fasterButton.setEnabled(false);
        }
    }

    /**
     * The listing line.
     */
    private class ListingLine
    {
        private String text;
        private boolean breakable = true;
        private boolean breakpoint = false;

        /**
         * Constructor.
         * @param text the line text.
         * @param breakable true if can set a breakpoint here, else false.
         */
        ListingLine(String text, boolean breakable)
        {
            this.breakable = breakable;
            this.text = text;
        }

        /**
         * Getter.
         * @return the line text.
         */
        String getText()
        {
            return text;
        }

        /**
         * @return true if breakable, else false.
         */
        boolean isBreakable()
        {
            return breakable;
        }

        /**
         * @return true if breakpoint set, else false.
         */
        boolean isBreakpoint()
        {
            return breakpoint;
        }

        /**
         * Toggle the breakpoint.
         */
        void toggleBreakpoint()
        {
            this.breakpoint = !this.breakpoint;
        }
    }

    private Font listingFont = new Font("Courier", Font.PLAIN, 12);
    private ImageIcon nobreakIcon = new ImageIcon("nobreak.gif");
    private ImageIcon breakableIcon = new ImageIcon("breakable.gif");
    private ImageIcon breakPointIcon = new ImageIcon("breakpoint.gif");

    /**
     * Listing line renderer.
     */
    private class ListingLineRenderer
        extends JLabel
        implements ListCellRenderer
    {
        /**
         * Constructor.
         */
        ListingLineRenderer()
        {
            setOpaque(true);
        }

        /**
         * Return the renderer component.
         * @param list the JList object.
         * @param value the listing line object.
         * @param index the list index.
         * @param isSelected true if selected, else false.
         * @param cellHasFocus true if has focus, else false.
         * @return the renderer component.
         */
        public Component getListCellRendererComponent(JList list,
                                                      Object value,
                                                      int index,
                                                      boolean isSelected,
                                                      boolean cellHasFocus)
        {
            ListingLine line = (ListingLine) value;
            ImageIcon icon =   !line.isBreakable() ? nobreakIcon
                             : !programRunning     ? nobreakIcon
                             : line.isBreakpoint() ? breakPointIcon
                             :                       breakableIcon;

            setIcon(icon);
            setFont(listingFont);
            setText(line.getText());

            setForeground(line.isBreakpoint() ? Color.RED : Color.BLACK);
            setBackground(isSelected && !settingBreakpoint
                          ? Color.LIGHT_GRAY : Color.WHITE);

            return this;
        }
    }
}
