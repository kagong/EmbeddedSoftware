package com.example.androidex;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;



public class MyCounterService extends Service{

	int count;
	Timer timer = null;
    IBinder mBinder = new MyBinder();
    Thread thd;
    class MyBinder extends Binder {
        MyCounterService getService() { // 서비스 객체를 리턴
            return MyCounterService.this;
        }
    }
    private class Timer implements Runnable {

    	boolean running = true;
    	public void run() {
    		running = true;
    		while (running) { 
    			try { count++;Thread.sleep(100); } catch (InterruptedException e) { e.printStackTrace(); }
    		}
    	}
    	public void stop(){
    		running = false;
    	}
    }
    @Override
    public IBinder onBind(Intent intent) {
        // 액티비티에서 bindService() 를 실행하면 호출됨
        // 리턴한 IBinder 객체는 서비스와 클라이언트 사이의 인터페이스 정의한다
        return mBinder; // 서비스 객체를 리턴
    }
    public boolean onUnbind(Intent intent) { 
    	 return super.onUnbind(intent); 
    }

    
    void startTimer() { // 임의 랜덤값을 리턴하는 메서드
    	count=0;
    	timer = new Timer();
    	thd = new Thread(timer);
    	thd.start();
    }
    void stopTimer() { // 임의 랜덤값을 리턴하는 메서드
    	if(timer != null)
    		timer.stop();
    	
    }
    String getTimeString(){
    	String min = Integer.toString((count/10)/60),sec =Integer.toString((count/10)%60);
    	if(min.length() == 1)
    		min = "0" + min;
    	if(sec.length() == 1)
    		sec = "0" + sec;
    		
    	return min + ":" + sec;
    }
    @Override
    public void onCreate() {
        super.onCreate();

    	
    }
    public int onStartCommand(Intent intent, int flags, int startId) {
        return super.onStartCommand(intent, flags, startId);
    }
    @Override
    public void onDestroy() {
        super.onDestroy();
    }
   
}




