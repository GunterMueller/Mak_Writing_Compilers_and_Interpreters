package wci.ide.ideimpl;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.tree.*;

import wci.ide.IDEControl;

import static javax.swing.tree.TreeSelectionModel.*;

/**
 * <h1>CallStackFrame</h1>
 *
 * <p>The call stack window of the Pascal IDE.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class CallStackFrame
    extends JInternalFrame
    implements ActionListener, TreeSelectionListener
{
    private BorderLayout contentBorderLayout = new BorderLayout();
    private GridBagLayout controlGridBagLayout = new GridBagLayout();
    private JScrollPane scrollPane = new JScrollPane();
    private JPanel controlPanel = new JPanel();
    private JTree callTree = new JTree();
    private JLabel nameLabel = new JLabel();
    private JTextField nameText = new JTextField();
    private JLabel valueLabel = new JLabel();
    private JTextField valueText = new JTextField();
    private JButton changeButton = new JButton();

    private IDEControl control;
    private DefaultTreeModel treeModel;
    private DefaultMutableTreeNode root;
    private DefaultMutableTreeNode currentRoutineNode;
    private int routineNodeIndex;
    private int variableNodeIndex;

    /**
     * Constructor.
     */
    public CallStackFrame()
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
    public CallStackFrame(IDEControl control)
    {
        this();
        this.control = control;

        changeButton.addActionListener(this);
    }

    /**
     * Initialize the GUI components.
     * @throws Exception if an error occurred.
     */
    void initGuiComponents()
        throws Exception
    {
        this.setIconifiable(true);
        this.setMaximizable(true);
        this.setResizable(true);
        this.getContentPane().setLayout(contentBorderLayout);
        this.getContentPane().add(scrollPane, BorderLayout.CENTER);
        this.getContentPane().add(controlPanel, java.awt.BorderLayout.SOUTH);
        scrollPane.getViewport().add(callTree, null);
        callTree.setFont(new java.awt.Font("Courier", 0, 12));
        callTree.setDoubleBuffered(true);
        callTree.setEditable(false);
        callTree.setEnabled(true);
        nameLabel.setText("Name:");
        nameText.setEditable(false);
        nameText.setText("");
        valueLabel.setText("Value:");
        valueText.setMinimumSize(new Dimension(45, 20));
        valueText.setPreferredSize(new Dimension(45, 20));
        valueText.setEditable(false);
        valueText.setText("");
        changeButton.setEnabled(false);
        changeButton.setText("Change");
        controlPanel.setBorder(BorderFactory.createLoweredBevelBorder());
        controlPanel.setLayout(controlGridBagLayout);
        controlPanel.add(nameLabel,
                         new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0,
                                                GridBagConstraints.EAST,
                                                GridBagConstraints.BOTH,
                                                new Insets(5, 5, 0, 0), 0, 0));
        controlPanel.add(nameText,
                         new GridBagConstraints(1, 0, 2, 1, 1.0, 0.0,
                                                GridBagConstraints.CENTER,
                                                GridBagConstraints.HORIZONTAL,
                                                new Insets(5, 5, 0, 5), 2, 0));
        controlPanel.add(valueLabel,
                         new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0,
                                                GridBagConstraints.EAST,
                                                GridBagConstraints.BOTH,
                                                new Insets(5, 5, 5, 0), 0, 0));
        controlPanel.add(valueText,
                         new GridBagConstraints(1, 1, 1, 1, 1.0, 0.0,
                                                GridBagConstraints.CENTER,
                                                GridBagConstraints.HORIZONTAL,
                                                new Insets(5, 5, 5, 0), 0, 0));
        controlPanel.add(changeButton,
                         new GridBagConstraints(2, 1, 1, 1, 0.0, 0.0,
                                                GridBagConstraints.WEST,
                                                GridBagConstraints.BOTH,
                                                new Insets(5, 5, 5, 5), 0, 0));
    }

    /**
     * Initialize the call stack tree.
     */
    public void initialize()
    {
        // Remove the old tree from the scroll pane.
        if (callTree != null) {
            scrollPane.getViewport().remove(callTree);
        }

        // Create a new tree.
        root = new DefaultMutableTreeNode("Most recent invocation on top.");
        treeModel = new DefaultTreeModel(root);
        callTree = new JTree(treeModel);
        callTree.getSelectionModel().setSelectionMode(SINGLE_TREE_SELECTION);
        callTree.addTreeSelectionListener(this);

        nameText.setText("");
        valueText.setText("");
        changeButton.setEnabled(false);

        routineNodeIndex = 0;
    }

    /**
     * Add an invoked routine to the call stack tree.
     * @param level the routine's nesting level.
     * @param header the routine's header.
     */
    public void addRoutine(String level, String header)
    {
        RoutineNodeInfo info = new RoutineNodeInfo(level, header);
        currentRoutineNode = new DefaultMutableTreeNode(info);

        treeModel.insertNodeInto(currentRoutineNode, root, routineNodeIndex++);
        variableNodeIndex = 0;
    }

    /**
     * Add a local variable to the call stack tree.
     * @param name the variable's name.
     * @param value the variable's value.
     */
    public void addVariable(String name, String value)
    {
        VariableNodeInfo info = new VariableNodeInfo(name, value);
        DefaultMutableTreeNode variableNode = new DefaultMutableTreeNode(info);

        treeModel.insertNodeInto(variableNode, currentRoutineNode,
                                 variableNodeIndex++);
    }

    /**
     * Complete the call stack tree.
     */
    public void complete()
    {
        // Expand all the nodes.
        for (int i = 0; i < callTree.getRowCount(); ++i) {
            callTree.expandRow(i);
        }

        // Insert the new tree into the scroll pane.
        scrollPane.getViewport().add(callTree, null);
    }

    /**
     * Button event dispatcher.
     * @param event the button event.
     */
    public void actionPerformed(ActionEvent event)
    {
        Object button = event.getSource();

        if (button == changeButton) {
            changeButtonAction();
        }
    }

    /**
     * Change button event handler.
     */
    private void changeButtonAction()
    {
        control.sendToDebuggerProcess("assign " + nameText.getText() +
                              " " + valueText.getText() + ";");
    }

    /**
     * Tree selection event handler.
     * @param event the selection event.
     */
    public void valueChanged(TreeSelectionEvent event)
    {
        TreePath path = event.getPath();
        DefaultMutableTreeNode node =
            (DefaultMutableTreeNode) path.getPath()[path.getPathCount() - 1];
        Object info = node.getUserObject();

        // Only operate on variable nodes.
        if (info instanceof VariableNodeInfo) {
            nameText.setText(((VariableNodeInfo) info).name);
            valueText.setText(((VariableNodeInfo) info).value);

            // Allow changing only the values of scalar variables.
            char firstChar = ((VariableNodeInfo) info).value.charAt(0);
            boolean changeable = (firstChar != '[') && (firstChar != '{');
            valueText.setEditable(changeable);
            changeButton.setEnabled(changeable);
        }
    }

    /**
     * Info for a routine tree node.
     */
    private class RoutineNodeInfo
    {
        private String level;   // routine's nesting level
        private String header;  // routine's header string

        /**
         * Constructor.
         * @param level the routine's nesting level.
         * @param header the routine's header string.
         */
        private RoutineNodeInfo(String level, String header)
        {
            this.level = level;
            this.header = header;
        }

        /**
         * @return the node label.
         */
        public String toString()
        {
            return level + " " + header;
        }
    }

    /**
     * Info for a variable tree node.
     */
    private class VariableNodeInfo
    {
        private String name;   // variable's name
        private String value;  // variable's value

        /**
         * Constructor.
         * @param name the variable's name.
         * @param value the variable's value.
         */
        private VariableNodeInfo(String name, String value)
        {
            this.name = name;
            this.value = value;
        }

        /**
         * @return the node label.
         */
        public String toString()
        {
            return name + ": " + value;
        }
    }
}
