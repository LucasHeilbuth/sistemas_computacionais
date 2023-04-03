public class RunnableExample implements Runnable {
    static int sum =0;

    public static void main(String[] args) {
        System.out.println("Inside : " + Thread.currentThread().getName());

        System.out.println("Creating Runnable...");
        Runnable runnable = new RunnableExample();

        System.out.println("Creating Thread1...");
        Thread thread1 = new Thread(runnable);
        
        System.out.println("Creating Thread2...");
        Thread thread2 = new Thread(runnable);

        System.out.println("Starting Thread1...");
        thread1.start();
        System.out.println("Starting Thread2...");
        thread2.start();
        // Print only when the two threads are dead
        try {
            thread1.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        try {
            thread2.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        System.out.println(sum);
    }

    @Override
    public void run() {
    	for(int i = 0; i < 1000; i++) {
  	sum = sum + 1;
}
    }
}
