package wci.message;

/**
 * <h1>MessageProducer</h1>
 *
 * <p>All classes that produce messages must implement this interface.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public interface MessageProducer
{
    /**
     * Add a listener to the listener list.
     * @param listener the listener to add.
     */
    public void addMessageListener(MessageListener listener);

    /**
     * Remove a listener from the listener list.
     * @param listener the listener to remove.
     */
    public void removeMessageListener(MessageListener listener);

    /**
     * Notify listeners after setting the message.
     * @param message the message to set.
     */
    public void sendMessage(Message message);
}
