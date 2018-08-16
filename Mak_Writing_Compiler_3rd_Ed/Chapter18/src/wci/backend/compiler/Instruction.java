package wci.backend.compiler;

/**
 * <h1>Instruction</h1>
 *
 * <p>Jasmin instructions.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public enum Instruction
{
    // Load constant
    ICONST_0, ICONST_1, ICONST_2, ICONST_3, ICONST_4, ICONST_5, ICONST_M1,
    FCONST_0, FCONST_1, FCONST_2, ACONST_NULL,
    BIPUSH, SIPUSH, LDC,

    // Load value or address
    ILOAD_0, ILOAD_1, ILOAD_2, ILOAD_3,
    FLOAD_0, FLOAD_1, FLOAD_2, FLOAD_3,
    ALOAD_0, ALOAD_1, ALOAD_2, ALOAD_3,
    ILOAD, FLOAD, ALOAD,
    GETSTATIC, GETFIELD,

    // Store value or address
    ISTORE_0, ISTORE_1, ISTORE_2, ISTORE_3,
    FSTORE_0, FSTORE_1, FSTORE_2, FSTORE_3,
    ASTORE_0, ASTORE_1, ASTORE_2, ASTORE_3,
    ISTORE, FSTORE, ASTORE,
    PUTSTATIC, PUTFIELD,

    // Operand stack
    POP, SWAP, DUP,

    // Arithmetic and logical
    IADD, FADD, ISUB, FSUB, IMUL, FMUL,
    IDIV, FDIV, IREM, FREM, INEG, FNEG,
    IINC, IAND, IOR, IXOR,

    // Type conversion and checking
    I2F, I2C, I2D, F2I, F2D, D2F,
    CHECKCAST,

    // Objects and arrays
    NEW, NEWARRAY, ANEWARRAY, MULTIANEWARRAY,
    IALOAD, FALOAD, BALOAD, CALOAD, AALOAD,
    IASTORE, FASTORE, BASTORE, CASTORE, AASTORE,

    // Compare and branch
    IFEQ, IFNE, IFLT, IFLE, IFGT, IFGE,
    IF_ICMPEQ, IF_ICMPNE, IF_ICMPLT, IF_ICMPLE, IF_ICMPGT, IF_ICMPGE,
    FCMPG, GOTO, LOOKUPSWITCH,

    // Call and return
    INVOKESTATIC, INVOKEVIRTUAL, INVOKENONVIRTUAL,
    RETURN, IRETURN, FRETURN, ARETURN,

    // No operation
    NOP;

    /**
     * @return the string that is emitted.
     */
    public String toString()
    {
        return super.toString().toLowerCase();
    }
}
