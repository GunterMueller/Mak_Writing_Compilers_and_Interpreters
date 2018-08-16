package wci.message;

/**
 * <h1>Message</h1>
 *
 * <p>Message format.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class Message
{
    private MessageType type;
    private Object body;

    /**
     * Constructor.
     * @param type the message type.
     * @param body the message body.
     */
    public Message(MessageType type, Object body)
    {
        this.type = type;
        this.body = body;
    }

    /**
     * Getter.
     * @return the message type.
     */
    public MessageType getType()
    {
        return type;
    }

    /**
     * Getter.
     * @return the message body.
     */
    public Object getBody()
    {
        return body;
    }
}
