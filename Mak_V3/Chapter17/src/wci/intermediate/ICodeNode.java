package wci.intermediate;

import java.util.ArrayList;

/**
 * <h1>ICodeNode</h1>
 *
 * <p>The interface for a node of the intermediate code.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public interface ICodeNode
{
    /**
     * Getter.
     * @return the node type.
     */
    public ICodeNodeType getType();

    /**
     * Return the parent of this node.
     * @return the parent node.
     */
    public ICodeNode getParent();

    /**
     * Set the type specification of this node.
     * @param typeSpec the type sprcification to set.
     */
    public void setTypeSpec(TypeSpec typeSpec);

    /**
     * Return the type specification of this node.
     * @return the type specification.
     */
    public TypeSpec getTypeSpec();

    /**
     * Add a child node.
     * @param node the child node.
     * @return the child node.
     */
    public ICodeNode addChild(ICodeNode node);

    /**
     * Return an array list of this node's children.
     * @return the array list of children.
     */
    public ArrayList<ICodeNode> getChildren();

    /**
     * Set a node attribute.
     * @param key the attribute key.
     * @param value the attribute value.
     */
    public void setAttribute(ICodeKey key, Object value);

    /**
     * Get the value of a node attribute.
     * @param key the attribute key.
     * @return the attribute value.
     */
    public Object getAttribute(ICodeKey key);

    /**
     * Make a copy of this node.
     * @return the copy.
     */
    public ICodeNode copy();
}
