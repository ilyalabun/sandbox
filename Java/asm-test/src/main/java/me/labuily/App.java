package me.labuily;

public class App 
{
    public static void main( String[] args )
    {
        final App o = new App();
        synchronized (o) {
            System.out.println("In da App sync!");
        }

        final Object sync2 = new Object();
        synchronized (sync2) {
            System.out.println("In da Object sync!");
        }

        o.doSync();
        o.doNotSync();
    }


    public synchronized void doSync() {
        System.out.println("In da doSync method!");
        synchronized (this) {
            System.out.println("In da second synchronized!");
        }
    }

    public void doNotSync() {
        System.out.println("In da doNotSync method!");
    }
}
