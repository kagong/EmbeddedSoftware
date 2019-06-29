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
        MyCounterService getService() { // return service obj
            return MyCounterService.this;
        }
    }
    //this class increase timer 
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
        return mBinder; // return service binder
    }
    public boolean onUnbind(Intent intent) { 
    	 return super.onUnbind(intent); 
    }

    
    void startTimer() { // run timer thread
    	count=0;
    	timer = new Timer();
    	thd = new Thread(timer);
    	thd.start();
    }
    void stopTimer() { // exit timer thread
    	if(timer != null)
    		timer.stop();
    	
    }
    //this function is called by activity
    //return time
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




