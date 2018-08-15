package wci.backend;

import wci.backend.compiler.CodeGenerator;
import wci.backend.interpreter.Executor;
import wci.intermediate.TypeSpec;
import wci.intermediate.symtabimpl.Predefined;

/**
 * <h1>BackendFactory</h1>
 *
 * <p>A factory class that creates compiler and interpreter components.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class BackendFactory
{
    /**
     * Create a compiler or an interpreter back end component.
     * @param operation either "compile" or "execute"
     * @return a compiler or an interpreter back end component.
     * @throws Exception if an error occurred.
     */
    public static Backend createBackend(String operation)
        throws Exception
    {
        if (operation.equalsIgnoreCase("compile")) {
            return new CodeGenerator();
        }
        else if (operation.equalsIgnoreCase("execute")) {
            return new Executor();
        }
        else {
            throw new Exception("Backend factory: Invalid operation '" +
                                operation + "'");
        }
    }

    /**
     * Return the default value for a data type.
     * @param type the data type.
     * @return the type descriptor.
     */
    public static Object defaultValue(TypeSpec type)
    {
        type = type.baseType();

        if (type == Predefined.integerType) {
            return new Integer(0);
        }
        else if (type == Predefined.realType) {
            return new Float(0.0f);
        }
        else if (type == Predefined.booleanType) {
            return new Boolean(false);
        }
        else if (type == Predefined.charType) {
            return new Character('#');
        }
        else /* string */ {
            return new String("#");
        }
    }
}
