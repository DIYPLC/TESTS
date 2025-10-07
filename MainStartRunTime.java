package JavaSoftPlc; // Согласно иерархии папок.

/**
 * @author VA
 * Документационный комментарий для класса.
 * ЧЕРНОВИК !!!
 * Первая попытка поизучать яву.
 * IntelliJ IDEA 2025.2.1 (Community Edition)
 * https://www.jetbrains.com/idea/download/?section=windows
 * Уровень 1. Язык программирования Java - 1 часть.
 * https://rutube.ru/video/14ca4d060c88846fd55aaf90b9d80022/
 */
public class MainStartRunTime { // Один файл один одноименный класс.


    boolean flag = false;
    byte int8 =0;
    short int16 = 0;
    int int32 = 0;
    long int64 = 0;
    final int MAGIC_CONSTANT = 314; // Magic constant.
    float float32 = 0.0f;
    double float64 = 0.0;
    String str1 = "UTF-16 string 16 bit char";
    static long time_current_ms = 0; // Мне не нравится верблюжья аннотация timeCurrentMs
    static long time_previous_ms = 0;
    public static long uptime_ms = 0;
    public static long time_sample_ms = 0;
    public static double time_sample_s = 0;
    public static boolean reset = false;


public void task1(boolean Reset, long Ts_ms, double Ts){
    uptime_ms = uptime_ms + Ts_ms;
    System.out.println("Uptime [s] = " + (double)uptime_ms / 1000);
    //System.out.println("Ts [s] = " + Ts);
    return;
}


void main(String[] args) throws InterruptedException {
    System.out.println("[OK] START");
    System.out.println("sin(pi/2) = " + Math.sin(Math.PI/2) );

    time_current_ms = System.currentTimeMillis();
    time_previous_ms = time_current_ms;
    task1(true,0, 0.0);
    boolean plc_run = true;
    while (plc_run){
        time_current_ms = System.currentTimeMillis();
        if (time_current_ms >= time_previous_ms){
            time_sample_ms = time_current_ms - time_previous_ms;
        }
        time_sample_s = ((double) time_sample_ms) / 1000.0;
        task1(false, time_sample_ms, time_sample_s);
        time_previous_ms = time_current_ms;
        Thread.sleep(1000); // ms delay
    }

    System.out.println("[OK] STOP.");
    System.exit(0);
}


}