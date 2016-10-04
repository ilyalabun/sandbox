package me.labuily;

import org.objectweb.asm.*;

/**
 * @author Ilya Labun
 */
public class SynchMonitorMethodVisitor extends MethodVisitor {

    private final String className;

    private final String methodName;

    private final String targetClassName;

    private final boolean shouldPrintMonitorLogForMethod;


    public SynchMonitorMethodVisitor(int api, MethodVisitor methodVisitor,
                                     String className, String name, String targetClassName,
                                     boolean shouldPrintMonitorLogForMethod) {
        super(api, methodVisitor);
        this.className = className;
        this.methodName = name;
        this.targetClassName = targetClassName;
        this.shouldPrintMonitorLogForMethod = shouldPrintMonitorLogForMethod;
    }

    @Override
    public void visitCode() {
        if (shouldPrintMonitorLogForMethod) {
            printLogMessage("Called synchronized method " + className + "::" + methodName);
        }
        super.visitCode();
    }

    @Override
    public void visitInsn(int opcode) {
        if (Opcodes.MONITORENTER == opcode) {
            mv.visitInsn(Opcodes.DUP);
            mv.visitTypeInsn(Opcodes.INSTANCEOF, targetClassName);
            final Label ifNotInstanceOf = new Label();
            mv.visitJumpInsn(Opcodes.IFEQ, ifNotInstanceOf);
            printLogMessage("Synchronized on " + targetClassName + " at " + className + "::" + methodName);

            mv.visitLabel(ifNotInstanceOf);
            mv.visitInsn(Opcodes.MONITORENTER);
        } else {
            mv.visitInsn(opcode);
        }
    }

    private void printLogMessage(String logMessage) {
        mv.visitFieldInsn(Opcodes.GETSTATIC, "java/lang/System", "err", "Ljava/io/PrintStream;");
        mv.visitLdcInsn(logMessage);
        mv.visitMethodInsn(Opcodes.INVOKEVIRTUAL, "java/io/PrintStream", "println", "(Ljava/lang/String;)V", false);
    }
}
