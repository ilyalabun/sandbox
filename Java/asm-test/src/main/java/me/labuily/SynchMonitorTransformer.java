package me.labuily;

import org.objectweb.asm.ClassReader;
import org.objectweb.asm.ClassWriter;

import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.instrument.ClassFileTransformer;
import java.lang.instrument.IllegalClassFormatException;
import java.security.ProtectionDomain;

/**
 * @author Ilya Labun
 */
public class SynchMonitorTransformer implements ClassFileTransformer {
    private final String patchClassPrefix;
    private final String targetClassName;
    private final boolean dumpClassFiles;

    public SynchMonitorTransformer(String patchClassPrefix, String targetClassName, boolean dumpClassFiles) {
        this.patchClassPrefix = patchClassPrefix;
        this.targetClassName = targetClassName;
        this.dumpClassFiles = dumpClassFiles;
    }

    @Override
    public byte[] transform(ClassLoader classLoader, String s, Class<?> aClass, ProtectionDomain protectionDomain, byte[] bytes) throws IllegalClassFormatException {
        if (!s.startsWith(patchClassPrefix)) {
            return bytes;
        }
        final ClassReader classReader = new ClassReader(bytes);
        final ClassWriter classWriter = new ClassWriter(classReader, ClassWriter.COMPUTE_FRAMES);
        classReader.accept(new SynchMonitorClassVisitor(classWriter, targetClassName), 0);
        final byte[] newBytecode = classWriter.toByteArray();
        dumpToFile(s, newBytecode);
        return newBytecode;
    }

    private void dumpToFile(String className, byte[] newBytecode) {
        if (dumpClassFiles) {
            try (final FileOutputStream fos = new FileOutputStream(("./patchedClasses/" + className.replace('/', '.') + ".class"))) {
                fos.write(newBytecode);
            } catch (IOException e) {
            }
        }
    }
}
