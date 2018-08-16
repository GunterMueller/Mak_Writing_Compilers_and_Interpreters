package wci.intermediate.icodeimpl;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;

import wci.intermediate.*;

/**
 * <h1>ICodeNodeImpl</h1>
 *
 * <p>An implementation of a node of the intermediate code.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class ICodeNodeImpl
    extends HashMap<ICodeKey, Object>
    implements ICodeNode
{
    private ICodeNodeType type;             // node type
    private ICodeNode parent;               // parent node
    private ArrayList<ICodeNode> children;  // children array list

    /**
     * Constructor.
     * @param type the node type whose name will be the name of this node.
     */
    public ICodeNodeImpl(ICodeNodeType type)
    {
        this.type = type;
        this.parent = null;
        this.children = new ArrayList<ICodeNode>();
    }

    /**
     * Getter.
     * @return the node type.
     */
    public ICodeNodeType getType()
    {
        return type;
    }

    /**
     * Return the parent of this node.
     * @return the parent node.
     */
    public ICodeNode getParent()
    {
        return parent;
    }

    /**
     * Add a child node.
     * @param node the child node. Not added if null.
     * @return the child node.
     */
    public ICodeNode addChild(ICodeNode node)
    {
        if (node != null) {
            children.add(node);
            ((ICodeNodeImpl) node).parent = this;
        }

        return node;
    }

    /**
     * Return an array list of this node's children.
     * @return the array list of children.
     */
    public ArrayList<ICodeNode> getChildren()
    {
        return children;
    }

    /**
     * Set a node attribute.
     * @param key the attribute key.
     * @param value the attribute value.
     */
    public void setAttribute(ICodeKey key, Object value)
    {
        put(key, value);
    }

    /**
     * Get the value of a node attribute.
     * @param key the attribute key.
     * @return the attribute value.
     */
    public Object getAttribute(ICodeKey key)
    {
        return get(key);
    }

    /**
     * Make a copy of this node.
     * @return the copy.
     */
    public ICodeNode copy()
    {
        // Create a copy with the same type.
        ICodeNodeImpl copy =
            (ICodeNodeImpl) ICodeFactory.createICodeNode(type);

        Set<Map.Entry<ICodeKey, Object>> attributes = entrySet();
        Iterator<Map.Entry<ICodeKey, Object>> it = attributes.iterator();

        // Copy attributes
        while (it.hasNext()) {
            Map.Entry<ICodeKey, Object> attribute = it.next();
            copy.put(attribute.getKey(), attribute.getValue());
        }

        return copy;
    }

    public String toString()
    {
        return type.toString();
    }
}
