package me.labuily;

import java.lang.instrument.Instrumentation;

/**
 * @author Ilya Labun
 */
public class SynchMonitorAgent {
    public static void premain(String agentArgs, Instrumentation inst) {
        final String[] splittedParameters = agentArgs.split(",");
        final String patchClassPrefix = splittedParameters[0];
        final String targetClassName = splittedParameters[1];
        boolean dumpClassFiles = false;
        if (splittedParameters.length > 2) {
            dumpClassFiles = Boolean.parseBoolean(splittedParameters[2]);
        }
        inst.addTransformer(new SynchMonitorTransformer(patchClassPrefix, targetClassName, dumpClassFiles));
    }
}
