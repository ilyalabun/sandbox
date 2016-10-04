package me.labuily;

import org.objectweb.asm.*;

/**
 * @author Ilya Labun
 */
public class SynchMonitorClassVisitor extends ClassVisitor {

    private final String targetClassName;

    private String className;

    public SynchMonitorClassVisitor(ClassVisitor cv, String targetClassName) {
        super(Opcodes.ASM5, cv);
        this.targetClassName = targetClassName;
    }

    @Override
    public void visit(int version, int access, String name, String signature, String superName, String[] interfaces) {
        super.visit(version, access, name, signature, superName, interfaces);
        className = name;
    }

    @Override
    public MethodVisitor visitMethod(int access, String name, String desc, String signature, String[] exceptions) {
        final boolean isSynchronized = (access & Opcodes.ACC_SYNCHRONIZED) != 0;
        final boolean shouldPrinMonitorLogForMethod = isSynchronized && className.equals(targetClassName);
        final MethodVisitor methodVisitor = super.visitMethod(access, name, desc, signature, exceptions);
        return new SynchMonitorMethodVisitor(api, methodVisitor, className, name + desc, targetClassName, shouldPrinMonitorLogForMethod);
    }
}
