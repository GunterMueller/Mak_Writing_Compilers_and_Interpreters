package wci.ide.ideimpl;

import java.io.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import java.awt.Dimension;
import java.awt.BorderLayout;

import wci.ide.IDEControl;

/**
 * <h1>EditFrame</h1>
 *
 * <p>The edit window of the Pascal IDE.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class EditFrame
    extends JInternalFrame
    implements ActionListener, CaretListener
{
    private TitledBorder parserMessagesBorder;
    private TitledBorder syntaxMessagesBorder;
    private BorderLayout contentBorderLayout = new BorderLayout();
    private BorderLayout parserMessagesBorderLayout = new BorderLayout();
    private GridBagLayout controlGridBagLayout = new GridBagLayout();
    private GridBagLayout positionGridBagLayout = new GridBagLayout();
    private JPanel positionPanel = new JPanel();
    private JPanel parserPanel = new JPanel();
    private JPanel controlPanel = new JPanel();
    private JSplitPane splitPane = new JSplitPane();
    private JScrollPane sourceScrollPane = new JScrollPane();
    private JScrollPane syntaxScrollPane = new JScrollPane();
    private JTextArea sourceArea = new JTextArea();
    private JTextArea parserTextArea = new JTextArea();
    private JLabel sourceFileLabel = new JLabel();
    private JTextField sourcePathText = new JTextField();
    private JButton browseSourceButton = new JButton();
    private JButton saveSourceButton = new JButton();
    private JLabel inputFileLabel = new JLabel();
    private JTextField inputPathText = new JTextField();
    private JButton browseInputButton = new JButton();
    private JButton clearInputButton = new JButton();
    private JLabel lineLabel = new JLabel();
    private JTextField lineText = new JTextField();
    private JLabel columnLabel = new JLabel();
    private JTextField columnText = new JTextField();
    private JButton runProgramButton = new JButton();
    private JList syntaxList = new JList();

    private IDEControl control;

    private String sourceName;
    private Vector syntaxErrors;

    /**
     * Constructor.
     */
    public EditFrame()
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
    public EditFrame(IDEControl control)
    {
        this();
        this.control = control;

        sourceArea.addCaretListener(this);
        browseSourceButton.addActionListener(this);
        saveSourceButton.addActionListener(this);
        browseInputButton.addActionListener(this);
        clearInputButton.addActionListener(this);
        runProgramButton.addActionListener(this);

        // Mouse click listener for the syntax errors.
        syntaxList.addMouseListener(new MouseAdapter()
        {
            public void mouseClicked(MouseEvent event)
            {
                highlightError(syntaxList.locationToIndex(event.getPoint()));
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
        parserMessagesBorder = new TitledBorder(
            BorderFactory.createLineBorder(new Color(153, 153, 153), 2),
            "Parser messages");
        syntaxMessagesBorder = new TitledBorder(
            BorderFactory.createLineBorder(new Color(153, 153, 153), 2),
            "Syntax error messages (click message to select source line)");
        this.setIconifiable(true);
        this.setMaximizable(true);
        this.setResizable(true);

        this.getContentPane().setLayout(contentBorderLayout);
        this.getContentPane().add(splitPane, BorderLayout.CENTER);

        splitPane.setOrientation(JSplitPane.VERTICAL_SPLIT);
        splitPane.setDividerLocation(300);
        splitPane.add(sourceScrollPane, JSplitPane.TOP);
        splitPane.add(controlPanel, JSplitPane.BOTTOM);

        sourceScrollPane.getViewport().add(sourceArea, null);
        sourceArea.setFont(new java.awt.Font("Courier", 0, 12));

        controlPanel.setLayout(controlGridBagLayout);
        controlPanel.setBorder(BorderFactory.createLoweredBevelBorder());
        sourceFileLabel.setText("Source file:");
        sourcePathText.setEditable(false);
        browseSourceButton.setSelected(false);
        browseSourceButton.setText("Browse ...");
        saveSourceButton.setEnabled(false);
        saveSourceButton.setText("Save file");
        inputFileLabel.setText("Input file:");
        inputPathText.setEditable(false);
        browseInputButton.setText("Browse ...");
        clearInputButton.setEnabled(false);
        clearInputButton.setText("Clear");

        positionPanel.setLayout(positionGridBagLayout);
        lineLabel.setText("Line:");
        lineText.setMinimumSize(new Dimension(35, 20));
        lineText.setPreferredSize(new Dimension(35, 20));
        lineText.setEditable(false);
        columnLabel.setText("Column:");
        columnText.setMinimumSize(new Dimension(20, 20));
        columnText.setPreferredSize(new Dimension(20, 20));
        columnText.setEditable(false);
        columnText.setText("");

        runProgramButton.setEnabled(false);
        runProgramButton.setText("Run program");

        parserPanel.setLayout(parserMessagesBorderLayout);
        parserPanel.setBorder(parserMessagesBorder);
        parserPanel.setMinimumSize(new Dimension(12, 50));
        parserPanel.setPreferredSize(new Dimension(12, 50));
        parserPanel.add(parserTextArea, BorderLayout.CENTER);
        parserTextArea.setFont(new java.awt.Font("Dialog", 1, 12));
        parserTextArea.setForeground(Color.blue);
        parserTextArea.setEditable(false);
        parserTextArea.setText(" ");

        syntaxScrollPane.setBorder(syntaxMessagesBorder);
        syntaxScrollPane.setMinimumSize(new Dimension(33, 90));
        syntaxScrollPane.setPreferredSize(new Dimension(60, 90));
        syntaxScrollPane.getViewport().add(syntaxList, null);

        syntaxList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);

        controlPanel.add(sourceFileLabel,
                         new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0,
                                                GridBagConstraints.WEST,
                                                GridBagConstraints.NONE,
                                                new Insets(5, 5, 0, 0), 0, 0));
        controlPanel.add(sourcePathText,
                         new GridBagConstraints(1, 0, 2, 1, 1.0, 0.0,
                                                GridBagConstraints.CENTER,
                                                GridBagConstraints.HORIZONTAL,
                                                new Insets(5, 5, 0, 0), 0, 0));
        controlPanel.add(browseSourceButton,
                         new GridBagConstraints(3, 0, 1, 1, 0.0, 0.0,
                                                GridBagConstraints.CENTER,
                                                GridBagConstraints.HORIZONTAL,
                                                new Insets(5, 5, 0, 0), 0, 0));
        controlPanel.add(saveSourceButton,
                         new GridBagConstraints(4, 0, 1, 1, 0.0, 0.0,
                                                GridBagConstraints.CENTER,
                                                GridBagConstraints.HORIZONTAL,
                                                new Insets(5, 5, 0, 5), 0, 0));
        controlPanel.add(inputFileLabel,
                         new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0,
                                                GridBagConstraints.WEST,
                                                GridBagConstraints.NONE,
                                                new Insets(5, 5, 0, 0), 0, 0));
        controlPanel.add(inputPathText,
                         new GridBagConstraints(1, 1, 2, 1, 0.0, 0.0,
                                                GridBagConstraints.CENTER,
                                                GridBagConstraints.HORIZONTAL,
                                                new Insets(5, 5, 0, 0), 0, 0));
        controlPanel.add(browseInputButton,
                         new GridBagConstraints(3, 1, 1, 1, 0.0, 0.0,
                                                GridBagConstraints.CENTER,
                                                GridBagConstraints.HORIZONTAL,
                                                new Insets(5, 5, 0, 0), 0, 0));
        controlPanel.add(clearInputButton,
                         new GridBagConstraints(4, 1, 1, 1, 0.0, 0.0,
                                                GridBagConstraints.CENTER,
                                                GridBagConstraints.HORIZONTAL,
                                                new Insets(5, 5, 0, 5), 0, 0));
        controlPanel.add(positionPanel,
                         new GridBagConstraints(0, 2, 2, 1, 0.0, 0.0,
                                                GridBagConstraints.CENTER,
                                                GridBagConstraints.NONE,
                                                new Insets(5, 5, 0, 0), 0, 0));
        controlPanel.add(runProgramButton,
                         new GridBagConstraints(3, 2, 2, 1, 0.0, 0.0,
                                                GridBagConstraints.EAST,
                                                GridBagConstraints.NONE,
                                                new Insets(5, 5, 0, 5), 0, 0));
        controlPanel.add(parserPanel,
                         new GridBagConstraints(0, 3, 5, 1, 1.0, 0.0,
                                                GridBagConstraints.CENTER,
                                                GridBagConstraints.HORIZONTAL,
                                                new Insets(5, 5, 0, 5), 0, 0));
        controlPanel.add(syntaxScrollPane,
                         new GridBagConstraints(0, 4, 5, 1, 1.0, 1.0,
                                                GridBagConstraints.CENTER,
                                                GridBagConstraints.BOTH,
                                                new Insets(0, 5, 5, 5), 0, 0));

        positionPanel.add(lineLabel,
                          new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0,
                                                 GridBagConstraints.EAST,
                                                 GridBagConstraints.NONE,
                                                 new Insets(0, 0, 0, 0), 0, 0));
        positionPanel.add(lineText,
                          new GridBagConstraints(1, 0, 1, 1, 0.0, 0.0,
                                                 GridBagConstraints.EAST,
                                                 GridBagConstraints.NONE,
                                                 new Insets(0, 5, 0, 0), 0, 0));

        positionPanel.add(columnLabel,
                          new GridBagConstraints(2, 0, 1, 1, 0.0, 0.0,
                                                 GridBagConstraints.EAST,
                                                 GridBagConstraints.NONE,
                                                 new Insets(0, 15, 0, 0),
                                                 0, 0));
        positionPanel.add(columnText,
                          new GridBagConstraints(3, 0, 1, 1, 0.0, 0.0,
                                                 GridBagConstraints.EAST,
                                                 GridBagConstraints.NONE,
                                                 new Insets(0, 5, 0, 0), 0, 0));
    }

    /**
     * Position the caret in the source area.
     * @param event the caret event.
     */
    public void caretUpdate(CaretEvent event)
    {
        int dot = event.getDot();

        try {
            int line = sourceArea.getLineOfOffset(dot);
            lineText.setText(Integer.toString(line + 1));
            int column = dot - sourceArea.getLineStartOffset(line);
            columnText.setText(Integer.toString(column + 1));
        }
        catch(Exception ex) {
            ex.printStackTrace();
        }
    }

    /**
     * Button event dispatcher.
     * @param event the button event.
     */
    public void actionPerformed(ActionEvent event)
    {
        Object button = event.getSource();

        if (button == browseSourceButton) {
            browseSourceAction();
        }
        else if (button == saveSourceButton) {
            saveSourceAction();
        }
        else if (button == browseInputButton) {
            browseInputAction();
        }
        else if (button == clearInputButton) {
            clearInputAction();
        }
        else if (button == runProgramButton) {
            runProgramAction();
        }
    }

    /**
     * Browse source button event handler.
     */
    private void browseSourceAction()
    {
        IDEFileChooser chooser =
            new IDEFileChooser(System.getProperty("user.dir"), null,
                               new IdeFileFilter(new String[] {".pas"},
                                                 "Pascal files (*.pas)"));
        File file = chooser.choose(sourcePathText, this);

        if (file != null) {
            sourceName = file.getName();
            control.setSourcePath(file.getPath());

            setTitle("EDIT: " + sourceName);
            sourceArea.setText(null);

            BufferedReader sourceFile = null;

            try {
                sourceFile = new BufferedReader(new FileReader(file));
                String line;

                while ((line = sourceFile.readLine()) != null) {
                    sourceArea.append(line + "\n");
                }

                sourceArea.setCaretPosition(0);
                runProgramButton.setEnabled(true);
                saveSourceButton.setEnabled(true);
            }
            catch (Exception ex) {
                ex.printStackTrace();
            }
            finally {
                try {
                    sourceFile.close();
                }
                catch (Exception ignore) {}
            }
        }
    }

    /**
     * Save source button event handler.
     */
    private void saveSourceAction()
    {
        BufferedWriter sourceFile = null;

        try {
            sourceFile = new BufferedWriter(
                             new FileWriter(control.getSourcePath(), false));
            sourceFile.write(sourceArea.getText());
        }
        catch (IOException ex) {
            ex.printStackTrace();
        }
        finally {
            if (sourceFile != null) {
                try {
                    sourceFile.close();
                }
                catch (Exception ignore) {}
            }
        }
    }

    /**
     * Browse input button event handler.
     */
    private void browseInputAction()
    {
        IDEFileChooser chooser =
            new IDEFileChooser(System.getProperty("user.dir"), null,
                               new IdeFileFilter(
                                   new String[] {".in", "*.txt", "*.dat"},
                                   "Input files (*.in, *.txt, *.dat)"));
        File file = chooser.choose(inputPathText, this);

        if (file != null) {
            control.setInputPath(file.getPath());
            clearInputButton.setEnabled(true);
        }
    }

    /**
     * Clear input button event handler.
     */
    private void clearInputAction()
    {
        inputPathText.setText("");
        clearInputButton.setEnabled(false);
        control.setInputPath(null);
    }

    /**
     * Run program button event handler.
     */
    private void runProgramAction()
    {
        syntaxErrors = new Vector();

        saveSourceAction();
        setMessage("", Color.BLUE);

        control.clearEditWindowErrors();
        control.clearDebugWindowListing();
        control.clearConsoleWindowOutput();

        control.stopDebuggerProcess();
        control.startDebuggerProcess(sourceName);
    }

    /**
     * Set the editor message.
     * @param message the message.
     * @param color the message color.
     */
    public void setMessage(String message, Color color)
    {
        parserTextArea.setForeground(color);
        parserTextArea.setText(message);
    }

    /**
     * Clear the syntax errors.
     */
    public void clearEditWindowErrors()
    {
        syntaxErrors.clear();
        syntaxList.setListData(syntaxErrors);
    }

    /**
     * Add a syntax error message.
     * @param line the error message.
     */
    public void addError(String line)
    {
        syntaxErrors.addElement(line);
        syntaxList.setListData(syntaxErrors);
    }

    /**
     * Highlight a source line containing a syntax error.
     * @param index the syntax error message index.
     */
    private void highlightError(int index)
    {
        String message = (String) syntaxErrors.elementAt(index);
        int i = message.indexOf(":");

        // Extract the source line number from the syntax error message.
        if ((i != -1) && (i < 10)) {
            int lineNumber = Integer.parseInt(message.substring(0,i).trim()) -1;

            // Highlight the source line.
            try {
                int start = sourceArea.getLineStartOffset(lineNumber);
                int end = sourceArea.getLineEndOffset(lineNumber);

                sourceArea.requestFocus();
                sourceArea.setSelectionStart(start);
                sourceArea.setSelectionEnd(end - 1);
            }
            catch (Exception ex) {
                ex.printStackTrace();
            }
        }
    }
}
