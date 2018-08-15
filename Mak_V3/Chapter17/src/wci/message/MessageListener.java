package wci.message;

/**
 * <h1>MessageListener</h1>
 *
 * <p>All classes that listen to messages must implement this interface.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public interface MessageListener
{
    /**
     * Called to receive a message sent by a message producer.
     * @param message the message that was sent.
     */
    public void messageReceived(Message message);
}
