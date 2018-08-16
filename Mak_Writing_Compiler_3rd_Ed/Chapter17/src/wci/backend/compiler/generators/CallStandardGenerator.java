package wci.backend.compiler.generators;

import java.util.ArrayList;

import wci.frontend.*;
import wci.intermediate.*;
import wci.intermediate.symtabimpl.*;
import wci.backend.compiler.*;

import static wci.frontend.pascal.PascalTokenType.*;
import static wci.intermediate.symtabimpl.SymTabKeyImpl.*;
import static wci.intermediate.symtabimpl.RoutineCodeImpl.*;
import static wci.intermediate.typeimpl.TypeKeyImpl.*;
import static wci.intermediate.typeimpl.TypeFormImpl.*;
import static wci.intermediate.icodeimpl.ICodeNodeTypeImpl.*;
import static wci.intermediate.icodeimpl.ICodeKeyImpl.*;
import static wci.backend.compiler.Instruction.*;

/**
 * <h1>CallStandardGenerator</h1>
 *
 * <p>Generate code to call a standard procedure or function.</p>
 *
 * <p>Copyright (c) 2009 by Ronald Mak</p>
 * <p>For instructional purposes only.  No warranties.</p>
 */
public class CallStandardGenerator extends CallGenerator
{
    private ExpressionGenerator exprGenerator;

    /**
     * Constructor.
     * @param the parent executor.
     */
    public CallStandardGenerator(CodeGenerator parent)
    {
        super(parent);
    }

    /**
     * Generate code to call to a standard procedure or function.
     * @param node the CALL node.
     * @return the function value, or null for a procedure call.
     */
    public void generate(ICodeNode node)
    {
        SymTabEntry routineId = (SymTabEntry) node.getAttribute(ID);
        RoutineCode routineCode =
                        (RoutineCode) routineId.getAttribute(ROUTINE_CODE);
        exprGenerator = new ExpressionGenerator(this);
        ICodeNode actualNode = null;

        // Get the actual parameters of the call.
        if (node.getChildren().size() > 0) {
            ICodeNode parmsNode = node.getChildren().get(0);
            actualNode = parmsNode.getChildren().get(0);
        }

        switch ((RoutineCodeImpl) routineCode) {
            case READ:
            case READLN:  generateReadReadln(node, routineCode); break;

            case WRITE:
            case WRITELN: generateWriteWriteln(node, routineCode); break;

            case EOF:
            case EOLN:    generateEofEoln(node, routineCode); break;

            case ABS:
            case SQR:     generateAbsSqr(routineCode, actualNode); break;

            case ARCTAN:
            case COS:
            case EXP:
            case LN:
            case SIN:
            case SQRT:    generateArctanCosExpLnSinSqrt(routineCode,
                                                        actualNode);
                          break;

            case PRED:
            case SUCC:    generatePredSucc(routineCode, actualNode); break;

            case CHR:     generateChr(actualNode); break;
            case ODD:     generateOdd(actualNode); break;
            case ORD:     generateOrd(actualNode); break;

            case ROUND:
            case TRUNC:   generateRoundTrunc(routineCode, actualNode); break;
        }
    }

    /**
     * Generate code for a call to read or readln.
     * @param callNode the CALL node.
     * @param routineCode the routine code.
     */
    private void generateReadReadln(ICodeNode callNode,
                                    RoutineCode routineCode)
    {
        ICodeNode parmsNode = callNode.getChildren().size() > 0
                                  ? callNode.getChildren().get(0)
                                  : null;
        String programName = symTabStack.getProgramId().getName();
        String standardInName = programName + "/_standardIn";

        if (parmsNode != null) {
            ArrayList<ICodeNode> actuals = parmsNode.getChildren();

            // Loop to process each actual parameter.
            for (ICodeNode actualNode : actuals) {
                SymTabEntry variableId =
                                (SymTabEntry) actualNode.getAttribute(ID);
                TypeSpec actualType = actualNode.getTypeSpec();
                TypeSpec baseType = actualType.baseType();

                // Generate code to call the appropriate PascalTextIn method.
                emit(GETSTATIC, standardInName, "LPascalTextIn;");
                if (baseType == Predefined.integerType) {
                    emit(INVOKEVIRTUAL, "PascalTextIn.readInteger()I");
                }
                else if (baseType == Predefined.realType) {
                    emit(INVOKEVIRTUAL, "PascalTextIn.readReal()F");
                }
                else if (baseType == Predefined.booleanType) {
                    emit(INVOKEVIRTUAL, "PascalTextIn.readBoolean()Z");
                }
                else if (baseType == Predefined.charType) {
                    emit(INVOKEVIRTUAL, "PascalTextIn.readChar()C");
                }

                localStack.increase(1);

                // Store the value that was read into the actual parameter.
                emitStoreVariable(variableId);

                localStack.decrease(1);
            }
        }

        // READLN: Skip the rest of the input line.
        if (routineCode == READLN) {
            emit(GETSTATIC, standardInName, "LPascalTextIn;");
            emit(INVOKEVIRTUAL, "PascalTextIn.nextLine()V");

            localStack.use(1);
        }
    }

    /**
     * Generate code for a call to eof or eoln.
     * @param callNode the CALL node.
     * @param routineCode the routine code.
     */
    private void generateEofEoln(ICodeNode callNode, RoutineCode routineCode)
    {
        String programName = symTabStack.getProgramId().getName();
        String standardInName = programName + "/_standardIn";

        // Generate code to call the appropriate PascalTextIn method.
        emit(GETSTATIC, standardInName, "LPascalTextIn;");
        if (routineCode == EOLN) {
            emit(INVOKEVIRTUAL, "PascalTextIn.atEoln()Z");
        }
        else {
            emit(INVOKEVIRTUAL, "PascalTextIn.atEof()Z");
        }

        localStack.increase(1);
    }

    /**
     * Generate code for a call to write or writeln.
     * @param callNode the CALL node.
     * @param routineCode the routine code.
     */
    private void generateWriteWriteln(ICodeNode callNode,
                                      RoutineCode routineCode)
    {
        ICodeNode parmsNode = callNode.getChildren().size() > 0
                                  ? callNode.getChildren().get(0)
                                  : null;
        StringBuilder buffer = new StringBuilder();
        int exprCount = 0;

        buffer.append("\"");

        // There are actual parameters.
        if (parmsNode != null) {
            ArrayList<ICodeNode> actuals = parmsNode.getChildren();

            // Loop to process each WRITE parameter
            // and build the format string.
            for (ICodeNode writeParmNode : actuals) {
                ArrayList<ICodeNode> children = writeParmNode.getChildren();
                ICodeNode exprNode = children.get(0);
                ICodeNodeType nodeType = exprNode.getType();

                // Append string constants directly to the format string.
                if (nodeType == STRING_CONSTANT) {
                    String str = (String)exprNode.getAttribute(VALUE);
                    buffer.append(str.replaceAll("%", "%%"));
                }

                // Create and append the appropriate format specification.
                else {
                    TypeSpec dataType = exprNode.getTypeSpec().baseType();
                    String typeCode = dataType.isPascalString()          ? "s"
                                    : dataType == Predefined.integerType ? "d"
                                    : dataType == Predefined.realType    ? "f"
                                    : dataType == Predefined.booleanType ? "s"
                                    : dataType == Predefined.charType    ? "c"
                                    :                                      "s";

                    ++exprCount;  // count the non-constant string parameters
                    buffer.append("%");

                    // Process any field width and precision values.
                    if (children.size() > 1) {
                        int w = (Integer) children.get(1).getAttribute(VALUE);
                        buffer.append(w == 0 ? 1 : w);
                    }
                    if (children.size() > 2) {
                        int p = (Integer) children.get(2).getAttribute(VALUE);
                        buffer.append(".");
                        buffer.append(p == 0 ? 1 : p);
                    }

                    buffer.append(typeCode);
                }
            }

            buffer.append(routineCode == WRITELN ? "\\n\"" : "\"");
        }

        emit(GETSTATIC, "java/lang/System/out", "Ljava/io/PrintStream;");
        localStack.increase(1);

        // WRITELN with no parameters.
        if (parmsNode == null) {
            emit(INVOKEVIRTUAL, "java/io/PrintStream.println()V");
            localStack.decrease(1);
        }

        // WRITE or WRITELN with parameters.
        else {
            ArrayList<ICodeNode> actuals = parmsNode.getChildren();

            // Load the format string.
            emit(LDC, buffer.toString());
            localStack.increase(1);

            // Generate code to create the values array for String.format().
            if (exprCount > 0) {
                emitLoadConstant(exprCount);
                emit(ANEWARRAY, "java/lang/Object");
                localStack.use(3, 1);

                int index = 0;
                ExpressionGenerator exprGenerator =
                                        new ExpressionGenerator(this);

                // Loop to generate code to evaluate each actual parameter.
                for (ICodeNode writeParmNode : actuals) {
                    ArrayList<ICodeNode> children = writeParmNode.getChildren();
                    ICodeNode exprNode = children.get(0);
                    ICodeNodeType nodeType = exprNode.getType();
                    TypeSpec dataType = exprNode.getTypeSpec().baseType();

                    // Skip string constants, which were made part of
                    // the format string.
                    if (nodeType != STRING_CONSTANT) {
                        emit(DUP);
                        emitLoadConstant(index++);
                        localStack.increase(2);

                        exprGenerator.generate(exprNode);

                        String signature = dataType.getForm() == SCALAR
                            ? valueOfSignature(dataType)
                            : null;

                        // Boolean: Write "true" or "false".
                        if (dataType == Predefined.booleanType) {
                            Label trueLabel = Label.newLabel();
                            Label nextLabel = Label.newLabel();
                            emit(IFNE, trueLabel);
                            emit(LDC, "\"false\"");
                            emit(Instruction.GOTO, nextLabel);
                            emitLabel(trueLabel);
                            emit(LDC, "\"true\"");
                            emitLabel(nextLabel);

                            localStack.use(1);
                        }

                        // Convert a scalar value to an object.
                        if (signature != null) {
                            emit(INVOKESTATIC, signature);
                        }

                        // Store the value into the values vector.
                        emit(AASTORE);
                        localStack.decrease(3);
                    }
                }

                // Format the string.
                emit(INVOKESTATIC,
                     "java/lang/String/format(Ljava/lang/String;" +
                     "[Ljava/lang/Object;)Ljava/lang/String;");
                localStack.decrease(2);
            }

            // Print.
            emit(INVOKEVIRTUAL,
                 "java/io/PrintStream.print(Ljava/lang/String;)V");
            localStack.decrease(2);
        }
    }

    /**
     * Generate code for a call to abs or sqr.
     * @param routineCode the routine code.
     * @param actualNode the actual parameter node.
     */
    private void generateAbsSqr(RoutineCode routineCode, ICodeNode actualNode)
    {
        exprGenerator.generate(actualNode);

        // ABS: Generate code to call the appropriate integer or float
        //      java.lang.Math method.
        // SQR: Multiply the value by itself.
        if (actualNode.getTypeSpec() == Predefined.integerType) {
            if (routineCode == ABS) {
                emit(INVOKESTATIC, "java/lang/Math/abs(I)I");
            }
            else {
                emit(DUP);
                emit(IMUL);
                localStack.use(1);
            }
        }
        else {
            if (routineCode == ABS) {
                emit(INVOKESTATIC, "java/lang/Math/abs(F)F");
            }
            else {
                emit(DUP);
                emit(FMUL);
                localStack.use(1);
            }
        }
    }

    /**
     * Generate code for a call to arctan, cos, exp, ln, sin, or sqrt.
     * @param routineCode the routine code.
     * @param actualNode the actual parameter node.
     */
    private void generateArctanCosExpLnSinSqrt(RoutineCode routineCode,
                                               ICodeNode actualNode)
    {
        String function = null;
        exprGenerator.generate(actualNode);

        // Convert an integer or real value to double.
        TypeSpec actualType = actualNode.getTypeSpec();
        if (actualType == Predefined.integerType) {
            emit(I2D);
        }
        else {
            emit(F2D);
        }

        // Select the appropriate java.lang.Math method.
        switch ((RoutineCodeImpl) routineCode) {
            case ARCTAN: function = "atan"; break;
            case COS:    function = "cos";  break;
            case EXP:    function = "exp";  break;
            case SIN:    function = "sin";  break;
            case LN:     function = "log";  break;
            case SQRT:   function = "sqrt"; break;
        }

        // Call the method and convert the result from double to float.
        emit(INVOKESTATIC, "java/lang/Math/" + function + "(D)D");
        emit(D2F);

        localStack.use(1);
    }

    /**
     * Generate code for a call to pred or succ.
     * @param routineCode the routine code.
     * @param actualNode the actual parameter node.
     */
    private void generatePredSucc(RoutineCode routineCode, ICodeNode actualNode)
    {
        // Generate code to add or subtract 1 from the value.
        exprGenerator.generate(actualNode);
        emit(ICONST_1);
        emit(routineCode == PRED ? ISUB : IADD);

        localStack.use(1);
    }

    /**
     * Generate code for a call to chr.
     * @param actualNode the actual parameter node.
     */
    private void generateChr(ICodeNode actualNode)
    {
        exprGenerator.generate(actualNode);
        emit(I2C);
    }

    /**
     * Generate code for a call to odd.
     * @param actualNode the actual parameter node.
     */
    private void generateOdd(ICodeNode actualNode)
    {
        // Generate code to leave the rightmost bit of the value
        // on the operand stack.
        exprGenerator.generate(actualNode);
        emit(ICONST_1);
        emit(IAND);

        localStack.use(1);
    }

    /**
     * Generate code for a call to ord.
     * @param actualNode the actual parameter node.
     */
    private void generateOrd(ICodeNode actualNode)
    {
        // A character value is treated as an integer value.
        if (actualNode.getType() == STRING_CONSTANT) {
            int value = ((String) actualNode.getAttribute(VALUE)).charAt(0);
            emitLoadConstant(value);
            localStack.increase(1);
        }
        else {
            exprGenerator.generate(actualNode);
        }
    }

    /**
     * Generate code for a call to round or trunc.
     * @param routineCode the routine code.
     * @param actualNode the actual parameter node.
     */
    private void generateRoundTrunc(RoutineCode routineCode,
                                    ICodeNode actualNode)
    {
        exprGenerator.generate(actualNode);

        // ROUND: Generate code to compute floor(value + 0.5).
        if (routineCode == ROUND) {
            emitLoadConstant(0.5f);
            emit(FADD);
            localStack.use(1);
        }

        // Truncate.
        emit(F2I);
    }
}
