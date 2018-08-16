package wci.intermediate.icodeimpl;

import wci.intermediate.ICode;
import wci.intermediate.ICodeNode;

/**
 * <h1>ICodeImpl</h1>
 *
 * <p>An implementation of the intermediate code as a parse tree.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class ICodeImpl implements ICode
{
    private ICodeNode root;  // root node

    /**
     * Set and return the root node.
     * @param node the node to set as root.
     * @return the root node.
     */
    public ICodeNode setRoot(ICodeNode node)
    {
        root = node;
        return root;
    }

    /**
     * Get the root node.
     * @return the root node.
     */
    public ICodeNode getRoot()
    {
        return root;
    }
}
